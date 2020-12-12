#pragma once

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <mutex>

#include "OpenRS/net/session.h"
#include "OpenRS/net/io/epoll.h"
#include "OpenRS/net/io/socket.h"

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
  static constexpr int kDefaultTimeout = 500;

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
