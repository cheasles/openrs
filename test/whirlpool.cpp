
#include "helper.hpp"

#include <doctest/doctest.h>

#include <sstream>

TEST_CASE ("Test Whirlpool") {
    using namespace fmt::literals;

    auto               digest = [](const char* s) -> std::string {
        return normalize ("{}"_format (Whirlpool::ComputeDigest (s, strlen (s))));
    };

    SUBCASE ("(empty string)") {
        auto const& actual   = digest ("");
        auto const& expected = normalize ("19FA61D75522A466 9B44E39C1D2E1726 C530232130D407F8 9AFEE0964997F7A7"
                                          "3E83BE698B288FEB CF88E3E03C4F0757 EA8964E59B63D937 08B138CC42A66EB3");
        REQUIRE (actual == expected);
    }
    SUBCASE ("Single 'a'") {
        auto const& actual   = digest ("a");
        auto const& expected = normalize ("8ACA2602792AEC6F 11A67206531FB7D7 F0DFF59413145E69 73C45001D0087B42"
                                          "D11BC645413AEFF6 3A42391A39145A59 1A92200D560195E5 3B478584FDAE231A");
        REQUIRE (actual == expected);
    }
    SUBCASE ("\"abc\"") {
        auto const& actual   = digest ("abc");
        auto const& expected = normalize ("4E2448A4C6F486BB 16B6562C73B4020B F3043E3A731BCE72 1AE1B303D97E6D4C"
                                          "7181EEBDB6C57E27 7D0E34957114CBD6 C797FC9D95D8B582 D225292076D4EEF5");
        REQUIRE (actual == expected);
    }
    SUBCASE ("\"message digest\"") {
        auto const& actual   = digest ("message digest");
        auto const& expected = normalize ("378C84A4126E2DC6 E56DCC7458377AAC 838D00032230F53C E1F5700C0FFB4D3B"
                                          "8421557659EF55C1 06B4B52AC5A4AAA6 92ED920052838F33 62E86DBD37A8903E");
        REQUIRE (actual == expected);
    }
    SUBCASE ("\"abcdefghijklmnopqrstuvwxyz\"") {
        auto const& actual   = digest ("abcdefghijklmnopqrstuvwxyz");
        auto const& expected = normalize ("F1D754662636FFE9 2C82EBB9212A484A 8D38631EAD4238F5 442EE13B8054E41B"
                                          "08BF2A9251C30B6A 0B8AAE86177AB4A6 F68F673E7207865D 5D9819A3DBA4EB3B");
        REQUIRE (actual == expected);
    }
    SUBCASE ("\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\"") {
        auto const& actual   = digest ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        auto const& expected = normalize ("DC37E008CF9EE69B F11F00ED9ABA2690 1DD7C28CDEC066CC 6AF42E40F82F3A1E"
                                          "08EBA26629129D8F B7CB57211B9281A6 5517CC879D7B9621 42C65F5A7AF01467");
        REQUIRE (actual == expected);
    }
    SUBCASE ("\"abcdbcdecdefdefgefghfghighijhijk\"") {
        auto const& actual   = digest ("abcdbcdecdefdefgefghfghighijhijk");
        auto const& expected = normalize ("2A987EA40F917061 F5D6F0A0E4644F48 8A7A5A52DEEE6562 07C562F988E95C69"
                                          "16BDC8031BC5BE1B 7B947639FE050B56 939BAAA0ADFF9AE6 745B7B181C3BE3FD");
        REQUIRE (actual == expected);
    }
    SUBCASE ("\"1234567890\" * 8") {
        std::string tmp;
        tmp.reserve (80);
        for (int i = 0; i < 8; ++i) {
            tmp.append ("1234567890");
        }
        auto const& actual   = digest (tmp.c_str ());
        auto const& expected = normalize ("466EF18BABB0154D 25B9D38A6414F5C0 8784372BCCB204D6 549C4AFADB601429"
                                          "4D5BD8DF2A6C44E5 38CD047B2681A51A 2C60481E88C5A20B 2C2A80CF3A9A083B");
        REQUIRE (actual == expected);
    }
    SUBCASE ("1 million 'a'") {
        std::string tmp (1000000, 'a');
        auto const& actual   = digest (tmp.c_str ());
        auto const& expected = normalize ("0C99005BEB57EFF5 0A7CF005560DDF5D 29057FD86B20BFD6 2DECA0F1CCEA4AF5"
                                          "1FC15490EDDC47AF 32BB2B66C34FF9AD 8C6008AD677F7712 6953B226E4ED8B01");
        REQUIRE (actual == expected);
    }
}
