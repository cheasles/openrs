#include "OpenRS/net/reactor.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "Common/log.h"

#include "OpenRS/net/client.h"
#include "OpenRS/net/io/channel.h"
#include "OpenRS/net/io/socket.h"

constexpr int openrs::net::Reactor::kDefaultPort;

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
    openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
        << "Listening on port " << openrs::net::Reactor::kDefaultPort;
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

    // Check for timeouts.
    const auto kCurrentTime = std::chrono::high_resolution_clock::now();

    {
      const std::lock_guard<std::mutex> lock(this->clients_mutex_);
      std::vector<std::shared_ptr<openrs::net::Client>> timed_out;
      for (const auto& client : this->clients_) {
        const auto kTimeDiff =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                kCurrentTime - client.second->socket().last_active()).count();
        if (kTimeDiff >= kDefaultTimeout) {
          client.second->set_status(ClientStatus::kDisconnected);
          timed_out.push_back(client.second);
        }
      }
      for (const auto& client : timed_out) {
        this->ClientDisconnect(client);
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

    {
        const std::lock_guard<std::mutex> lock(this->clients_mutex_);
        if (this->clients_.find(socket.getSocketId()) != this->clients_.cend())
        {
            openrs::common::Log(openrs::common::Log::LogLevel::kWarning)
                << "Accepted client with the same ID as an existing client, id "
                << socket.getSocketId();
            return;
        }
    }

    openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
        << "Accepted client from " << inet_ntoa(addr.sin_addr)
        << ", id " << socket.getSocketId();

    auto client = std::make_shared<Client>();
    auto client_channel = std::make_shared<io::ClientChannel>();
    client_channel->set_callback(std::bind(&Reactor::DoReadWrite, this, std::placeholders::_1, client));
    if (!this->epoll_.AddPollEvent(client_channel, EPOLLIN, socket.getSocketId()))
    {
        openrs::common::Log(openrs::common::Log::LogLevel::kError)
            << "Failed to add a client.";
        return;
    }

    {
        const std::lock_guard<std::mutex> lock(this->clients_mutex_);
        this->clients_[socket.getSocketId()] = client;
    }
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
                openrs::common::Log(openrs::common::Log::LogLevel::kError)
                    << "Could not queue input for client.";
            }
        }
    }

    if (client->status() == ClientStatus::kDisconnected)
    {
        const std::lock_guard<std::mutex> lock(this->clients_mutex_);
        this->ClientDisconnect(client);
    }
    else if (client->HasOutput())
    {
        if (!this->epoll_.UpdatePollEvent(EPOLLOUT | EPOLLIN, client->socket().getSocketId()))
        {
            openrs::common::Log(openrs::common::Log::LogLevel::kError)
                << "Could not queue output for client.";
        }
    }
}

void openrs::net::Reactor::ClientDisconnect(
    const std::shared_ptr<Client>& client)
{
    openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
        << "Client "<< client->socket().getSocketId() << " disconnected.";
    openrs::common::Log(openrs::common::Log::LogLevel::kDebug)
        << "  Sent: "<< client->bytes_sent();
    openrs::common::Log(openrs::common::Log::LogLevel::kDebug)
        << "  Recv: "<< client->bytes_received();

    this->epoll_.RemovePollEvent(client->socket().getSocketId());
    this->clients_.erase(client->socket().getSocketId());
}
