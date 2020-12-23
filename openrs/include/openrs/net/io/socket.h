#pragma once

#include <inttypes.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <chrono>
#include <string>
#include <vector>

namespace openrs {

namespace net {

namespace io {

// An RAII base class for handling sockets.
// Socket is movable but not copyable.
class BaseSocket {
 private:
  int socketId;

 public:
  static constexpr int kInvalidSocketId = -1;

 protected:
  // Designed to be a base class not used used directly.
  BaseSocket(int socketId);

 public:
  inline int getSocketId() const { return socketId; }

 public:
  ~BaseSocket();

  // Moveable but not Copyable
  BaseSocket(BaseSocket&& move) noexcept;
  BaseSocket& operator=(BaseSocket&& move) noexcept;
  void swap(BaseSocket& other) noexcept;
  BaseSocket(BaseSocket const&) = delete;
  BaseSocket& operator=(BaseSocket const&) = delete;

  // User can manually call close
  void close();
};

// A class that can read/write to a socket
class DataSocket : public BaseSocket {
 private:
  std::chrono::high_resolution_clock::time_point last_active_;
  struct sockaddr_in socket_address_;

 public:
  DataSocket(int socketId)
      : BaseSocket(socketId),
        last_active_(std::chrono::high_resolution_clock::now()) {}

  std::size_t getMessageData(std::vector<uint8_t>* output);
  void putMessageData(const std::vector<uint8_t>& data);
  void putMessageClose();

  inline const auto& last_active() const { return this->last_active_; }
  inline const auto& socket_address() const { return this->socket_address_; }

  inline void set_socket_address(const struct sockaddr_in& socket_address) {
    this->socket_address_ = socket_address;
  }
};

// A class the conects to a remote machine
// Allows read/write accesses to the remote machine
class ConnectSocket : public DataSocket {
 public:
  ConnectSocket(const std::string& host, int port);
};

// A server socket that listens on a port for a connection
class ServerSocket : public BaseSocket {
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
