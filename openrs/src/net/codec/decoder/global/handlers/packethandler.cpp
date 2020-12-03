#include "OpenRS/net/codec/decoder/global/handlers/packethandler.h"

#include <endian.h>

#include <string>

#include "Common/log.h"

#include "OpenRS/net/io/buffer.h"
#include "OpenRS/manager/cache/grabmanager.h"
#include "OpenRS/net/codec/decoder/global/grabdecoder.h"
#include "OpenRS/net/codec/decoder/global/handlers/grabpackethandler.h"

void openrs::net::codec::decoder::global::handlers::PacketHandler::Handle(
    openrs::net::codec::Packet& packet,
    openrs::net::Client* client)
{
    if (PacketType::kHandshake == packet.type &&
        client->status() == ClientStatus::kConnected)
    {
        int* custom_client_build_ptr = nullptr;
        int* client_build_ptr = nullptr;

        if (!packet.data.GetData<int>(&custom_client_build_ptr) ||
            !packet.data.GetData<int>(&client_build_ptr))
        {
            return;
        }

        uint32_t client_build = be32toh(*client_build_ptr);

        // Validate the client version is supported.
        switch (client_build)
        {
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
        client->set_status(ClientStatus::kLoggingIn);

        // Make sure the next packets are handled correctly.
        client->SetDecoder(std::make_unique<GrabDecoder>());
        client->SetHandler(std::make_unique<GrabPacketHandler>());

        // Send the grab data back to the client.
        io::Buffer buffer;
        manager::cache::GrabManager::WriteKeysToBuffer(&buffer);

        Packet grab_packet;
        grab_packet.type = PacketType::kStartUp;
        grab_packet.data = buffer;
        client->Send(grab_packet);
    }
    else if (PacketType::kLogin == packet.type &&
        client->status() == ClientStatus::kLoggingIn)
    {
        common::Log(common::Log::LogLevel::kWarning)
            << "Temp failure";
    }
    else
    {
        // Reject all other clients.
        common::Log(common::Log::LogLevel::kDebug)
            << "Client force disconnected";
        client->set_status(ClientStatus::kDisconnected);
    }
}
