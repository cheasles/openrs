#include "OpenRS/net/codec/handler/global/loginpackethandler.h"

#include <endian.h>

#include <string>

#include "OpenRS/manager/cache/cachemanager.h"
#include "OpenRS/manager/cache/grabmanager.h"
#include "OpenRS/manager/configmanager.h"
#include "common/log.h"

void openrs::net::codec::handler::global::LoginPacketHandler::Handle(
    openrs::net::codec::Packet& packet, openrs::net::Client* client) {
  if (PacketType::kLoginWorld == packet.type) {
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
  } else if (PacketType::kLoginLobby == packet.type) {
  }
}
