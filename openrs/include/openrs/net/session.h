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

#include <openrs/common/io/buffer.h>
#include <sys/epoll.h>

#include <memory>

#include "openrs/net/codec/decoder/global/decoder.h"
#include "openrs/net/codec/encoder/global/encoder.h"
#include "openrs/net/codec/handler/global/packethandler.h"
#include "openrs/net/io/socket.h"

namespace openrs {
namespace net {
namespace codec {
namespace handler {
namespace global {

class PacketHandler;

}  // namespace global
}  // namespace handler
}  // namespace codec
}  // namespace net
}  // namespace openrs

namespace openrs {
namespace net {

enum SessionStatus {
  kDisconnected,
  kConnected,
  kDownloadingCache,
  kLoggingIn,
  kLoggedIn
};

class Session : public std::enable_shared_from_this<Session> {
 private:
  SessionStatus status_;
  io::DataSocket socket_;

  std::shared_ptr<openrs::net::codec::decoder::global::Decoder> decoder_;
  std::shared_ptr<openrs::net::codec::handler::global::PacketHandler>
      packet_handler_;
  std::shared_ptr<openrs::net::codec::encoder::global::Encoder> encoder_;

  openrs::common::io::Buffer<> buffer_input_;
  openrs::common::io::Buffer<> buffer_output_;

  std::size_t bytes_received_;
  std::size_t bytes_sent_;

  uint32_t client_build_;
  uint32_t player_index_;
  uint32_t player_world_;

  static constexpr size_t kReadSize = 1024;

 public:
  Session();
  ~Session();

  void Read();
  void Write();

  void Send(const openrs::common::io::Buffer<>& buffer);

  inline void Send(const openrs::net::codec::Packet& packet) {
    openrs::common::io::Buffer<> buffer;
    this->encoder_->Encode(packet, &buffer);
    this->Send(buffer);
  }

  inline void SendOpCode(const openrs::net::codec::PacketType& opcode) {
    openrs::net::codec::Packet error_packet;
    error_packet.type = opcode;
    this->Send(error_packet);
  }

  /**
   * Transition the state of a client connection so that subsequent packets are
   *  handled differently.
   *
   * @param kState The new state to transition to.
   */
  void TransitionState(const SessionStatus& kState);

  inline bool HasOutput() const { return this->buffer_output_.size() != 0; }

  inline void SetDecoder(
      std::unique_ptr<openrs::net::codec::decoder::global::Decoder>&& decoder) {
    this->decoder_ = std::move(decoder);
  }

  inline void SetEncoder(
      const std::shared_ptr<openrs::net::codec::encoder::global::Encoder>&
          kEncoder) {
    this->encoder_ = kEncoder;
  }

  inline void SetHandler(
      std::unique_ptr<openrs::net::codec::handler::global::PacketHandler>&&
          handler) {
    this->packet_handler_ = std::move(handler);
  }

  inline void ResetEncoder() {
    this->encoder_ =
        std::make_unique<openrs::net::codec::encoder::global::Encoder>();
  }

  inline void ResetDecoder() {
    this->decoder_ =
        std::make_unique<openrs::net::codec::decoder::global::Decoder>();
  }

  inline void ResetHandler() {
    this->packet_handler_ =
        std::make_unique<openrs::net::codec::handler::global::PacketHandler>();
  }

  inline SessionStatus status() const { return this->status_; }

  inline const io::DataSocket& socket() const { return this->socket_; }

  inline std::size_t bytes_received() const { return this->bytes_received_; }

  inline std::size_t bytes_sent() const { return this->bytes_sent_; }

  inline uint32_t client_build() const { return this->client_build_; }
  inline uint32_t player_index() const { return this->player_index_; }
  inline uint32_t player_world() const { return this->player_world_; }

  inline void set_socket(io::DataSocket& socket) {
    this->socket_ = std::move(socket);
  }

  inline void set_status(const SessionStatus& status) {
    this->status_ = status;
  }

  inline void set_client_build(const uint32_t& client_build) {
    this->client_build_ = client_build;
  }
  inline void set_player_index(const uint32_t& player_index) {
    this->player_index_ = player_index;
  }
  inline void set_player_world(const uint32_t& kPlayerWorld) {
    this->player_world_ = kPlayerWorld;
  }
};

}  // namespace net
}  // namespace openrs
