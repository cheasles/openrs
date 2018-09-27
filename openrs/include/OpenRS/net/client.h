#pragma once

#include <sys/epoll.h>

#include <memory>

#include "OpenRS/net/io/buffer.h"
#include "OpenRS/net/io/socket.h"
#include "OpenRS/net/codec/decoder.h"

namespace openrs
{

namespace net
{

enum ClientStatus
{
    kDisconnected,
    kConnected,
    kLoggingIn,
    kLoggedIn
};

class Client
{
private:
    ClientStatus status_;
    io::DataSocket socket_;

    std::unique_ptr<openrs::net::codec::Decoder> decoder_;

    openrs::net::io::Buffer buffer_input_;
    openrs::net::io::Buffer buffer_output_;

    static constexpr size_t kReadSize = 256;

public:
    Client();
    ~Client();

    void Read();
    void Write();

    inline ClientStatus status() const
    {
        return this->status_;
    }

    inline void set_socket(io::DataSocket& socket)
    {
        this->socket_ = std::move(socket);
    }
};

}  // namespace net

}  // namespace openrs
