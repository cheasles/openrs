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
#include "openrs/net/io/socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

openrs::net::io::BaseSocket::BaseSocket(int socketId) : socketId(socketId) {}

openrs::net::io::BaseSocket::~BaseSocket() {
  if (socketId == kInvalidSocketId) {
    // This object has been closed or moved.
    // So we don't need to call close.
    return;
  }

  try {
    close();
  } catch (...) {
    // We should log this
    // TODO: LOGGING CODE HERE

    // If the user really want to catch close errors
    // they should call close() manually and handle
    // any generated exceptions. By using the
    // destructor they are indicating that failures is
    // an OK condition.
  }
}

void openrs::net::io::BaseSocket::close() {
  if (socketId == kInvalidSocketId) {
    throw std::logic_error(
        "close called on a bad socket object (this object was moved)");
  }

  if (::close(socketId) != 0) {
    switch (errno) {
      case EBADF:
        throw std::domain_error("close: EBADF");
      case EIO:
        throw std::runtime_error("close: EIO");
      case EINTR: {
        // TODO: Check for user interrupt flags.
        //       Beyond the scope of this project
        //       so continue normal operations.
        break;
      }
      default:
        throw std::runtime_error("close: ???");
    }
  }
  socketId = kInvalidSocketId;
}

void openrs::net::io::BaseSocket::swap(BaseSocket& other) noexcept {
  using std::swap;
  swap(socketId, other.socketId);
}

openrs::net::io::BaseSocket::BaseSocket(BaseSocket&& move) noexcept
    : socketId(kInvalidSocketId) {
  move.swap(*this);
}

openrs::net::io::BaseSocket& openrs::net::io::BaseSocket::operator=(
    BaseSocket&& move) noexcept {
  move.swap(*this);
  return *this;
}

std::size_t openrs::net::io::DataSocket::getMessageData(
    std::vector<uint8_t>* output) {
  std::size_t data_read = 0;
  const std::size_t kDataMax = output->size();

  while (data_read < kDataMax) {
    std::size_t get = ::read(getSocketId(), output->data() + data_read,
                             output->size() - data_read);
    if (-1 == get) {
      if (data_read > 0) {
        break;
      }

      return -1;
    }

    if (get == 0) {
      break;
    }

    data_read += get;
  }

  this->last_active_ = std::chrono::high_resolution_clock::now();
  return data_read;
}

void openrs::net::io::DataSocket::putMessageData(
    const std::vector<uint8_t>& data) {
  std::size_t dataWritten = 0;

  while (dataWritten < data.size()) {
    std::size_t put = write(getSocketId(), data.data() + dataWritten,
                            data.size() - dataWritten);
    if (put == static_cast<std::size_t>(-1)) {
      switch (errno) {
        case EINVAL:
        case EBADF:
        case ECONNRESET:
        case ENXIO:
        case EPIPE: {
          // Fatal error. Programming bug
          throw std::domain_error("write: critical error");
        }
        case EDQUOT:
        case EFBIG:
        case EIO:
        case ENETDOWN:
        case ENETUNREACH:
        case ENOSPC: {
          // Resource acquisition failure or device error
          throw std::runtime_error("write: resource failure");
        }
        case EINTR:
          // TODO: Check for user interrupt flags.
          //       Beyond the scope of this project
          //       so continue normal operations.
        case EAGAIN: {
          // Temporary error.
          // Simply retry the read.
          continue;
        }
        default: {
          throw std::runtime_error("write: returned -1");
        }
      }
    }
    dataWritten += put;
  }

  if (dataWritten > 0) {
    this->last_active_ = std::chrono::high_resolution_clock::now();
  }

  return;
}

void openrs::net::io::DataSocket::putMessageClose() {
  if (::shutdown(getSocketId(), SHUT_WR) != 0) {
    throw std::domain_error("shutdown: critical error");
  }
}

openrs::net::io::ConnectSocket::ConnectSocket(std::string const& host, int port)
    : DataSocket(::socket(PF_INET, SOCK_STREAM, 0)) {
  struct sockaddr_in serverAddr {};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = inet_addr(host.c_str());

  if (::connect(getSocketId(), (struct sockaddr*)&serverAddr,
                sizeof(serverAddr)) != 0) {
    this->close();
    throw std::runtime_error(std::string("connect: ") + strerror(errno));
  }
}

openrs::net::io::ServerSocket::ServerSocket(int port)
    : BaseSocket(::socket(PF_INET, SOCK_STREAM, 0)) {
  struct sockaddr_in serverAddr;
  std::memset((char*)&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (::bind(getSocketId(), (struct sockaddr*)&serverAddr,
             sizeof(serverAddr)) != 0) {
    close();
    throw std::runtime_error("bind");
  }

  if (::listen(getSocketId(), kMaxClients) != 0) {
    close();
    throw std::runtime_error("listen");
  }
}

openrs::net::io::DataSocket openrs::net::io::ServerSocket::accept() {
  if (getSocketId() == kInvalidSocketId) {
    throw std::logic_error(
        "accept called on a bad socket object (this object was moved)");
  }

  struct sockaddr_storage serverStorage;
  socklen_t addr_size = sizeof serverStorage;
  int newSocket =
      ::accept(getSocketId(), (struct sockaddr*)&serverStorage, &addr_size);
  if (newSocket == -1) {
    throw std::runtime_error(std::string("accept: ") + strerror(errno));
  }

  // Configure the socket for asynchronous operation.
  if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("fcntl");
  }

  return DataSocket(newSocket);
}
