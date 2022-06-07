//
// Created by Davide Nicoli on 30/05/22.
//

#ifndef GENETIC_TSP_TSP_GA_STATIC_HPP
#define GENETIC_TSP_TSP_GA_STATIC_HPP

#include "config.hpp"

#include <array>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xnorm.hpp>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "utils.hpp"

template <typename Coordinates, size_t N_CITIES> class TSP {
  typedef unsigned short city_index;
  // Preventing stack overflow
  static_assert(N_CITIES <= 1000);

public:
  typedef std::array<city_index, N_CITIES - 1> Individual;
  typedef double FitnessMeasure;

  template <class Container>
  explicit TSP(const Container &city_coordinates)
      : m_city_coordinates(city_coordinates),
        m_cut_distribution(0, N_CITIES - 2) {}
  TSP(TSP &&) noexcept = default;

  template <typename PopulationIt, class RNG>
  void generate(PopulationIt first_individual, size_t N, RNG &rng) {
    Individual base;
    std::iota(base.begin(), base.end(), 1);
    std::for_each(first_individual, snext(first_individual, N),
                  [&](Individual &i) {
                    std::copy(base.cbegin(), base.cend(), i.begin());
                    std::shuffle(i.begin(), i.end(), rng);
                  });
  }

  FitnessMeasure evaluate(const Individual &individual) {
    // The first city is fixed
    FitnessMeasure total_distance{xt::norm_l1(
        m_city_coordinates[*individual.cbegin()] - m_city_coordinates[0])()};
    for (auto i = std::next(individual.cbegin()); i < individual.cend(); i++) {
      total_distance += xt::norm_l1(m_city_coordinates[*i] -
                                    m_city_coordinates[*std::prev(i)])(0);
    }
    return std::max(static_cast<FitnessMeasure>(0),
                    static_cast<FitnessMeasure>(1664) - total_distance);
  }

  template <typename PopulationIt, typename EvaluationsIt, typename OutPopIt,
            class RNG>
  static void select_parents(PopulationIt first_individual, size_t N,
                             OutPopIt first_new_individual,
                             EvaluationsIt first_evaluation, RNG &rng) {
    std::discrete_distribution<int64_t> parents_distribution(
        first_evaluation, snext(first_evaluation, N));
    std::generate(first_new_individual, snext(first_new_individual, N), [&]() {
      return *std::next(first_individual, parents_distribution(rng));
    });
  }

  template <class RNG>
  auto crossover(const Individual &parent_1, const Individual &parent_2,
                 RNG &rng) {
    using std::next;
    Individual child_1(parent_1), child_2(parent_2);
    const auto cut = m_cut_distribution(rng);
    swap_order_by_rank(snext(child_1.begin(), cut), child_1.end(),
                       snext(child_2.begin(), cut));
    return std::make_pair(child_1, child_2);
  }

  template <class RNG> void mutate(Individual &individual, RNG &rng) {
    const auto roll = m_mutation_distribution(rng);
    if (roll == 0) {
      _mutate_reflect(individual, rng);
    } else if (roll == 1) {
      _mutate_shift(individual, rng);
    }
  }

#ifdef USE_MPI
  static constexpr MPI_Datatype individual_mpi() {
    MPI_Datatype i_m;
    MPI_Type_contiguous(N_CITIES - 1, MPI_UNSIGNED_SHORT, &i_m);
    MPI_Type_commit(&i_m);
    return i_m;
  }
#endif

private:
  const std::array<Coordinates, N_CITIES> m_city_coordinates;
  std::uniform_int_distribution<size_t> m_cut_distribution;
  std::uniform_int_distribution<unsigned short> m_mutation_distribution{0, 1};

  template <class RNG> void _mutate_reflect(Individual &individual, RNG &rng) {
    auto i1 = m_cut_distribution(rng);
    auto i2 = m_cut_distribution(rng);
    if (i1 > i2) {
      std::swap(i1, i2);
    }
    std::reverse(std::next(individual.begin(), int(i1)),
                 std::next(individual.begin(), int(i2)));
  }
  template <class RNG> void _mutate_shift(Individual &individual, RNG &rng) {
    using std::next;
    std::vector<int> cuts(4);
    std::generate(cuts.begin(), cuts.end(),
                  [&]() { return int(m_cut_distribution(rng)); });
    std::sort(cuts.begin(), cuts.end());
    auto first = individual.begin();
    const auto length = std::min(cuts[1] - cuts[0], cuts[3] - cuts[2]);
    std::swap_ranges(next(first, cuts[0]), next(first, cuts[0] + length),
                     next(first, cuts[2]));
  }
};

#endif // GENETIC_TSP_TSP_GA_STATIC_HPP
