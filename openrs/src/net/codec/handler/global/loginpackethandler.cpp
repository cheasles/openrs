#include "openrs/net/codec/handler/global/loginpackethandler.h"

#include <endian.h>
#include <integer.h>
#include <modes.h>
#include <openrs/common/crypt/xtea.h>
#include <openrs/common/log.h>
#include <openrs/database/models/player.h>
#include <openrs/game/player.h>
#include <rsa.h>

#include <string>

#include "openrs/manager/cache/cachemanager.h"
#include "openrs/manager/cache/grabmanager.h"
#include "openrs/manager/configmanager.h"
#include "openrs/manager/databasemanager.h"

void HandleLoginWorld(openrs::net::codec::Packet& packet,
                      openrs::net::Session* session) {
  packet.data.seek(SEEK_CUR, sizeof(uint8_t));
  uint16_t* rsa_block_size_ptr = nullptr;
  if (!packet.data.GetData(&rsa_block_size_ptr)) {
    return;
  }
  const uint16_t kRsaBlockSize = ::be16toh(*rsa_block_size_ptr);
  if (kRsaBlockSize > packet.data.remaining()) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  CryptoPP::RSA::PublicKey rsa_function;
  const auto& login_config = openrs::manager::ConfigManager::get()["login"];
  CryptoPP::Integer private_exponent(
      login_config["private_exponent"].get<std::string>().c_str());
  CryptoPP::Integer modulus(login_config["modulus"].get<std::string>().c_str());
  rsa_function.Initialize(modulus, private_exponent);
  const auto decrypted_packet_int =
      rsa_function.ApplyFunction(CryptoPP::Integer(
          packet.data.data() + packet.data.position(), kRsaBlockSize));
  packet.data.seek(SEEK_CUR, kRsaBlockSize);
  openrs::common::io::Buffer<> decrypted_packet;
  decrypted_packet.resize(decrypted_packet_int.MinEncodedSize());
  decrypted_packet_int.Encode(decrypted_packet.data(), decrypted_packet.size());

  uint8_t* rsa_block_header_ptr = nullptr;
  if (!decrypted_packet.GetData(&rsa_block_header_ptr) ||
      *rsa_block_header_ptr != 10) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  if (decrypted_packet.remaining() < CryptoPP::XTEA::KEYLENGTH) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }
  const uint8_t* xtea_keys =
      decrypted_packet.data() + decrypted_packet.position();
  decrypted_packet.seek(SEEK_CUR, CryptoPP::XTEA::KEYLENGTH);

  uint64_t* rsa_block_ptr = nullptr;
  if (!decrypted_packet.GetData(&rsa_block_ptr) || *rsa_block_ptr != 0) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  std::string password;
  if (!decrypted_packet.GetString(&password) || password.size() < 3 ||
      password.size() > 30) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorInvalidLogin);
    return;
  }

  // Decode XTEA block from the packet.
  openrs::common::io::Buffer<> decoded_packet;
  openrs::common::crypt::DecryptXTEA(
      xtea_keys, CryptoPP::XTEA::KEYLENGTH,
      packet.data.data() + packet.data.position(),
      packet.data.remaining() -
          (packet.data.remaining() % CryptoPP::XTEA::BLOCKSIZE),
      &decoded_packet);

  uint8_t* username_header_ptr = nullptr;
  if (!decoded_packet.GetData(&username_header_ptr) ||
      *username_header_ptr != 1) {
    // TODO: Handle 'long' usernames for when *username_header_ptr != 1.
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  std::string username;
  if (!decoded_packet.GetString(&username)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorInvalidLogin);
    return;
  }

  uint8_t* display_mode_ptr = nullptr;
  uint16_t* screen_width_ptr = nullptr;
  uint16_t* screen_height_ptr = nullptr;
  if (!decoded_packet.GetData(&display_mode_ptr) ||
      !decoded_packet.GetData(&screen_width_ptr) ||
      !decoded_packet.GetData(&screen_height_ptr)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  // Skip unknown byte.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t));
  // Skip unknown 24 bytes.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t) * 24);

  std::string settings;
  if (!decoded_packet.GetString(&settings)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  // Skip unknown int.
  decoded_packet.seek(SEEK_CUR, sizeof(uint32_t));

  // Skip variable length block.
  uint8_t* block_length_ptr = nullptr;
  if (!decoded_packet.GetData(&block_length_ptr)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }
  decoded_packet.seek(SEEK_CUR, *block_length_ptr);

  // Skip unknown int.
  decoded_packet.seek(SEEK_CUR, sizeof(uint32_t));
  // Skip unknown long.
  decoded_packet.seek(SEEK_CUR, sizeof(uint64_t));

  uint8_t* has_additional_info_ptr = nullptr;
  if (!decoded_packet.GetData(&has_additional_info_ptr)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }
  if (*has_additional_info_ptr == 1) {
    std::string additional_information;
    if (!decoded_packet.GetString(&additional_information)) {
      session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
      return;
    }
  }

  // Skip unknown byte.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t));
  // Skip unknown byte.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t));
  // Skip unknown byte.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t));
  // Skip unknown byte.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t));
  // Skip unknown int.
  decoded_packet.seek(SEEK_CUR, sizeof(uint32_t));
  // Skip unknown string.
  if (!decoded_packet.GetString(&settings)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }
  // Skip unknown byte.
  decoded_packet.seek(SEEK_CUR, sizeof(uint8_t));

  // Validate cache hashes.
  const auto& cache = openrs::manager::cache::CacheManager::get().cache();
  for (uint32_t index = 0;
       index < std::min(static_cast<uint32_t>(32), cache->GetTypeCount());
       ++index) {
    const auto& store_crc = cache->GetStore(index).crc();
    uint32_t* crc_ptr = nullptr;
    if (!decoded_packet.GetData(&crc_ptr)) {
      session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
      return;
    }
    if (store_crc != ::be32toh(*crc_ptr)) {
      session->SendOpCode(openrs::net::codec::PacketType::kClientOutdated);
      return;
    }
  }

  // TODO: Validate username.
  openrs::game::Player player;
  auto& database_manager = openrs::manager::DatabaseManager::get();
  std::vector<openrs::database::models::PlayerModel> players;
  if (!database_manager.GetModel<openrs::database::models::PlayerModel>(
          &players)) {
    player.username = username;
    openrs::game::Player::GenerateRandomString(24, &player.salt);
    openrs::game::Player::EncodePassword(password, player.salt,
                                         &player.password);
    database_manager.CreateModel(player);
    openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
        << "Player " << username << " has registered.";
  } else {
    if (players.size() != 1) {
      session->SendOpCode(
          openrs::net::codec::PacketType::kErrorInvalidLoginServer);
      return;
    }

    player = players.at(0);

    // Validate the password.
    if (!player.CheckPassword(password)) {
      // TODO: Add IP to anti-spam filter.
      session->SendOpCode(openrs::net::codec::PacketType::kErrorInvalidLogin);
      return;
    }
  }

  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "Player " << player.username << " logged in.";
}

void openrs::net::codec::handler::global::LoginPacketHandler::Handle(
    openrs::net::codec::Packet& packet, openrs::net::Session* session) {
  session->ResetDecoder();

  uint16_t* packet_size_ptr = nullptr;
  if (!packet.data.GetData(&packet_size_ptr)) {
    return;
  }

  // Validate packet size against the packet buffer.
  const auto kPacketSize = ::be16toh(*packet_size_ptr);
  if (kPacketSize != packet.data.remaining()) {
    return;
  }

  uint32_t* client_build_ptr = nullptr;
  uint32_t* custom_client_build_ptr = nullptr;

  if (!packet.data.GetData<uint32_t>(&client_build_ptr) ||
      !packet.data.GetData<uint32_t>(&custom_client_build_ptr)) {
    return;
  }

  const uint32_t kClientBuild = ::be32toh(*client_build_ptr);

  // Validate the client version is supported.
  switch (kClientBuild) {
    case 718:
      session->set_client_build(kClientBuild);
      break;
    default:
      // Session version is not supported.
      session->ResetEncoder();
      session->SendOpCode(openrs::net::codec::PacketType::kClientOutdated);
      return;
  }

  if (PacketType::kLoginWorld == packet.type) {
    HandleLoginWorld(packet, session);
  } else if (PacketType::kLoginLobby == packet.type) {
  }
}
