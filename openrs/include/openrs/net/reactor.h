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
#pragma once

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <mutex>

#include "openrs/net/io/epoll.h"
#include "openrs/net/io/socket.h"
#include "openrs/net/session.h"

namespace openrs {

namespace net {

class Reactor {
 private:
  /**
   * The default port to use to listen for connections.
   */
  static constexpr int kDefaultPort = 43594;

  /**
   * The amount of time in ms to wait before timing out a client.
   */
#ifndef NDEBUG
  static constexpr int kDefaultTimeout = 500;
#else  // _DEBUG
  static constexpr int kDefaultTimeout = 5000;
#endif  // _DEBUG

  openrs::net::io::ServerSocket socket_;
  openrs::net::io::EPoll<> epoll_;

  std::mutex sessions_mutex_;
  std::map<int, std::shared_ptr<openrs::net::Session>> sessions_;

 public:
  Reactor();

  void Poll();

  void DoAccept(const std::shared_ptr<io::CallbackChannel>&);

  void DoReadWrite(const std::shared_ptr<io::CallbackChannel>&,
                   std::shared_ptr<Session>&);

  void SessionDisconnect(const std::shared_ptr<Session>& session);
};

}  // namespace net

}  // namespace openrs
