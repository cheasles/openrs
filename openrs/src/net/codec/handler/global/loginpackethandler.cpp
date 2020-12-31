#include "openrs/net/codec/handler/global/loginpackethandler.h"

#include <endian.h>
#include <integer.h>
#include <modes.h>
#include <openrs/common/crypt/xtea.h>
#include <openrs/common/log.h>
#include <openrs/database/models/player.h>
#include <openrs/game/player.h>
#include <openrs/game/world.h>
#include <rsa.h>
#include <tea.h>

#include <limits>
#include <string>

#include "openrs/manager/cache/cachemanager.h"
#include "openrs/manager/cache/grabmanager.h"
#include "openrs/manager/configmanager.h"
#include "openrs/manager/databasemanager.h"
#include "openrs/manager/interfacemanager.h"
#include "openrs/manager/worldmanager.h"
#include "openrs/net/codec/decoder/global/worlddecoder.h"
#include "openrs/net/codec/encoder/global/worldencoder.h"
#include "openrs/net/codec/handler/global/worldpackethandler.h"

void SendLoginDetails(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session) {
  if (std::numeric_limits<uint8_t>::max() < player->rights) {
    throw new std::logic_error("Invalid player.rights value.");
  }
  if (std::numeric_limits<uint16_t>::max() < player->id) {
    throw new std::logic_error("Invalid player.id value.");
  }

  openrs::common::io::Buffer<> buffer;
  buffer.PutDataBE<uint8_t>(static_cast<uint8_t>(player->rights));
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE<uint8_t>(1);
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE(static_cast<uint16_t>(player->id));
  buffer.PutDataBE(static_cast<uint8_t>(
      openrs::manager::WorldManager::get().worlds().at(1).world_type()));
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE<uint8_t>(0);
  buffer.PutDataBE(static_cast<uint8_t>(
      openrs::manager::WorldManager::get().worlds().at(1).world_type()));
  buffer.PutString(player->username);

  openrs::net::codec::Packet details_packet;
  details_packet.type = openrs::net::codec::PacketType::kLoginDetails;
  details_packet.data = buffer;
  session->Send(details_packet);

  const auto& world_manager = openrs::manager::WorldManager::get();
  world_manager.SendMap(player, session, true);
  const auto& interface_manager = openrs::manager::InterfaceManager::get();
  interface_manager.SendInterfaces(player, session);
  world_manager.SendRunEnergy(player, session);
  world_manager.SendPlayerHitPoints(player, session);
}

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

  openrs::game::Player::DisplayMode* display_mode_ptr = nullptr;
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
  auto player = std::make_shared<openrs::game::Player>();
  auto& database_manager = openrs::manager::DatabaseManager::get();
  std::vector<openrs::database::models::PlayerModel> players;
  if (!database_manager.GetModel<openrs::database::models::PlayerModel>(
          &players)) {
    const auto& kStartingLocation =
        openrs::manager::ConfigManager::get()["game"]["starting_location"]
            .get<std::vector<uint32_t>>();
    player->username = username;
    openrs::game::Player::GenerateRandomString(24, &player->salt);
    openrs::game::Player::EncodePassword(password, player->salt,
                                         &player->password);
    player->set_x(kStartingLocation[0]);
    player->set_y(kStartingLocation[1]);
    player->set_z(kStartingLocation[2]);
    database_manager.CreateModel(*player);
    openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
        << "Player " << username << " has registered.";
  } else {
    if (players.size() != 1) {
      session->SendOpCode(
          openrs::net::codec::PacketType::kErrorInvalidLoginServer);
      return;
    }

    *player = players.at(0);

    // Validate the password.
    if (!player->CheckPassword(password)) {
      // TODO: Add IP to anti-spam filter.
      session->SendOpCode(openrs::net::codec::PacketType::kErrorInvalidLogin);
      return;
    }
  }

  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "Player " << player->username << " logged in.";
  player->set_display_mode(*display_mode_ptr);
  player->set_screen_width(::be16toh(*screen_width_ptr));
  player->set_screen_height(::be16toh(*screen_height_ptr));
  session->set_player(player);

  // Make sure the next packets are handled correctly.
  session->SetDecoder(
      std::make_unique<openrs::net::codec::decoder::global::WorldDecoder>());
  session->SetEncoder(
      std::make_unique<openrs::net::codec::encoder::global::WorldEncoder>());
  session->SetHandler(
      std::make_unique<
          openrs::net::codec::handler::global::WorldPacketHandler>());

  // Start the game.
  session->set_player_index(
      openrs::manager::WorldManager::get().add_player(1, player));
  SendLoginDetails(player, session);
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
