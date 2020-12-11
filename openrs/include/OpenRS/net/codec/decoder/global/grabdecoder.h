#pragma once

#include <common/io/buffer.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "OpenRS/net/codec/decoder/global/decoder.h"
#include "OpenRS/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace decoder {

namespace global {

class GrabDecoder : public Decoder {
 public:
  bool Decode(openrs::common::io::Buffer<>& buffer,
              openrs::net::codec::Packet* packet) override;
};

}  // namespace global

}  // namespace decoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
