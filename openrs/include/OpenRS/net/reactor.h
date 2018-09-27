#pragma once

#include <sys/epoll.h>

#include <map>
#include <memory>

#include "OpenRS/net/client.h"
#include "OpenRS/net/io/socket.h"
#include "OpenRS/net/io/epoll.h"

namespace openrs
{

namespace net
{

class Reactor
{
private:

    /**
     * The default port to use to listen for connections.
     */
    static constexpr int kDefaultPort = 43594;

    openrs::net::io::ServerSocket socket_;
    openrs::net::io::EPoll<> epoll_;

    std::map<int, std::shared_ptr<openrs::net::Client>> clients_;

public:
    Reactor();

    void Poll();

    void DoAccept(const std::shared_ptr<io::CallbackChannel>&);

    void DoReadWrite(const std::shared_ptr<io::CallbackChannel>&, std::shared_ptr<Client>&);
};

}  // namespace net

}  // namespace openrs
