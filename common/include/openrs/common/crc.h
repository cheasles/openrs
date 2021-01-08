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

#include <stddef.h>
#include <stdint.h>

namespace openrs {

namespace common {

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
// constexpr uint32_t kPolynomial = 0x82f63b78;

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
constexpr uint32_t kPolynomial = 0xedb88320;

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len) {
  int k;

  crc = ~crc;
  while (len--) {
    crc ^= *buf++;
    for (k = 0; k < 8; k++) crc = crc & 1 ? (crc >> 1) ^ kPolynomial : crc >> 1;
  }
  return ~crc;
}

}  // namespace common

}  // namespace openrs
