#include "OpenRS/net/codec/handler/global/loginpackethandler.h"

#include <endian.h>
#include <integer.h>
#include <modes.h>
#include <osrng.h>
#include <rsa.h>
#include <tea.h>

#include <string>

#include "OpenRS/manager/cache/cachemanager.h"
#include "OpenRS/manager/cache/grabmanager.h"
#include "OpenRS/manager/configmanager.h"
#include "common/log.h"

void DecodeXTEA(const std::vector<uint32_t>& keys,
                openrs::common::io::Buffer<>& input,
                openrs::common::io::Buffer<>* output) {
  uint32_t l = input.position();
  uint32_t i1 =
      input.remaining() - (input.remaining() % CryptoPP::XTEA::BLOCKSIZE);
  for (uint32_t j1 = 0; j1 < i1; j1++) {
    uint32_t* k1_ptr = nullptr;
    uint32_t* l1_ptr = nullptr;
    if (!input.GetData(&k1_ptr) || !input.GetData(&l1_ptr)) {
      return;
    }
    uint32_t k1 = ::be32toh(*k1_ptr);
    uint32_t l1 = ::be32toh(*l1_ptr);
    uint32_t sum = 0xc6ef3720;
    uint32_t delta = 0x9e3779b9;
    for (uint32_t k2 = 32; k2-- > 0;) {
      l1 -= keys[(sum & 0x1c84) >> 11] + sum ^ (k1 >> 5 ^ k1 << 4) + k1;
      sum -= delta;
      k1 -= (l1 >> 5 ^ l1 << 4) + l1 ^ keys[sum & 3] + sum;
    }

    output->PutDataBE(k1);
    output->PutDataBE(l1);
  }
}

void HandleLoginWorld(openrs::net::codec::Packet& packet,
                      openrs::net::Session* session) {
  packet.data.seek(std::ios_base::cur, sizeof(uint8_t));
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
  const auto login_config = openrs::manager::ConfigManager::get()["login"];
  CryptoPP::Integer private_exponent(
      login_config["private_exponent"].get<std::string>().c_str());
  CryptoPP::Integer modulus(login_config["modulus"].get<std::string>().c_str());
  rsa_function.Initialize(modulus, private_exponent);
  const auto decrypted_packet_int =
      rsa_function.ApplyFunction(CryptoPP::Integer(
          packet.data.data() + packet.data.position(), kRsaBlockSize));
  packet.data.seek(std::ios_base::cur, kRsaBlockSize);
  openrs::common::io::Buffer<> decrypted_packet;
  decrypted_packet.resize(decrypted_packet_int.MinEncodedSize());
  decrypted_packet_int.Encode(decrypted_packet.data(), decrypted_packet.size());

  uint8_t* rsa_block_header_ptr = nullptr;
  if (!decrypted_packet.GetData(&rsa_block_header_ptr) ||
      *rsa_block_header_ptr != 10) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  std::vector<uint32_t> xtea_keys;
  xtea_keys.reserve(4 * sizeof(uint32_t));
  for (int i = 0; i < 4; ++i) {
    uint32_t* xtea_key = nullptr;
    if (!decrypted_packet.GetData(&xtea_key)) {
      session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
      return;
    }
    xtea_keys.emplace_back(::be32toh(*xtea_key));
  }

  uint64_t* rsa_block_ptr = nullptr;
  if (!decrypted_packet.GetData(&rsa_block_ptr) || *rsa_block_ptr != 0) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  std::string password;
  if (!decrypted_packet.GetString(&password) || password.size() < 3 ||
      password.size() > 30) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorInvalidUsername);
    return;
  }

  // Decode XTEA block from the packet.
  openrs::common::io::Buffer<> decoded_packet;
  DecodeXTEA(xtea_keys, packet.data, &decoded_packet);

  uint8_t* username_header_ptr = nullptr;
  if (!decoded_packet.GetData(&username_header_ptr) ||
      *username_header_ptr != 1) {
    // TODO: Handle 'long' usernames for when *username_header_ptr != 1.
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  std::string username;
  if (!decoded_packet.GetString(&username)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorInvalidUsername);
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
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t));
  // Skip unknown 24 bytes.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t) * 24);

  std::string settings;
  if (!decoded_packet.GetString(&settings)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }

  // Skip unknown int.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint32_t));

  // Skip variable length block.
  uint8_t* block_length_ptr = nullptr;
  if (!decoded_packet.GetData(&block_length_ptr)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }
  decoded_packet.seek(std::ios_base::cur, *block_length_ptr);

  // Skip unknown int.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint32_t));
  // Skip unknown long.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint64_t));

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
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t));
  // Skip unknown byte.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t));
  // Skip unknown byte.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t));
  // Skip unknown byte.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t));
  // Skip unknown int.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint32_t));
  // Skip unknown string.
  if (!decoded_packet.GetString(&settings)) {
    session->SendOpCode(openrs::net::codec::PacketType::kErrorSession);
    return;
  }
  // Skip unknown byte.
  decoded_packet.seek(std::ios_base::cur, sizeof(uint8_t));

  // Validate cache hashes.
  const auto& cache = openrs::manager::cache::CacheManager::get().cache();
  for (uint32_t index = 0; index < cache->GetTypeCount(); ++index) {
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

  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "Player " << username << " logged in.";
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
