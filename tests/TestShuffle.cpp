//
// Created by Davide Nicoli on 20/05/22.
//

#include <catch2/catch.hpp>
#include <chrono>
#include <random>

#include "ariel_random.hpp"
#include "config.hpp"
#include "random_utils.hpp"

TEST_CASE("Shuffling normal vectors", "[random]") {
  SECTION("Shuffle length 0 vector") {
    std::vector<int> a(0, 0);
    ARandom rng(SEEDS_PATH "seed.in", PRIMES_PATH "Primes");
    for (auto i = 0; i < 10000; i++) {
      REQUIRE_NOTHROW(random_shuffle(a.begin(), a.end(), rng));
      REQUIRE(a == std::vector<int>(0, 0));
    }
  }
  SECTION("Shuffle length 1 vector") {
    std::vector<int> a(1, 0);
    ARandom rng(SEEDS_PATH "seed.in", PRIMES_PATH "Primes");
    for (auto i = 0; i < 10000; i++) {
      REQUIRE_NOTHROW(random_shuffle(a.begin(), a.end(), rng));
      REQUIRE(a == std::vector<int>(1, 0));
    }
  }
  SECTION("Shuffle length 2 vector") {
    std::vector<int> a(2, 0);
    ARandom rng(SEEDS_PATH "seed.in", PRIMES_PATH "Primes");
    for (auto i = 0; i < 10000; i++) {
      REQUIRE_NOTHROW(random_shuffle(a.begin(), a.end(), rng));
    }
  }
  SECTION("Shuffle length 10 vector") {
    std::vector<int> a(10, 0);
    ARandom rng(SEEDS_PATH "seed.in", PRIMES_PATH "Primes");
    for (auto i = 0; i < 10000; i++) {
      REQUIRE_NOTHROW(random_shuffle(a.begin(), a.end(), rng));
    }
  }
}

TEST_CASE("Check consistency", "[random]") {
  SECTION("Permutations") {
    std::vector<int> a0(100);
    std::vector<int> a(a0.size());
    std::iota(a0.begin(), a0.end(), 0);
    std::copy(a0.cbegin(), a0.cend(), a.begin());

    std::random_device rd;
    std::uniform_int_distribution<size_t> dist(0, 32000);

    ARandom rng(SEEDS_PATH "seed.in", PRIMES_PATH "primes32001.in", dist(rd));
    for (auto i = 0; i < 1000; i++) {
      random_shuffle(a.begin(), a.end(), rng);
      std::sort(a.begin(), a.end());
      REQUIRE(std::mismatch(a.cbegin(), a.cend(), a0.cbegin()) ==
              std::make_pair(a.cend(), a0.cend()));
    }
  }
}

TEST_CASE("Check rngs", "[random]") {
  /*SECTION("Testing ARandom vs linear_congruential_engine") {
    ARandom arandom(SEEDS_PATH "seed.in", PRIMES_PATH "Primes");
    lcg::Rannyu rannyu(SEEDS_PATH "seed.in", PRIMES_PATH "Primes");

    std::uniform_real_distribution<float> dist(0, 1);
    const size_t N = 100'000'000;
    std::vector<double> results1(N);
    std::vector<float> results2(N);

    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();
    std::generate(results1.begin(), results1.end(),
                  [&]() { return arandom.Rannyu(); });
    auto t1 = clock::now() - t0;

    t0 = clock::now();
    std::generate(results2.begin(), results2.end(),
                  [&]() { return dist(rannyu); });
    auto t2 = clock::now() - t0;

    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t1).count()
        << '\t'
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2).count()
        << std::endl;

    for (auto i = 0; i < 100; i++) {
      std::cout << arandom.Rannyu() << "\t" << dist(rannyu) << '\n';
    }
  }*/
}

TEST_CASE("Random utils", "[random]") {
  SECTION("Conversion") {
    REQUIRE(b4096tob10<size_t>(4096, 0, 0, 0) == 281474976710656);
  }
}