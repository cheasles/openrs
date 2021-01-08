#pragma once

#include <openrs/common/io/buffer.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/decoder/global/decoder.h"
#include "openrs/net/codec/packet.h"

namespace openrs {
namespace net {
namespace codec {
namespace decoder {
namespace global {

class WorldDecoder : public Decoder {
 private:
  static inline constexpr frozen::map<uint8_t, PacketType, 15> code_mapping_{
      {5, PacketType::kActionButton3},
      {6, PacketType::kActionButton7},
      {14, PacketType::kActionButton1},
      {16, PacketType::kAFK},
      {21, PacketType::kPing},
      {27, PacketType::kActionButton9},
      {30, PacketType::kFinishedLoadingRegion},
      {32, PacketType::kActionButton8},
      {33, PacketType::kReceivedPacketCount},
      {39, PacketType::kUnknown},
      {55, PacketType::kActionButton4},
      {67, PacketType::kActionButton2},
      {68, PacketType::kActionButton5},
      {90, PacketType::kActionButton6},
      {98, PacketType::kScreen},
  };
  static inline constexpr frozen::map<PacketType, int8_t, 15> packet_sizes_{
      {PacketType::kActionButton3, 8},
      {PacketType::kActionButton7, 8},
      {PacketType::kActionButton1, 8},
      {PacketType::kAFK, 2},
      {PacketType::kPing, 0},
      {PacketType::kActionButton9, 8},
      {PacketType::kFinishedLoadingRegion, 0},
      {PacketType::kActionButton8, 8},
      {PacketType::kReceivedPacketCount, 4},
      {PacketType::kUnknown, 4},
      {PacketType::kActionButton4, 8},
      {PacketType::kActionButton2, 8},
      {PacketType::kActionButton5, 8},
      {PacketType::kActionButton6, 8},
      {PacketType::kScreen, 6},
  };

 public:
  bool Decode(openrs::common::io::Buffer<>& buffer,
              openrs::net::codec::Packet* packet) override;
};

}  // namespace global
}  // namespace decoder
}  // namespace codec
}  // namespace net
}  // namespace openrs
