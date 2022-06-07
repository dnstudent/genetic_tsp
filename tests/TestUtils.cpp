//
// Created by Davide Nicoli on 25/05/22.
//

#include <catch2/catch.hpp>
#include <sstream>

#include "utils.hpp"

TEST_CASE("Testing utilities", "[utils]") {
  SECTION("argsort") {
    SECTION("Empty") {
      const std::vector<int> v{};
      std::vector<int> i{};
      argsort(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i.empty());
      argsort_n(v.cbegin(), v.size(), i.begin(), std::less<>());
      REQUIRE(i.empty());
    }
    SECTION("One element") {
      const std::vector<int> v{2};
      std::vector<int> i(1, 3);
      argsort(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0});
      argsort_n(v.cbegin(), v.size(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0});
    }
    SECTION("FullChar1") {
      const std::vector<char> v{'a', 'h', 'p', 'b'};
      std::vector<int> i(v.size(), 3);
      argsort(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0, 3, 1, 2});
      argsort_n(v.cbegin(), v.size(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0, 3, 1, 2});
    }
    SECTION("FullChar2") {
      const std::vector<char> v{'a', 'h', 'p', 'b', 'e', 'c', 'i'};
      const std::vector<int> expected{0, 3, 5, 4, 1, 6, 2};
      std::vector<int> i(v.size(), 3);
      argsort(v.cbegin(), v.cend(), i.begin(), std::greater<>());
      REQUIRE(std::equal(i.cbegin(), i.cend(), expected.crbegin()));
      argsort_n(v.cbegin(), v.size(), i.begin(), std::greater<>());
      REQUIRE(std::equal(i.cbegin(), i.cend(), expected.crbegin()));
    }
    SECTION("FullInt1") {
      const std::vector<int> v{1, 2, 3, 4, 5, 6};
      const std::vector<int> expected{0, 1, 2, 3, 4, 5};
      std::vector<int> i(6);
      argsort(v.cbegin(), v.cend(), i.begin());
      REQUIRE(std::equal(i.cbegin(), i.cend(), expected.cbegin()));
      argsort_n(v.cbegin(), v.size(), i.begin());
      REQUIRE(std::equal(i.cbegin(), i.cend(), expected.cbegin()));
    }
    SECTION("FullInt2") {
      std::vector<int> v{1, 5, 4, 3, 2, -2};
      const std::vector<int> expected{5, 0, 4, 3, 2, 1};
      std::vector<int> i(6);
      argsort(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(std::equal(i.cbegin(), i.cend(), expected.cbegin()));
      argsort_n(v.cbegin(), v.size(), i.begin());
      REQUIRE(std::equal(i.cbegin(), i.cend(), expected.cbegin()));
    }
  }

  SECTION("order_to") {
    SECTION("Empty") {
      std::vector<char> v{};
      std::vector<int> i{};
      order_to(v.begin(), v.end(), i.cbegin());
      REQUIRE(v.empty());
    }
    SECTION("One element") {
      std::vector<int> v{2};
      std::vector<int> i(1, 0);
      order_to(v.begin(), v.end(), i.cbegin());
      REQUIRE(v == std::vector<int>{2});
    }
    SECTION("Full") {
      std::vector<char> v{'a', 'g', 'e', 'b'};
      std::vector<int> i{2, 1, 3, 0};
      order_to(v.begin(), v.end(), i.cbegin());
      REQUIRE(v == std::vector<char>{'e', 'g', 'b', 'a'});
    }
  }

  SECTION("rank") {
    SECTION("Empty") {
      std::vector<int> v{};
      std::vector<int> i{};
      rank(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i.empty());
    }
    SECTION("One element") {
      std::vector<int> v{2};
      std::vector<int> i(1, 3);
      rank(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0});
    }
    SECTION("FullChar1") {
      std::vector<char> v{'a', 'h', 'p', 'b'};
      std::vector<int> i(v.size(), 3);
      rank(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0, 2, 3, 1});
    }
    SECTION("FullChar2") {
      std::vector<char> v{'a', 'h', 'p', 'b', 'e', 'c', 'i'};
      std::vector<int> i(v.size(), 3);
      rank(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0, 4, 6, 1, 3, 2, 5});
    }
    SECTION("FullInt1") {
      std::vector<int> v{1, 2, 3, 4, 5, 6};
      std::vector<int> i(6);
      rank(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0, 1, 2, 3, 4, 5});
    }
    SECTION("FullInt2") {
      std::vector<int> v{1, 2, 4, 3, 5, 6};
      std::vector<int> i(6);
      rank(v.cbegin(), v.cend(), i.begin(), std::less<>());
      REQUIRE(i == std::vector<int>{0, 1, 3, 2, 4, 5});
    }
  }

  SECTION("swap_order_by_rank") {
    SECTION("Four elements") {
      std::vector<char> v1{'u', 'g', 'a', 'h', 'p', 'b'};
      //    std::vector<char> v1_cpy(v1.cbegin(), v1.cend());
      std::vector<float> v2{1, 6, 2, 7, 3, 8};
      swap_order_by_rank(v1.begin() + 2, v1.end(), v2.begin() + 2);
      CHECK(v1 == std::vector<char>{'u', 'g', 'a', 'h', 'b', 'p'});
      CHECK(v2 == std::vector<float>{1, 6, 2, 7, 8, 3});
    }
    SECTION("Six elements") {
      std::vector<char> v1{'z', 'u', 'g', 'a', 'h', 'p', 'b'};
      std::vector<float> v2{0, 1, 6, 2, 7, 3, 8};
      swap_order_by_rank(v1.begin() + 1, v1.end(), v2.begin() + 1);
      CHECK(v1 == std::vector<char>{'z', 'a', 'h', 'b', 'p', 'g', 'u'});
      CHECK(v2 == std::vector<float>{0, 8, 3, 1, 6, 7, 2});
    }
  }
}