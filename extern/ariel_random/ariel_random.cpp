/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <cmath>
#include <fstream>
#include <iostream>
#include <numbers>
#include <stdexcept>

#include "ariel_random.hpp"

void ARandom ::SaveSeed() const {
  std::ofstream WriteSeed;
  WriteSeed.open("seed.out");
  if (WriteSeed.is_open()) {
    WriteSeed << m_l1 << " " << m_l2 << " " << m_l3 << " " << m_l4 << std::endl;
  } else {
    try {
      WriteSeed.close();
    } catch (...) {
    }
    throw std::runtime_error("PROBLEM: Unable to open random.out");
  }
  WriteSeed.close();
}

double ARandom ::Gauss(double mean, double sigma) {
  double s = Rannyu();
  double t = Rannyu();
  double x = sqrt(-2 * log(1 - s)) * cos(2 * std::numbers::pi * t);
  return mean + x * sigma;
}

double ARandom ::Rannyu(double min, double max) {
  return min + (max - min) * Rannyu();
}

double ARandom ::Rannyu() {
  const double twom12 = 0.000244140625;
  int i1, i2, i3, i4;

  i1 = m_l1 * m_m4 + m_l2 * m_m3 + m_l3 * m_m2 + m_l4 * m_m1 + m_n1;
  i2 = m_l2 * m_m4 + m_l3 * m_m3 + m_l4 * m_m2 + m_n2;
  i3 = m_l3 * m_m4 + m_l4 * m_m3 + m_n3;
  i4 = m_l4 * m_m4 + m_n4;
  m_l4 = i4 % 4096;
  i3 = i3 + i4 / 4096;
  m_l3 = i3 % 4096;
  i2 = i2 + i3 / 4096;
  m_l2 = i2 % 4096;
  m_l1 = (i1 + i2 / 4096) % 4096;

  return twom12 * (m_l1 + twom12 * (m_l2 + twom12 * (m_l3 + twom12 * (m_l4))));
}

void ARandom ::SetRandom(const int *s, int p1, int p2) {
  m_m1 = 502;
  m_m2 = 1521;
  m_m3 = 4071;
  m_m4 = 2107;
  m_l1 = s[0];
  m_l2 = s[1];
  m_l3 = s[2];
  m_l4 = s[3];
  m_n1 = 0;
  m_n2 = 0;
  m_n3 = p1;
  m_n4 = p2;
}
ARandom::ARandom(const std::string_view &seeds_source,
                 const std::string_view &primes_source, size_t primes_line)
    : m_m1(502), m_m2(1521), m_m3(4071), m_m4(2107), m_l1(0), m_l2(0), m_l3(0),
      m_l4(1), m_n1(0), m_n2(0), m_n3(2892), m_n4(2587) {
  std::ifstream primes(primes_source);
  if (primes.is_open()) {
    for (size_t i = 0; i < primes_line && !primes.eof(); i++)
      primes >> m_n3 >> m_n4;
    primes.close();
  } else {
    try {
      primes.close();
    } catch (...) {
    }
    throw std::runtime_error("Could not open " + std::string(primes_source));
  }

  std::ifstream input(seeds_source);
  std::string property;
  if (input.is_open()) {
    while (!input.eof()) {
      input >> property;
      if (property == "RANDOMSEED") {
        input >> m_l1 >> m_l2 >> m_l3 >> m_l4;
      }
    }
    input.close();
  } else {
    try {
      input.close();
    } catch (...) {
    }
    throw std::runtime_error("Could not open " + std::string(seeds_source));
  }
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
