//
// Created by Davide Nicoli on 23/05/22.
//

#ifndef GENETIC_TSP_LCG_HPP
#define GENETIC_TSP_LCG_HPP

#include <bitset>
#include <cstdint>
#include <fstream>
#include <string_view>

#include "lcg_utils.hpp"

namespace lcg {
template <typename uint, uint _a, uint _m> class LCG {
public:
  using result_type = uint;
  LCG() = delete;
  LCG(uint seed, uint c) : m_x(seed), m_c(c) {}

  constexpr uint_fast64_t operator()() {
    m_x = (_a * m_x + m_c) % _m;
    return m_x;
  }

  [[nodiscard]] constexpr static uint_fast64_t min() { return 0; }
  [[nodiscard]] constexpr static uint_fast64_t max() { return _m; }

protected:
  uint m_x{};
  uint m_c{};
};

// 2^48
class Rannyu {
public:
  typedef uint_fast64_t result_type;
  Rannyu() = delete;
  Rannyu(result_type seed, result_type c) : m_x(seed), m_c(c) {}
  Rannyu(const std::string_view &seeds_source, const std::string_view &primes_source,
         size_t primes_line = 0)
      : Rannyu(read_seed<uint_fast64_t>(seeds_source),
               read_primes<uint_fast64_t>(primes_source, primes_line)) {}

  constexpr result_type operator()() {
    m_x = (m_a * m_x + m_c) % m_m;
    return static_cast<result_type>(m_x);
  }

  [[nodiscard]] static constexpr result_type min() { return 0ULL; }
  [[nodiscard]] static constexpr result_type max() { return m_m - 1ULL; }

private:
  __uint128_t m_x;
  const __uint128_t m_c;
  const __uint128_t m_a{twop12to10<result_type>(502ULL, 1521ULL, 4071ULL, 2107ULL)};
  static constexpr __uint128_t m_m{281474976710656ULL};
};
} // namespace lcg
#endif // GENETIC_TSP_LCG_HPP
