/**
 * This file is part of OpenRS.
 *
 * OpenRS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * OpenRS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with OpenRS. If not, see <https://www.gnu.org/licenses/>.
 */
#include "openrs/net/reactor.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <openrs/common/log.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "openrs/manager/worldmanager.h"
#include "openrs/net/io/channel.h"
#include "openrs/net/io/socket.h"
#include "openrs/net/session.h"

constexpr int openrs::net::Reactor::kDefaultPort;

openrs::net::Reactor::Reactor() : socket_(Reactor::kDefaultPort), epoll_(100) {
  auto login_channel = std::make_shared<io::LoginChannel>();
  login_channel->set_callback(
      std::bind(&Reactor::DoAccept, this, std::placeholders::_1));

  this->epoll_.AddPollEvent(login_channel, EPOLLIN,
                            this->socket_.getSocketId());
  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "Listening on port " << openrs::net::Reactor::kDefaultPort;
}

void openrs::net::Reactor::Poll() {
  std::vector<std::shared_ptr<io::Channel>> event_clients;
  if (this->epoll_.Wait(&event_clients) != 0) {
    for (auto& channel : event_clients) {
      auto callback_channel =
          std::dynamic_pointer_cast<io::CallbackChannel>(channel);
      if (callback_channel != nullptr) {
        callback_channel->callback()(callback_channel);
      }
    }
  }

  // Check for timeouts.
  const auto kCurrentTime = std::chrono::high_resolution_clock::now();

  {
    const std::lock_guard<std::mutex> lock(this->sessions_mutex_);
    std::vector<std::shared_ptr<openrs::net::Session>> timed_out;
    for (const auto& session : this->sessions_) {
      const auto kTimeDiff =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              kCurrentTime - session.second->socket().last_active())
              .count();
      if (kTimeDiff >= kDefaultTimeout) {
        session.second->set_status(SessionStatus::kDisconnected);
        timed_out.push_back(session.second);
      }
    }
    for (const auto& session : timed_out) {
      this->SessionDisconnect(session);
    }
  }
}

void openrs::net::Reactor::DoAccept(
    const std::shared_ptr<io::CallbackChannel>&) {
  auto socket = this->socket_.accept();

  // Initialize the session.
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if (0 != ::getpeername(socket.getSocketId(),
                         reinterpret_cast<struct sockaddr*>(&addr),
                         &addr_len)) {
    throw std::runtime_error(
        "Could not determine the peer name to accept a client.");
  }

  socket.set_socket_address(addr);

  {
    const std::lock_guard<std::mutex> lock(this->sessions_mutex_);
    if (this->sessions_.find(socket.getSocketId()) != this->sessions_.cend()) {
      openrs::common::Log(openrs::common::Log::LogLevel::kWarning)
          << "Accepted client with the same ID as an existing session, id "
          << socket.getSocketId();
      return;
    }
  }

  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "Accepted client from " << inet_ntoa(addr.sin_addr) << ", id "
      << socket.getSocketId();

  auto session = std::make_shared<Session>();
  auto session_channel = std::make_shared<io::ClientChannel>();
  session_channel->set_callback(
      std::bind(&Reactor::DoReadWrite, this, std::placeholders::_1, session));
  if (!this->epoll_.AddPollEvent(session_channel, EPOLLIN,
                                 socket.getSocketId())) {
    openrs::common::Log(openrs::common::Log::LogLevel::kError)
        << "Failed to add a client.";
    return;
  }

  {
    const std::lock_guard<std::mutex> lock(this->sessions_mutex_);
    this->sessions_[socket.getSocketId()] = session;
  }
  session->set_socket(socket);
}

void openrs::net::Reactor::DoReadWrite(
    const std::shared_ptr<io::CallbackChannel>& channel,
    std::shared_ptr<Session>& session) {
  if ((channel->event() & EPOLLIN) != 0) {
    session->Read();
  }

  if ((channel->event() & EPOLLOUT) != 0) {
    session->Write();

    if (!session->HasOutput()) {
      if (!this->epoll_.UpdatePollEvent(EPOLLIN,
                                        session->socket().getSocketId())) {
        openrs::common::Log(openrs::common::Log::LogLevel::kError)
            << "Could not queue input for client.";
      }
    }
  }

  if (session->status() == SessionStatus::kDisconnected) {
    const std::lock_guard<std::mutex> lock(this->sessions_mutex_);
    this->SessionDisconnect(session);
  } else if (session->HasOutput()) {
    if (!this->epoll_.UpdatePollEvent(EPOLLOUT | EPOLLIN,
                                      session->socket().getSocketId())) {
      openrs::common::Log(openrs::common::Log::LogLevel::kError)
          << "Could not queue output for client.";
    }
  }
}

void openrs::net::Reactor::SessionDisconnect(
    const std::shared_ptr<Session>& session) {
  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "Client " << session->socket().getSocketId() << " disconnected.";
  openrs::common::Log(openrs::common::Log::LogLevel::kDebug)
      << "  Sent: " << session->bytes_sent();
  openrs::common::Log(openrs::common::Log::LogLevel::kDebug)
      << "  Recv: " << session->bytes_received();

  if (-1 != session->player_index()) {
    openrs::manager::WorldManager::get()->remove_player(
        session->player_world(), session->player_index());
  }
  this->epoll_.RemovePollEvent(session->socket().getSocketId());
  this->sessions_.erase(session->socket().getSocketId());
}
