//
// Created by Davide Nicoli on 30/05/22.
//

#ifndef GENETIC_TSP_TSP_GA1_HPP
#define GENETIC_TSP_TSP_GA1_HPP

#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <xtensor/xnorm.hpp>

#include "utils.hpp"

template <typename Coordinates, class RNG> class TSP_GA_mut {
  using city_index = unsigned short;

public:
  using Individual = std::vector<city_index>;
  using Population = std::vector<Individual>;
  using FitnessMeasure = double;
  using Evaluations = std::vector<FitnessMeasure>;

  [[maybe_unused]] explicit TSP_GA_mut(
      std::vector<Coordinates> city_coordinates)
      : m_n_cities(city_coordinates.size()),
        m_city_coordinates(std::move(city_coordinates)),
        m_cut_distribution(0, m_n_cities - 2) {}

  Population generate(size_t N, RNG &rng) {
    Individual base(m_n_cities - 1);
    std::iota(base.begin(), base.end(), 1);
    Population population(N, base);
    //    for (auto &i : population) {
    //      std::shuffle(i.begin(), i.end(), rng);
    //    }
    std::for_each(population.begin(), population.end(), [&](Individual &i) {
      std::shuffle(i.begin(), i.end(), rng);
    });
    return population;
  }

  FitnessMeasure evaluate(const Individual &individual) {
    // The first city is fixed
    FitnessMeasure total_distance{xt::norm_l1(
        m_city_coordinates[*individual.cbegin()] - m_city_coordinates[0])()};
    for (auto i = individual.cbegin() + 1; i < individual.cend(); i++) {
      total_distance +=
          xt::norm_l1(m_city_coordinates[*i] - m_city_coordinates[*(i - 1)])();
    }
    return 1. / total_distance;
  }

  static Population select_parents(const Population &population,
                                   const Evaluations &evaluations, RNG &rng) {
    std::discrete_distribution<size_t> parents_distribution(
        evaluations.cbegin(), evaluations.cend());
    Population new_population(population.size());
    std::generate(new_population.begin(), new_population.end(),
                  [&]() { return population[parents_distribution(rng)]; });
    return new_population;
  }

  auto crossover(const Individual &parent_1, const Individual &parent_2,
                 RNG &rng) {
    using std::next;
    Individual child_1(parent_1), child_2(parent_2);
    const auto cut = m_cut_distribution(rng);
    swap_order_by_rank(next(child_1.begin(), int(cut)), child_1.end(),
                       next(child_2.begin(), int(cut)));
    return std::make_pair(child_1, child_2);
  }

  void mutate(Individual &individual, RNG &rng) {
    std::uniform_int_distribution<unsigned short> dice(0, 1);
    const auto roll = dice(rng);
    if (roll == 0) {
      _mutate_reflect(individual, rng);
    } else if (roll == 1) {
      _mutate_shift(individual, rng);
    }
  }

private:
  const size_t m_n_cities;
  const std::vector<Coordinates> m_city_coordinates;
  std::uniform_int_distribution<size_t> m_cut_distribution;

  void _mutate_reflect(Individual &individual, RNG &rng) {
    auto i1 = m_cut_distribution(rng);
    auto i2 = m_cut_distribution(rng);
    if (i1 > i2) {
      std::swap(i1, i2);
    }
    std::reverse(std::next(individual.begin(), int(i1)),
                 std::next(individual.begin(), int(i2)));
  }
  void _mutate_shift(Individual &individual, RNG &rng) {
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

#endif // GENETIC_TSP_TSP_GA1_HPP
