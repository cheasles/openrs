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

namespace cache_718 {

class Decoder : public openrs::net::codec::decoder::global::Decoder {
 private:
  static constexpr frozen::map<uint8_t, PacketType, 2> code_mapping_{
      {15, PacketType::kHandshake},
      {14, PacketType::kLogin},
  };
};

}  // namespace cache_718

}  // namespace decoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
