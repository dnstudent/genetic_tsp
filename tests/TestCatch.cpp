//
// Created by Davide Nicoli on 18/05/22.
//

#include <catch2/catch.hpp>
#include <sstream>

int test_constant() { return 42; }

float test_product(float x) { return 2 * x; }

using namespace Catch::literals;

TEST_CASE("Trying Catch2", "[catch2]") {
  SECTION("Assert call to constant") { REQUIRE(test_constant() == 42); }
  SECTION("Assert float product") { REQUIRE(test_product(3.0) == 6.0_a); }
}