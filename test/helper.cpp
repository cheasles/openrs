
#include "helper.hpp"

#include <doctest/doctest.h>

#include <iomanip>

std::string normalize (const std::string& s) {
    std::string result;
    result.reserve (s.size ());
    for (auto ch : s) {
        if (std::isxdigit (ch)) {
            result.push_back (std::toupper (ch));
        }
    }
    return result;
}

bool check (const std::string& actual, const std::string& expected) {
    return normalize (actual) == normalize (expected);
}

TEST_CASE ("Normalize string (for comparison)") {
    REQUIRE (normalize ("") == "");
    REQUIRE (normalize (" ") == "");
    REQUIRE (normalize ("   ") == "");
    REQUIRE (normalize ("   Z") == "");
    REQUIRE (normalize ("0 1 2 3 4 5 6 7 8 9 A B C D E F G") == "0123456789ABCDEF");
    REQUIRE (normalize ("01234567\n89ABcDeFG\n") == "0123456789ABCDEF");
}

TEST_CASE ("Compare with normalization") {
    REQUIRE (check ("", ""));
    REQUIRE (check ("", " "));
    REQUIRE (check ("A", "a"));
    REQUIRE_FALSE (check ("a", "B"));
}
