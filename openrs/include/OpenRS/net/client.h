#pragma once

#include <common/io/buffer.h>
#include <sys/epoll.h>

#include <memory>

#include "OpenRS/net/codec/decoder/global/decoder.h"
#include "OpenRS/net/codec/handler/global/packethandler.h"
#include "OpenRS/net/codec/encoder/global/encoder.h"
#include "OpenRS/net/io/socket.h"

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

enum ClientStatus {
  kDisconnected,
  kConnected,
  kDownloadingCache,
  kLoggingIn,
  kLoggedIn
};

class Client {
 private:
  ClientStatus status_;
  io::DataSocket socket_;

  std::unique_ptr<openrs::net::codec::decoder::global::Decoder> decoder_;
  std::unique_ptr<openrs::net::codec::handler::global::PacketHandler>
      packet_handler_;
  std::unique_ptr<openrs::net::codec::encoder::global::Encoder> encoder_;

  openrs::common::io::Buffer<> buffer_input_;
  openrs::common::io::Buffer<> buffer_output_;

  std::size_t bytes_received_;
  std::size_t bytes_sent_;

  uint32_t client_build_;

  static constexpr size_t kReadSize = 1024;

 public:
  Client();
  ~Client();

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

  inline bool HasOutput() const { return this->buffer_output_.size() != 0; }

  inline void SetDecoder(
      std::unique_ptr<openrs::net::codec::decoder::global::Decoder>&& decoder) {
    this->decoder_ = std::move(decoder);
  }

  inline void SetEncoder(
      std::unique_ptr<openrs::net::codec::encoder::global::Encoder>&& encoder) {
    this->encoder_ = std::move(encoder);
  }

  inline void SetHandler(
      std::unique_ptr<
          openrs::net::codec::handler::global::PacketHandler>&&
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
    this->packet_handler_ = std::make_unique<
        openrs::net::codec::handler::global::PacketHandler>();
  }

  inline ClientStatus status() const { return this->status_; }

  inline const io::DataSocket& socket() const { return this->socket_; }

  inline std::size_t bytes_received() const { return this->bytes_received_; }

  inline std::size_t bytes_sent() const { return this->bytes_sent_; }

  inline uint32_t client_build() const { return this->client_build_; }

  inline void set_socket(io::DataSocket& socket) {
    this->socket_ = std::move(socket);
  }

  inline void set_status(const ClientStatus& status) { this->status_ = status; }

  inline void set_client_build(const uint32_t& client_build) {
    this->client_build_ = client_build;
  }
};

}  // namespace net

}  // namespace openrs
