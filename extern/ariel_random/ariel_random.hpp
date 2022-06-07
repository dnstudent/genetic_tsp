/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#ifndef __ARIEL_RANDOM__
#define __ARIEL_RANDOM__

#include <cstddef>
#include <limits>
#include <string_view>

//#if defined(__clang__)
template <typename uint>
constexpr uint b4096tob10(uint a, uint b, uint c, uint d) {
  return d + static_cast<uint>(4096U) *
                 (c + static_cast<uint>(4096U) *
                          (b + static_cast<uint>(4096U) * a));
}
/*
#elif defined(__GNUC__) || defined(__GNUG__)
template <typename uint> constexpr uint twop12to10(uint a, uint b, uint c, uint
d) { return d + 4096ULL * (c + 4096ULL * (b + 4096ULL * a));
}
#endif
*/

class ARandom {
protected:
public:
  typedef uint_fast64_t result_type;
  // constructors
  ARandom() = default;
  ARandom(const std::string_view &, const std::string_view &, size_t = 1);
  // methods
  [[maybe_unused]] void SetRandom(result_type const *, result_type,
                                  result_type);
  [[maybe_unused]] void SaveSeed() const;
  [[maybe_unused]] double Rannyu();
  result_type operator()();
  [[nodiscard]] static constexpr result_type min() { return 0ULL; }
  [[nodiscard]] static constexpr result_type max() {
    // 2^48 - 1
    return 281474976710655ULL;
  }
  [[maybe_unused]] double Rannyu(double min, double max);
  [[maybe_unused]] double Gauss(double mean, double sigma);

private:
  // moltiplicatore
  const result_type m_m1{502ULL}, m_m2{1521ULL}, m_m3{4071ULL}, m_m4{2107ULL};
  // prime due word del prime
  const result_type m_n1{0ULL}, m_n2{0ULL};
  // seed
  result_type m_l1{}, m_l2{}, m_l3{}, m_l4{};
  // seconde due word del prime
  result_type m_n3{}, m_n4{};
};

#endif

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
