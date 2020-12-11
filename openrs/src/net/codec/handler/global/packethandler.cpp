#include "OpenRS/net/codec/handler/global/packethandler.h"

#include <common/io/buffer.h>
#include <endian.h>

#include <string>

#include "OpenRS/manager/cache/grabmanager.h"
#include "OpenRS/net/codec/decoder/global/grabdecoder.h"
#include "OpenRS/net/codec/handler/global/grabpackethandler.h"
#include "OpenRS/net/codec/handler/global/loginpackethandler.h"
#include "OpenRS/net/codec/decoder/global/logindecoder.h"
#include "OpenRS/net/codec/encoder/global/grabencoder.h"
#include "OpenRS/net/codec/encoder/global/loginencoder.h"
#include "common/log.h"

void openrs::net::codec::handler::global::PacketHandler::Handle(
    openrs::net::codec::Packet& packet, openrs::net::Client* client) {
  if (PacketType::kHandshake == packet.type &&
      client->status() == ClientStatus::kConnected) {
    int* custom_client_build_ptr = nullptr;
    int* client_build_ptr = nullptr;

    if (!packet.data.GetData<int>(&custom_client_build_ptr) ||
        !packet.data.GetData<int>(&client_build_ptr)) {
      return;
    }

    uint32_t client_build = be32toh(*client_build_ptr);

    // Validate the client version is supported.
    switch (client_build) {
      case 718:
        client->set_client_build(client_build);
        break;
      default:
        // Client version is not supported.
        Packet error_packet;
        error_packet.type = PacketType::kClientOutdated;
        client->Send(error_packet);
        return;
    }

    // Check the server token is correct.
    std::string token;
    packet.data.GetString(&token);

    // Client has been validated.
    client->set_status(ClientStatus::kDownloadingCache);

    // Make sure the next packets are handled correctly.
    client->SetDecoder(std::make_unique<decoder::global::GrabDecoder>());
    client->SetEncoder(std::make_unique<encoder::global::GrabEncoder>());
    client->SetHandler(std::make_unique<handler::global::GrabPacketHandler>());

    // Send the grab data back to the client.
    common::io::Buffer<> buffer;
    manager::cache::GrabManager::WriteKeysToBuffer(&buffer);

    Packet grab_packet;
    grab_packet.type = PacketType::kStartUp;
    grab_packet.data = buffer;
    client->Send(grab_packet);
  } else if (PacketType::kLogin == packet.type &&
             client->status() == ClientStatus::kConnected) {
    client->set_status(ClientStatus::kLoggingIn);

    // Make sure the next packets are handled correctly.
    client->SetDecoder(std::make_unique<decoder::global::LoginDecoder>());
    client->SetEncoder(std::make_unique<encoder::global::LoginEncoder>());
    client->SetHandler(std::make_unique<handler::global::LoginPacketHandler>());

    Packet login_packet;
    login_packet.type = PacketType::kStartUp;
    client->Send(login_packet);
  } else {
    // Reject all other clients.
    common::Log(common::Log::LogLevel::kDebug) << "Client force disconnected";
    client->set_status(ClientStatus::kDisconnected);
  }
}
