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

#include <string_view>

class ARandom {
private:
  int m_m1, m_m2, m_m3, m_m4, m_l1, m_l2, m_l3, m_l4, m_n1, m_n2, m_n3, m_n4;

protected:
public:
  // constructors
  // Random() = delete;
  ARandom() = default;
  ARandom(const std::string_view &, const std::string_view &, size_t = 1);
  // methods
  void SetRandom(const int *, int, int);
  void SaveSeed() const;
  double Rannyu();
  double Rannyu(double min, double max);
  double Gauss(double mean, double sigma);
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
