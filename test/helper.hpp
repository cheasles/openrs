//
// Created by Masashi Fujita on 2019-03-19.
//
#pragma once

#include <whirlpool.hpp>

#include <fmt/format.h>
#include <iostream>
#include <string>

namespace fmt {
    template <>
        struct formatter<Whirlpool::digest_t> {
            template <typename ParseContext>
                constexpr auto parse (ParseContext &ctx) { return ctx.begin (); }

            template <typename FormatContext>
                auto format (const Whirlpool::digest_t &digest, FormatContext &ctx) {
                    for (size_t i  = 0 ; i < 2 ; ++i) {
                        fmt::format_to (ctx.out()
                                            , "{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}"
                                            , digest[32 * i + 0]
                                            , digest[32 * i + 1]
                                            , digest[32 * i + 2]
                                            , digest[32 * i + 3]
                                            , digest[32 * i + 4]
                                            , digest[32 * i + 5]
                                            , digest[32 * i + 6]
                                            , digest[32 * i + 7]);
                        fmt::format_to (ctx.out()
                                            , " {:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}"
                                            , digest[32 * i +  8]
                                            , digest[32 * i +  9]
                                            , digest[32 * i + 10]
                                            , digest[32 * i + 11]
                                            , digest[32 * i + 12]
                                            , digest[32 * i + 13]
                                            , digest[32 * i + 14]
                                            , digest[32 * i + 15]);
                        fmt::format_to (ctx.out()
                                            , " {:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}"
                                            , digest[32 * i + 16]
                                            , digest[32 * i + 17]
                                            , digest[32 * i + 18]
                                            , digest[32 * i + 19]
                                            , digest[32 * i + 20]
                                            , digest[32 * i + 21]
                                            , digest[32 * i + 22]
                                            , digest[32 * i + 23]);
                        fmt::format_to (ctx.out()
                                             , " {:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}\n"
                                             , digest[32 * i + 24]
                                             , digest[32 * i + 25]
                                             , digest[32 * i + 26]
                                             , digest[32 * i + 27]
                                             , digest[32 * i + 28]
                                             , digest[32 * i + 29]
                                             , digest[32 * i + 30]
                                             , digest[32 * i + 31]);
                    }
                    return ctx.out();
                }
        };
} // namespace fmt

std::string normalize (const std::string &s);

bool check (const std::string &actual, const std::string &expected);
