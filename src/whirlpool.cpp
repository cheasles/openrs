/*
 * whirlpool.cxx:
 *
 * Copyright (c) Masashi Fujita
 */
#include <assert.h>
#include <stdexcept>
#include "whirlpool.hpp"

/** Non-zero means using full-size (x8 in size) table for computation.  */
#define WHIRLPOOL_USE_FULL_TABLE        0

#include "whirlpool.inc"

static const size_t MAX_ROUND = 10;

namespace Whirlpool {

    Generator &Generator::update (unsigned char value) {
        if (finalized_) {
            throw std::runtime_error { "Whirlpool::Generator::update: Already finalized." } ;
        }
        if (remain_ <= 0) {
            flush ();
        }
        uint8_t *   q = &buffer_ [sizeof (buffer_) - remain_];
        *q++ = value;
        --remain_;
        add_bit_count (8);
        return *this;
    }

    Generator &Generator::update (const void *data, size_t size) {
        if (finalized_) {
            throw std::runtime_error { "Whirlpool::Generator::update: Already finalized." } ;
        }
        auto p     = static_cast<const uint8_t *> (data);
        auto p_end = p + size;

        uint8_t *   q = &buffer_ [sizeof (buffer_) - remain_];

        while (p < p_end) {
            if (remain_ <= 0) {
                flush ();
                q = &buffer_ [0];
            }
            *q++ = *p++;
            --remain_;
        }
        add_bit_count (8 * size);
        return *this;
    }

#if defined (WHIRLPOOL_USE_FULL_TABLE) && (WHIRLPOOL_USE_FULL_TABLE != 0)
    static inline uint64_t      CIR (size_t n, uint64_t value) {
        return CIR_ [256 * n + (static_cast<int> (value) & 0xFF)] ;
    }
#else

    static inline uint64_t RotateRight (uint64_t value, size_t count) {
#if defined (_MSC_VER) && (1300 <= _MSC_VER)
        return _rotr64 (value, static_cast<int> (count)) ;
#else
        return ((value >> count) | (value << (64 - count)));
#endif
    }

    static inline uint64_t CIR (size_t n, uint64_t value) {
        uint64_t result = CIR_ [static_cast<int> (value) & 0xFF];
        return RotateRight (result, 8 * n);
    }

#endif

    static inline uint64_t ToUInt64 (const void *data) {
        const unsigned char *p = static_cast<const unsigned char *> (data);
        return ( (static_cast<uint64_t> (p [0]) << 56u)
               | (static_cast<uint64_t> (p [1]) << 48u)
               | (static_cast<uint64_t> (p [2]) << 40u)
               | (static_cast<uint64_t> (p [3]) << 32u)
               | (static_cast<uint64_t> (p [4]) << 24u)
               | (static_cast<uint64_t> (p [5]) << 16u)
               | (static_cast<uint64_t> (p [6]) <<  8u)
               | (static_cast<uint64_t> (p [7]) <<  0u));
    }

