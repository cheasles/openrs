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
#include "openrs/net/session.h"

#include <openrs/common/log.h>
#include <sys/socket.h>

#include "openrs/net/codec/decoder/global/grabdecoder.h"
#include "openrs/net/codec/decoder/global/logindecoder.h"
#include "openrs/net/codec/encoder/global/grabencoder.h"
#include "openrs/net/codec/encoder/global/loginencoder.h"
#include "openrs/net/codec/handler/global/grabpackethandler.h"
#include "openrs/net/codec/handler/global/loginpackethandler.h"
#include "openrs/net/codec/packet.h"

openrs::net::Session::Session()
    : status_(SessionStatus::kConnected),
      socket_(openrs::net::io::BaseSocket::kInvalidSocketId),
      client_build_(0),
      player_index_(-1),
      player_world_(-1),
      bytes_received_(0),
      bytes_sent_(0) {
  this->TransitionState(this->status_);
}

openrs::net::Session::~Session() {}

void openrs::net::Session::Read() {
  std::vector<uint8_t> buffer;
  buffer.resize(Session::kReadSize);

  int amount_read = this->socket_.getMessageData(&buffer);
  if (-1 == amount_read || 0 == amount_read) {
    this->status_ = SessionStatus::kDisconnected;
    return;
  }

  this->buffer_input_.insert(this->buffer_input_.cend(), buffer.cbegin(),
                             buffer.cbegin() + amount_read);

  while (0 != this->buffer_input_.remaining()) {
    codec::Packet packet;
    if (!this->decoder_->Decode(this->buffer_input_, &packet)) {
      common::Log(common::Log::LogLevel::kWarning)
          << "Failed to decode a packet from a client.";
      return;
    }

    this->packet_handler_->Handle(packet, shared_from_this());
  }
  this->buffer_input_.clear();

  this->bytes_received_ += amount_read;
}

void openrs::net::Session::Write() {
  this->buffer_output_.ClearOldData();

  this->bytes_sent_ += this->buffer_output_.size();

  this->socket_.putMessageData(this->buffer_output_);

  this->buffer_output_.clear();
}

void openrs::net::Session::Send(const openrs::common::io::Buffer<>& buffer) {
  this->buffer_output_.insert(this->buffer_output_.cend(), buffer.cbegin(),
                              buffer.cend());
}

void openrs::net::Session::TransitionState(const SessionStatus& kState) {
  switch (kState) {
    case SessionStatus::kDownloadingCache: {
      static const auto HANDLER = std::make_shared<
          openrs::net::codec::handler::global::GrabPacketHandler>();
      static const auto DECODER =
          std::make_shared<openrs::net::codec::decoder::global::GrabDecoder>();
      static const auto ENCODER =
          std::make_shared<openrs::net::codec::encoder::global::GrabEncoder>();
      this->SetHandler(HANDLER);
      this->SetDecoder(DECODER);
      this->SetEncoder(ENCODER);
      break;
    }
    default: {
      static const auto HANDLER = std::make_shared<
          openrs::net::codec::handler::global::PacketHandler>();
      static const auto DECODER =
          std::make_shared<openrs::net::codec::decoder::global::Decoder>();
      static const auto ENCODER =
          std::make_shared<openrs::net::codec::encoder::global::Encoder>();
      this->SetHandler(HANDLER);
      this->SetDecoder(DECODER);
      this->SetEncoder(ENCODER);
      break;
    }
  }

  this->set_status(kState);
}