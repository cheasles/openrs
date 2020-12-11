#include "OpenRS/net/codec/handler/global/loginpackethandler.h"

#include <endian.h>

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
