//
// Created by Davide Nicoli on 20/05/22.
//

#ifndef GENETIC_TSP_CONCEPTS_HPP
#define GENETIC_TSP_CONCEPTS_HPP

#include <concepts>
#include <iterator>

template <typename T>
concept is_sized = requires(T &t) {
                     { std::size(t) };
                   };

namespace genetic {
// clang-format off
template <class A, class RNG>
concept algorithm = requires (A &a) {
    typename A::Individual;
    typename A::Population;
    std::totally_ordered<typename A::FitnessMeasure>;
    typename A::Evaluations;
    } &&
    requires(A &a, RNG &rng, size_t population_size, typename A::Individual &individual, typename A::Population &population, typename A::Evaluations &evaluations) {
      { a.generate(population_size, rng) }  -> std::same_as<typename A::Population>;
      { a.evaluate(individual) }  -> std::same_as<typename A::FitnessMeasure>;
      { a.select_parents(population, evaluations, rng)}  -> std::same_as<typename A::Population>;
      { a.crossover(individual, individual, rng) }  -> std::same_as<std::pair<typename A::Individual, typename A::Individual>>;
      { a.mutate(individual, rng) } ;
    };

template <class GP, class Algo, class RNG, typename Population>
concept process =
    is_sized<Population> &&
    requires(GP gp, Algo &a, RNG &rng, size_t population_size, Population &population) {
      { gp.generate(population_size, rng) } -> std::same_as<Population>;
      { gp.evaluate(population) } ; // totally_ordered vector
      { gp.crossover(population) }  -> std::same_as<Population>;
      { gp.mutate(population) } ; // modify the input for performance reasons
    };
//  clang-format on
} // namespace genetic

#endif // GENETIC_TSP_CONCEPTS_HPP