    void        Generator::flush () {
        uint_fast64_t K0 = digest_ [0];
        uint_fast64_t K1 = digest_ [1];
        uint_fast64_t K2 = digest_ [2];
        uint_fast64_t K3 = digest_ [3];
        uint_fast64_t K4 = digest_ [4];
        uint_fast64_t K5 = digest_ [5];
        uint_fast64_t K6 = digest_ [6];
        uint_fast64_t K7 = digest_ [7];

        uint_fast64_t B0 = ToUInt64 (&buffer_ [8 * 0]);
        uint_fast64_t B1 = ToUInt64 (&buffer_ [8 * 1]);
        uint_fast64_t B2 = ToUInt64 (&buffer_ [8 * 2]);
        uint_fast64_t B3 = ToUInt64 (&buffer_ [8 * 3]);
        uint_fast64_t B4 = ToUInt64 (&buffer_ [8 * 4]);
        uint_fast64_t B5 = ToUInt64 (&buffer_ [8 * 5]);
        uint_fast64_t B6 = ToUInt64 (&buffer_ [8 * 6]);
        uint_fast64_t B7 = ToUInt64 (&buffer_ [8 * 7]);

        uint_fast64_t S0 = B0 ^ K0;
        uint_fast64_t S1 = B1 ^ K1;
        uint_fast64_t S2 = B2 ^ K2;
        uint_fast64_t S3 = B3 ^ K3;
        uint_fast64_t S4 = B4 ^ K4;
        uint_fast64_t S5 = B5 ^ K5;
        uint_fast64_t S6 = B6 ^ K6;
        uint_fast64_t S7 = B7 ^ K7;

        for (size_t r = 1; r <= MAX_ROUND; ++r) {
            uint_fast64_t L0 = K0;
            uint_fast64_t L1 = K1;
            uint_fast64_t L2 = K2;
            uint_fast64_t L3 = K3;
            uint_fast64_t L4 = K4;
            uint_fast64_t L5 = K5;
            uint_fast64_t L6 = K6;
            uint_fast64_t L7 = K7;
            // Compute K^r from K^{r - 1}
            K0 = (CIR (0, L0 >> 56) ^ CIR (1, L7 >> 48) ^ CIR (2, L6 >> 40) ^ CIR (3, L5 >> 32) ^
                  CIR (4, L4 >> 24) ^ CIR (5, L3 >> 16) ^ CIR (6, L2 >>  8) ^ CIR (7, L1 >>  0) ^
                  RC_[r]);
            K1 = (CIR (0, L1 >> 56) ^ CIR (1, L0 >> 48) ^ CIR (2, L7 >> 40) ^ CIR (3, L6 >> 32) ^
                  CIR (4, L5 >> 24) ^ CIR (5, L4 >> 16) ^ CIR (6, L3 >>  8) ^ CIR (7, L2 >>  0));
            K2 = (CIR (0, L2 >> 56) ^ CIR (1, L1 >> 48) ^ CIR (2, L0 >> 40) ^ CIR (3, L7 >> 32) ^
                  CIR (4, L6 >> 24) ^ CIR (5, L5 >> 16) ^ CIR (6, L4 >>  8) ^ CIR (7, L3 >>  0));
            K3 = (CIR (0, L3 >> 56) ^ CIR (1, L2 >> 48) ^ CIR (2, L1 >> 40) ^ CIR (3, L0 >> 32) ^
                  CIR (4, L7 >> 24) ^ CIR (5, L6 >> 16) ^ CIR (6, L5 >>  8) ^ CIR (7, L4 >>  0));
            K4 = (CIR (0, L4 >> 56) ^ CIR (1, L3 >> 48) ^ CIR (2, L2 >> 40) ^ CIR (3, L1 >> 32) ^
                  CIR (4, L0 >> 24) ^ CIR (5, L7 >> 16) ^ CIR (6, L6 >>  8) ^ CIR (7, L5 >>  0));
            K5 = (CIR (0, L5 >> 56) ^ CIR (1, L4 >> 48) ^ CIR (2, L3 >> 40) ^ CIR (3, L2 >> 32) ^
                  CIR (4, L1 >> 24) ^ CIR (5, L0 >> 16) ^ CIR (6, L7 >>  8) ^ CIR (7, L6 >>  0));
            K6 = (CIR (0, L6 >> 56) ^ CIR (1, L5 >> 48) ^ CIR (2, L4 >> 40) ^ CIR (3, L3 >> 32) ^
                  CIR (4, L2 >> 24) ^ CIR (5, L1 >> 16) ^ CIR (6, L0 >>  8) ^ CIR (7, L7 >>  0));
            K7 = (CIR (0, L7 >> 56) ^ CIR (1, L6 >> 48) ^ CIR (2, L5 >> 40) ^ CIR (3, L4 >> 32) ^
                  CIR (4, L3 >> 24) ^ CIR (5, L2 >> 16) ^ CIR (6, L1 >>  8) ^ CIR (7, L0 >>  0));

            L0 = S0;
            L1 = S1;
            L2 = S2;
            L3 = S3;
            L4 = S4;
            L5 = S5;
            L6 = S6;
            L7 = S7;

            S0 = (CIR (0, L0 >> 56u) ^ CIR (1, L7 >> 48u) ^ CIR (2, L6 >> 40u) ^ CIR (3, L5 >> 32u) ^
                  CIR (4, L4 >> 24u) ^ CIR (5, L3 >> 16u) ^ CIR (6, L2 >>  8u) ^ CIR (7, L1 >>  0u) ^
                  K0);
            S1 = (CIR (0, L1 >> 56u) ^ CIR (1, L0 >> 48u) ^ CIR (2, L7 >> 40u) ^ CIR (3, L6 >> 32u) ^
                  CIR (4, L5 >> 24u) ^ CIR (5, L4 >> 16u) ^ CIR (6, L3 >>  8u) ^ CIR (7, L2 >>  0u) ^
                  K1);
            S2 = (CIR (0, L2 >> 56u) ^ CIR (1, L1 >> 48u) ^ CIR (2, L0 >> 40u) ^ CIR (3, L7 >> 32u) ^
                  CIR (4, L6 >> 24u) ^ CIR (5, L5 >> 16u) ^ CIR (6, L4 >>  8u) ^ CIR (7, L3 >>  0u) ^
                  K2);
            S3 = (CIR (0, L3 >> 56u) ^ CIR (1, L2 >> 48u) ^ CIR (2, L1 >> 40u) ^ CIR (3, L0 >> 32u) ^
                  CIR (4, L7 >> 24u) ^ CIR (5, L6 >> 16u) ^ CIR (6, L5 >>  8u) ^ CIR (7, L4 >>  0u) ^
                  K3);
            S4 = (CIR (0, L4 >> 56u) ^ CIR (1, L3 >> 48u) ^ CIR (2, L2 >> 40u) ^ CIR (3, L1 >> 32u) ^
                  CIR (4, L0 >> 24u) ^ CIR (5, L7 >> 16u) ^ CIR (6, L6 >>  8u) ^ CIR (7, L5 >>  0u) ^
                  K4);
            S5 = (CIR (0, L5 >> 56u) ^ CIR (1, L4 >> 48u) ^ CIR (2, L3 >> 40u) ^ CIR (3, L2 >> 32u) ^
                  CIR (4, L1 >> 24u) ^ CIR (5, L0 >> 16u) ^ CIR (6, L7 >>  8u) ^ CIR (7, L6 >>  0u) ^
                  K5);
            S6 = (CIR (0, L6 >> 56u) ^ CIR (1, L5 >> 48u) ^ CIR (2, L4 >> 40u) ^ CIR (3, L3 >> 32u) ^
                  CIR (4, L2 >> 24u) ^ CIR (5, L1 >> 16u) ^ CIR (6, L0 >>  8u) ^ CIR (7, L7 >>  0u) ^
                  K6);
            S7 = (CIR (0, L7 >> 56u) ^ CIR (1, L6 >> 48u) ^ CIR (2, L5 >> 40u) ^ CIR (3, L4 >> 32u) ^
                  CIR (4, L3 >> 24u) ^ CIR (5, L2 >> 16u) ^ CIR (6, L1 >>  8u) ^ CIR (7, L0 >>  0u) ^
                  K7);
        }
        digest_ [0] ^= S0 ^ B0;
        digest_ [1] ^= S1 ^ B1;
        digest_ [2] ^= S2 ^ B2;
        digest_ [3] ^= S3 ^ B3;
        digest_ [4] ^= S4 ^ B4;
        digest_ [5] ^= S5 ^ B5;
        digest_ [6] ^= S6 ^ B6;
        digest_ [7] ^= S7 ^ B7;
        remain_ = sizeof (buffer_);
    }

