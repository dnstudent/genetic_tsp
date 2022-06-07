//
// Created by Davide Nicoli on 23/05/22.
//

#ifndef GENETIC_TSP_LCG_UTILS_HPP
#define GENETIC_TSP_LCG_UTILS_HPP

#include <concepts>
#include <fstream>
#include <string_view>

namespace lcg {
template <std::unsigned_integral uint>
constexpr uint twop12to10(uint a, uint b, uint c, uint d) {
  return d +
         static_cast<uint>(4096) *
             (c + static_cast<uint>(4096) * (b + static_cast<uint>(4096) * a));
}

template <std::unsigned_integral uint>
uint read_primes(const std::string_view &primes_source,
                 size_t primes_line = 0) {
  std::ifstream primes((std::string(primes_source)));
  if (primes.is_open()) {
    uint a, b;
    for (size_t i = 0; i + 1 < primes_line && !primes.eof(); i++)
      primes >> a >> b;
    if (!primes.eof()) {
      primes >> a >> b;
      primes.close();
      return twop12to10(uint(0), uint(0), a, b);
    } else {
      throw std::runtime_error("The primes file was too short");
    }
  } else {
    try {
      primes.close();
    } catch (...) {
    }
    throw std::runtime_error("Could not open " + std::string(primes_source));
  }
}

template <std::unsigned_integral uint>
uint read_seed(const std::string_view &seeds_source) {
  std::ifstream input((std::string(seeds_source)));
  std::string property;
  if (input.is_open()) {
    while (!input.eof()) {
      input >> property;
      if (property == "RANDOMSEED") {
        uint a, b, c, d;
        input >> a >> b >> c >> d;
        input.close();
        return twop12to10(a, b, c, d);
      }
    }
    throw std::runtime_error("Reched the end of " + std::string(seeds_source) +
                             " without finding the correct specification");
  } else {
    try {
      input.close();
    } catch (...) {
    }
    throw std::runtime_error("Could not open " + std::string(seeds_source));
  }
}
} // namespace lcg
#endif // GENETIC_TSP_LCG_UTILS_HPP
