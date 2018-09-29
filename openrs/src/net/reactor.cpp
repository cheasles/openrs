#include "OpenRS/net/reactor.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "OpenRS/net/client.h"
#include "OpenRS/net/io/channel.h"
#include "OpenRS/net/io/socket.h"

openrs::net::Reactor::Reactor()
    : socket_(Reactor::kDefaultPort)
    , epoll_(100)
{
    auto login_channel = std::make_shared<io::LoginChannel>();
    login_channel->set_callback(std::bind(&Reactor::DoAccept, this, std::placeholders::_1));

    this->epoll_.AddPollEvent(
        login_channel,
        EPOLLIN,
        this->socket_.getSocketId());
    std::cout << "Listening on port " << Reactor::kDefaultPort << std::endl;
}

void openrs::net::Reactor::Poll()
{
    std::vector<std::shared_ptr<io::Channel>> event_clients;
    if (this->epoll_.Wait(&event_clients) != 0)
    {
        for (auto& channel : event_clients)
        {
            auto callback_channel = std::dynamic_pointer_cast<io::CallbackChannel>(channel);
            if (callback_channel != nullptr)
            {
                callback_channel->callback()(callback_channel);
            }
        }
    }
}

void openrs::net::Reactor::DoAccept(const std::shared_ptr<io::CallbackChannel>&)
{
    auto socket = this->socket_.accept();

    // Initialize the client.
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (0 != ::getpeername(socket.getSocketId(), reinterpret_cast<struct sockaddr*>(&addr), &addr_len))
    {
        throw std::runtime_error("Could not determine the peer name to accept a client.");
    }

    printf("Accepted client from %s\n", inet_ntoa(addr.sin_addr));

    auto client = std::make_shared<Client>();
    auto client_channel = std::make_shared<io::ClientChannel>();
    client_channel->set_callback(std::bind(&Reactor::DoReadWrite, this, std::placeholders::_1, client));
    if (!this->epoll_.AddPollEvent(client_channel, EPOLLIN, socket.getSocketId()))
    {
        std::cerr << "Failed to add a client." << std::endl;
        return;
    }

    this->clients_[socket.getSocketId()] = client;
    client->set_socket(socket);
}

void openrs::net::Reactor::DoReadWrite(
    const std::shared_ptr<io::CallbackChannel>& channel,
    std::shared_ptr<Client>& client)
{
    if ((channel->event() & EPOLLIN) != 0)
    {
        client->Read();
    }

    if ((channel->event() & EPOLLOUT) != 0)
    {
        client->Write();

        if (!client->HasOutput())
        {
            if (!this->epoll_.UpdatePollEvent(EPOLLIN, client->socket().getSocketId()))
            {
                std::cerr << "Could not queue input for client." << std::endl;
            }
        }
    }

    if (client->status() == ClientStatus::kDisconnected)
    {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        if (0 < ::getpeername(client->socket().getSocketId(), reinterpret_cast<struct sockaddr*>(&addr), &addr_len))
        {
            std::cout << "Client " << inet_ntoa(addr.sin_addr) << " disconnected." << std::endl;
        }

        std::cout << "  Sent: " << std::to_string(client->bytes_sent()) << std::endl;
        std::cout << "  Recv: " << std::to_string(client->bytes_received()) << std::endl;

        this->epoll_.RemovePollEvent(client->socket().getSocketId());
        this->clients_.erase(client->socket().getSocketId());
    }
    else if (client->HasOutput())
    {
        if (!this->epoll_.UpdatePollEvent(EPOLLOUT | EPOLLIN, client->socket().getSocketId()))
        {
            std::cerr << "Could not queue output for client." << std::endl;
        }
    }
}
