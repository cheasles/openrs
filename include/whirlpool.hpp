//
// whirlpool.h: Whirlpool message digest.
//
// Copyright (c) 2016 Masashi Fujita
//
#pragma once

#include <array>
#include <cctype>
#include <cstdint>

namespace Whirlpool {

    using digest_t = std::array<uint8_t, 64>;

    /** The whirlpool message digest generator.  */
    class Generator {
    private:
        std::array<uint64_t, 8> digest_;
        bool                    finalized_;
        int32_t                 remain_;
        std::array<uint8_t, 64> buffer_;
        std::array<uint64_t, 4> bitCount_;  // 256bits counter.
    public:
        /** The default constructor.  */
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
        Generator () { clear (); }

        /**
         * The copy constructor.
         *
         * @param src    The source to copy
         */
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
        Generator (const Generator &src) : finalized_ {src.finalized_}, remain_ {src.remain_} {
            digest_.fill (0);
            buffer_.fill (0);
            bitCount_.fill (0);
        }

        /**
         * Assigns SRC's state to self.
         *
         * @param src    The source to assign
         *
         * @return *this
         */
        Generator &assign (const Generator &src) { return this->operator= (src); }

        Generator &operator= (const Generator &src) {
            if (this != &src) {
                finalized_ = src.finalized_;
                remain_    = src.remain_;
                digest_    = src.digest_;
                buffer_    = src.buffer_;
                bitCount_  = src.bitCount_;
            }
            return *this;
        }

        /**
         * Clears internal state.
         *
         * @return *this
         */
        Generator &clear () {
            finalized_ = false;
            remain_    = sizeof (buffer_);
            digest_.fill (0);
            bitCount_.fill (0);
            return *this;
        }

        /**
         * Updates state with VALUE (1 byte).
         *
         * @param value Value to examine
         *
         * @return *this
         */
        Generator &update (unsigned char value);

        /**
         * Updates state with the sequence of DATA [0..SIZE - 1].
         *
         * @param data Start of the input
         * @param size # of bytes for input
         *
         * @return *this
         */
        Generator &update (const void *data, size_t size);

        /**
         * Finalizes internal state and computes digest.
         *
         * @return Computed digest
         */
        digest_t finalize ();

    private:
        void flush ();

        /**
         * Increments bitCount_ by VALUE.
         *
         * @param value  Value for increment
         */
        void add_bit_count (uint64_t value);
        void embed_bit_count ();
    };

    /**
     * Convenience function for computing digest in 1-pass.
     *
     * @param data Data to examine
     * @param size # of bytes for input
     *
     * @return Computed digest
     */
    inline digest_t ComputeDigest (const void *data, size_t size) { return Generator ().update (data, size).finalize (); }
}  // namespace Whirlpool
