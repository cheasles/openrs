#include "openrs/net/codec/handler/global/packethandler.h"

#include <openrs/common/io/buffer.h>
#include <openrs/common/log.h>
#include <endian.h>

#include <string>

#include "openrs/manager/cache/grabmanager.h"
#include "openrs/net/codec/decoder/global/grabdecoder.h"
#include "openrs/net/codec/decoder/global/logindecoder.h"
#include "openrs/net/codec/encoder/global/grabencoder.h"
#include "openrs/net/codec/encoder/global/loginencoder.h"
#include "openrs/net/codec/handler/global/grabpackethandler.h"
#include "openrs/net/codec/handler/global/loginpackethandler.h"

void openrs::net::codec::handler::global::PacketHandler::Handle(
    openrs::net::codec::Packet& packet, openrs::net::Session* session) {
  if (PacketType::kHandshake == packet.type &&
      session->status() == SessionStatus::kConnected) {
    uint32_t* client_revision = nullptr;
    uint32_t* client_revision_sub = nullptr;

    if (!packet.data.GetData<uint32_t>(&client_revision) ||
        !packet.data.GetData<uint32_t>(&client_revision_sub)) {
      return;
    }

    const uint32_t kClientBuild = ::be32toh(*client_revision);

    // Validate the client version is supported.
    switch (kClientBuild) {
      case 718:
        session->set_client_build(kClientBuild);
        break;
      default:
        // Client version is not supported.
        session->SendOpCode(PacketType::kClientOutdated);
        return;
    }

    // Check the server token is correct.
    std::string token;
    packet.data.GetString(&token);

    // Session has been validated.
    session->set_status(SessionStatus::kDownloadingCache);

    // Make sure the next packets are handled correctly.
    session->SetDecoder(std::make_unique<decoder::global::GrabDecoder>());
    session->SetEncoder(std::make_unique<encoder::global::GrabEncoder>());
    session->SetHandler(std::make_unique<handler::global::GrabPacketHandler>());

    // Send the grab data back to the client.
    common::io::Buffer<> buffer;
    manager::cache::GrabManager::WriteKeysToBuffer(&buffer);

    Packet grab_packet;
    grab_packet.type = PacketType::kStartUp;
    grab_packet.data = buffer;
    session->Send(grab_packet);
  } else if (PacketType::kLogin == packet.type &&
             session->status() == SessionStatus::kConnected) {
    session->set_status(SessionStatus::kLoggingIn);

    // Make sure the next packets are handled correctly.
    session->SetDecoder(std::make_unique<decoder::global::LoginDecoder>());
    session->SetEncoder(std::make_unique<encoder::global::LoginEncoder>());
    session->SetHandler(
        std::make_unique<handler::global::LoginPacketHandler>());

    session->SendOpCode(PacketType::kStartUp);
  } else {
    // Reject all other clients.
    common::Log(common::Log::LogLevel::kDebug) << "Session force disconnected";
    session->set_status(SessionStatus::kDisconnected);
  }
}