    void        Generator::add_bit_count (uint64_t value) {
        uint_fast64_t x = bitCount_ [0];
        bitCount_ [0] += value;
        if (bitCount_ [0] < x) {
            for (size_t i = 1; i < bitCount_.size () ; ++i) {
                x = bitCount_ [i];
                bitCount_ [i] += 1;
                if (x < bitCount_ [i]) {
                    break;
                }
            }
        }
    }

    digest_t    Generator::finalize () {
        if (!finalized_) {
            if (remain_ <= 0) {
                flush ();
            }
            uint8_t *q = &buffer_ [sizeof (buffer_) - remain_];
            assert (static_cast<size_t> (buffer_.data() + sizeof (buffer_) - q) == remain_);
            *q++ = 0x80;
            --remain_;
            ::memset (q, 0, remain_);
            if (remain_ < sizeof (bitCount_)) {
                flush ();
                buffer_.fill (0) ;
            }
            embed_bit_count ();
            flush ();
            finalized_ = true;
        }
        digest_t    result ;
        for (int_fast32_t i = 0 ; i < 8 ; ++i) {
            auto v = digest_ [i] ;
            result [8 * i + 0] = static_cast<uint8_t> (v >> 56u) ;
            result [8 * i + 1] = static_cast<uint8_t> (v >> 48u) ;
            result [8 * i + 2] = static_cast<uint8_t> (v >> 40u) ;
            result [8 * i + 3] = static_cast<uint8_t> (v >> 32u) ;
            result [8 * i + 4] = static_cast<uint8_t> (v >> 24u) ;
            result [8 * i + 5] = static_cast<uint8_t> (v >> 16u) ;
            result [8 * i + 6] = static_cast<uint8_t> (v >>  8u) ;
            result [8 * i + 7] = static_cast<uint8_t> (v >>  0u) ;
        }
        return result ;
    }

    void        Generator::embed_bit_count () {
        assert (sizeof (bitCount_) <= remain_);
        unsigned char *p = &buffer_[sizeof (buffer_) - sizeof (bitCount_)];
        for (int_fast32_t i = static_cast<int_fast32_t> (bitCount_.size ()) - 1; 0 <= i; --i) {
            uint_fast64_t v = bitCount_[i];
            p [0] = static_cast<unsigned char> (v >> 56u);
            p [1] = static_cast<unsigned char> (v >> 48u);
            p [2] = static_cast<unsigned char> (v >> 40u);
            p [3] = static_cast<unsigned char> (v >> 32u);
            p [4] = static_cast<unsigned char> (v >> 24u);
            p [5] = static_cast<unsigned char> (v >> 16u);
            p [6] = static_cast<unsigned char> (v >>  8u);
            p [7] = static_cast<unsigned char> (v >>  0u);
            p += 8;
        }
    }
}       /* end of [namespace Whirlpool] */
