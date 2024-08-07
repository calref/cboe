
#include "catch.hpp"
#include <sstream>
#include "location.hpp"

TEST_CASE("Parsing and stringifying locations") {
	SECTION("Stringifying") {
        location l(27, 923);
        std::ostringstream out;
        out << l;
        CHECK(out.str() == "(27,923)");
    }
	SECTION("Parsing") {
        location l;
        std::istringstream in("(27,923)");
        in >> l;
        CHECK(l.x == 27);
        CHECK(l.y == 923);
    }
}