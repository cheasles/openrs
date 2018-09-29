#include "OpenRS/net/codec/decoder/global/handlers/packethandler.h"

#include <endian.h>

#include <string>

#include "OpenRS/net/io/buffer.h"
#include "OpenRS/manager/cache/grabmanager.h"

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

        // Send the grab data back to the client.
        io::Buffer buffer;
        manager::cache::GrabManager::WriteKeysToBuffer(&buffer);

        Packet grab_packet;
        grab_packet.type = PacketType::kGrabKeys;
        grab_packet.data = buffer;
        client->Send(grab_packet);
    }
    else if (PacketType::kGrabCache == packet.type)
    {
        uint8_t* index_id_ptr = nullptr;
        uint32_t* archive_id_ptr = nullptr;
        if (!packet.data.GetData(&index_id_ptr) ||
            !packet.data.GetData(&archive_id_ptr))
        {
            return;
        }
    }
    else if (PacketType::kLogin == packet.type &&
        client->status() == ClientStatus::kLoggingIn)
    {

    }
    else
    {
        // Reject all other clients.
        client->set_status(ClientStatus::kDisconnected);
    }
}
