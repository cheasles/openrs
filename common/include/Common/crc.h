#pragma once

#include <stddef.h>
#include <stdint.h>

namespace openrs
{

namespace common
{

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
// constexpr uint32_t kPolynomial = 0x82f63b78;

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
constexpr uint32_t kPolynomial = 0xedb88320;

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len)
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ kPolynomial : crc >> 1;
    }
    return ~crc;
}

}  // namespace common

}  // namespace openrs
