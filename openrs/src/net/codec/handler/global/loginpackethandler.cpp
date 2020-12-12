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

void HandleLoginWorld(openrs::net::codec::Packet& packet,
                      openrs::net::Client* client) {
  packet.data.seek(std::ios_base::cur, sizeof(uint8_t));
  uint16_t* rsa_block_size_ptr = nullptr;
  if (!packet.data.GetData(&rsa_block_size_ptr)) {
    return;
  }
  const uint16_t kRsaBlockSize = ::be16toh(*rsa_block_size_ptr);
  if (kRsaBlockSize > packet.data.remaining()) {
    openrs::net::codec::Packet error_packet;
    error_packet.type = openrs::net::codec::PacketType::kErrorRSA;
    client->Send(error_packet);
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
    openrs::net::codec::Packet error_packet;
    error_packet.type = openrs::net::codec::PacketType::kErrorRSA;
    client->Send(error_packet);
    return;
  }

  std::vector<uint8_t> isaac_keys;
  isaac_keys.reserve(4 * sizeof(uint32_t));
  if (decrypted_packet.remaining() < 4 * sizeof(uint32_t)) {
    openrs::net::codec::Packet error_packet;
    error_packet.type = openrs::net::codec::PacketType::kErrorRSA;
    client->Send(error_packet);
    return;
  }
  isaac_keys.insert(isaac_keys.cbegin(),
                    decrypted_packet.data() + decrypted_packet.position(),
                    decrypted_packet.data() + decrypted_packet.position() +
                        4 * sizeof(uint32_t));
  decrypted_packet.seek(std::ios_base::cur, 4 * sizeof(uint32_t));

  uint64_t* rsa_block_ptr = nullptr;
  if (!decrypted_packet.GetData(&rsa_block_ptr) || *rsa_block_ptr != 0) {
    openrs::net::codec::Packet error_packet;
    error_packet.type = openrs::net::codec::PacketType::kErrorRSA;
    client->Send(error_packet);
    return;
  }

  std::string password;
  if (!decrypted_packet.GetString(&password) || password.size() < 3 ||
      password.size() > 30) {
    openrs::net::codec::Packet error_packet;
    error_packet.type = openrs::net::codec::PacketType::kErrorInvalidUsername;
    client->Send(error_packet);
    return;
  }

  // Decode XTEA block from the packet.
  openrs::common::io::Buffer<> decoded_packet;
  CryptoPP::AutoSeededRandomPool prng;
  std::vector<uint8_t> iv;
  iv.resize(CryptoPP::XTEA::BLOCKSIZE);
  prng.GenerateBlock(iv.data(), iv.size());
  CryptoPP::CBC_Mode<CryptoPP::XTEA>::Decryption xtea;
  xtea.SetKeyWithIV(isaac_keys.data(), isaac_keys.size(), iv.data());
  try {
    // Ignore excess bytes.
    const auto kDecodedPacketSize =
        packet.data.remaining() -
        (packet.data.remaining() % CryptoPP::XTEA::BLOCKSIZE);
    CryptoPP::StringSource encrypted_source(
        packet.data.data() + packet.data.position(), kDecodedPacketSize, true,
        new CryptoPP::StreamTransformationFilter(
            xtea, new CryptoPP::VectorSink(decoded_packet)));
  } catch (const CryptoPP::InvalidCiphertext& ex) {
    openrs::common::Log(openrs::common::Log::LogLevel::kWarning)
        << "Invalid cipher text from client "
        << std::to_string(client->socket().getSocketId()) << ": " << ex.what();
    openrs::net::codec::Packet error_packet;
    error_packet.type = openrs::net::codec::PacketType::kErrorRSA;
    client->Send(error_packet);
    return;
  }
}

void openrs::net::codec::handler::global::LoginPacketHandler::Handle(
    openrs::net::codec::Packet& packet, openrs::net::Client* client) {
  client->ResetDecoder();

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
      client->set_client_build(kClientBuild);
      break;
    default:
      // Client version is not supported.
      Packet error_packet;
      error_packet.type = PacketType::kClientOutdated;
      client->ResetEncoder();
      client->Send(error_packet);
      return;
  }

  if (PacketType::kLoginWorld == packet.type) {
    HandleLoginWorld(packet, client);
  } else if (PacketType::kLoginLobby == packet.type) {
  }
}
