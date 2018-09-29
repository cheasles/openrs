#pragma once

#include <inttypes.h>
#include <sys/time.h>

#include <vector>

namespace openrs
{

namespace net
{

namespace io
{

// An RAII base class for handling sockets.
// Socket is movable but not copyable.
class BaseSocket
{
private:
    int     socketId;

protected:
    static constexpr int kInvalidSocketId = -1;

protected:
    // Designed to be a base class not used used directly.
    BaseSocket(int socketId);

public:
    inline int getSocketId() const
    {
        return socketId;
    }

public:
    ~BaseSocket();

    // Moveable but not Copyable
    BaseSocket(BaseSocket&& move)               noexcept;
    BaseSocket& operator=(BaseSocket&& move)    noexcept;
    void swap(BaseSocket& other)                noexcept;
    BaseSocket(BaseSocket const&)               = delete;
    BaseSocket& operator=(BaseSocket const&)    = delete;

    // User can manually call close
    void close();
};

// A class that can read/write to a socket
class DataSocket : public BaseSocket
{
private:
    struct timeval last_active_;

public:
    DataSocket(int socketId)
        : BaseSocket(socketId)
    {}

    std::size_t getMessageData(std::vector<uint8_t>* output);
    void putMessageData(const std::vector<uint8_t>& data);
    void putMessageClose();
};

// A server socket that listens on a port for a connection
class ServerSocket : public BaseSocket
{
public:
    static constexpr int kMaxClients = 2000;

public:
    ServerSocket(int port);

    // An accepts waits for a connection and returns a socket
    // object that can be used by the client for communication
    DataSocket accept();
};

}  // namespace io

}  // namespace net

}  // namespace openrs
