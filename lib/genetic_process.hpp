//
// Created by Davide Nicoli on 19/05/22.
//

#ifndef GENETIC_TSP_GENETIC_PROCESS_HPP
#define GENETIC_TSP_GENETIC_PROCESS_HPP

#include "config.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <indicators/dynamic_progress.hpp>
#include <indicators/progress_bar.hpp>

#include "utils.hpp"

namespace genetic {

template <class GA> class Process {
  using Individual = typename GA::Individual;

public:
  explicit Process(GA &&ga) : m_ga(std::forward<GA>(ga)) {}

  template <typename PopulationIt, class RNG>
  inline constexpr void generate(PopulationIt first_individual, size_t N,
                                 RNG &rng) {
    return m_ga.generate(first_individual, N, rng);
  }

  template <typename PopulationIt, typename EvaluationsIt>
  inline constexpr void evaluate(PopulationIt first_individual, size_t N,
                                 EvaluationsIt first_evaluation) {
    std::transform(first_individual, snext(first_individual, N),
                   first_evaluation,
                   [&](const auto &i) { return m_ga.evaluate(i); });
  }

  template <typename PopulationIt, typename ParentIt, typename EvaluationsIt,
            class RNG>
  inline constexpr void
  select_parents(PopulationIt first_individual, size_t N, ParentIt first_parent,
                 EvaluationsIt first_evaluation, RNG &rng) {
    m_ga.select_parents(first_individual, N, first_parent, first_evaluation,
                        rng);
    std::shuffle(first_parent, snext(first_parent, N), rng);
  }

  template <typename PopulationInIt, typename PopulationOutIt, class RNG>
  inline constexpr void crossover(PopulationInIt first_parent, size_t N,
                                  PopulationOutIt first_child, RNG &rng) {
    for (size_t i = 0; i < N; i += 2) {
      const auto [eldest, youngest] = m_ga.crossover(
          *snext(first_parent, i), *snext(first_parent, i + 1), rng);
      *snext(first_child, i) = std::move(eldest);
      *snext(first_child, i + 1) = std::move(youngest);
    }
  }

  template <typename PopulationIt, class RNG>
  inline constexpr void mutate(PopulationIt first_individual, size_t N,
                               double mutation_probability, RNG &rng) {
    std::for_each(first_individual, snext(first_individual, N),
                  [&](auto &individual) {
                    if (m_mutprob(rng) < mutation_probability)
                      m_ga.mutate(individual, rng);
                  });
  }

  template <typename PopulationIt, typename EvaluationsIt, class RNG>
  [[maybe_unused]] void run(PopulationIt first_individual,
                            size_t population_size,
                            EvaluationsIt first_evaluation, size_t n_iterations,
                            double mutation_probability, RNG &rng) {
    generate(first_individual, population_size, rng);
    evaluate(first_individual, population_size, first_evaluation);

    if (n_iterations == 0)
      return;

    std::vector<Individual> parents_buffer(population_size);
    loop_from_start(first_individual, population_size, parents_buffer.begin(),
                    first_evaluation, n_iterations, mutation_probability, rng);
  }

  template <size_t POPULATION_SIZE, typename PopulationIt,
            typename EvaluationsIt, class RNG>
  [[maybe_unused]] void run(PopulationIt first_individual,
                            EvaluationsIt first_evaluation, size_t n_iterations,
                            double mutation_probability, RNG &rng) {
    static_assert(POPULATION_SIZE <= 1000);
    generate(first_individual, POPULATION_SIZE, rng);
    evaluate(first_individual, POPULATION_SIZE, first_evaluation);

    if (n_iterations == 0)
      return;

    std::array<Individual, POPULATION_SIZE> parents_buffer{};
    loop_from_start(first_individual, POPULATION_SIZE, parents_buffer.begin(),
                    first_evaluation, n_iterations, mutation_probability, rng);
  }

  template <typename PopulationIt, typename EvaluationsIt, class RNG>
  void mpi_run(PopulationIt first_individual, size_t population_size,
               EvaluationsIt first_evaluation, size_t iterations_per_block,
               size_t n_blocks, double mutation_probability, RNG &rng) {
    using namespace indicators;

    generate(first_individual, population_size, rng);
    evaluate(first_individual, population_size, first_evaluation);

    if (n_blocks == 0)
      return;

    // MPI initialization
    int mpi_id = 0;
    int n_procs = 1;
#ifdef USE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
#endif
    if (population_size % size_t(n_procs) != 0ULL) {
      throw std::runtime_error(
          "Population size should be a multiple of the number of processes.\n"
          "population_size: " +
          std::to_string(population_size) +
          "\tn_procs: " + std::to_string(n_procs));
    }
#ifdef USE_MPI
    const auto individual_per_process = signed(population_size) / n_procs;
    std::vector<size_t> ranks(population_size);
#endif
    std::vector<Individual> population_buffer(population_size);

    select_parents(first_individual, population_size, population_buffer.begin(),
                   first_evaluation, rng);

    ProgressBar pbar{option::MaxProgress{n_blocks},
                     option::ShowElapsedTime{true},
                     option::ShowRemainingTime{true}, option::BarWidth{80}};

    for (auto i = 0U; i < n_blocks; i++) {
      loop_from_parents(first_individual, population_size,
                        population_buffer.begin(), first_evaluation,
                        iterations_per_block, mutation_probability, rng);
#ifdef USE_MPI
      combine_best_individuals(first_individual, population_size,
                               population_buffer.data(), first_evaluation,
                               ranks.begin(), individual_per_process,
                               (i < n_blocks - 1));
      if (i < n_blocks - 1) {
        std::shuffle(population_buffer.begin(), population_buffer.end(), rng);
      }
#endif
      if (mpi_id == 0) {
        pbar.tick();
        const auto best_fitness = std::max_element(
            first_evaluation, snext(first_evaluation, population_size));
        pbar.set_option(option::PostfixText{"best fitness: " +
                                            std::to_string(*best_fitness)});
      }
    }

    if (mpi_id == 0) {
      std::copy(population_buffer.cbegin(), population_buffer.cend(),
                first_individual);
    }
  }

private:
  GA m_ga;
  std::uniform_real_distribution<double> m_mutprob{};

#ifdef USE_MPI
  template <typename PopulationIt, typename BufferIt, typename EvaluationsIt,
            typename RanksIt>
  inline void
  combine_best_individuals(PopulationIt first_individual,
                           size_t population_size, BufferIt first_buffer,
                           EvaluationsIt first_evaluation, RanksIt first_rank,
                           int individual_per_process, bool all) {
    rank_n(first_evaluation, population_size, first_rank);
    order_by_n(first_individual, population_size, first_rank);
    if (all)
      MPI_Allgather(&*first_individual, individual_per_process,
                    GA::individual_mpi(), first_buffer, individual_per_process,
                    GA::individual_mpi(), MPI_COMM_WORLD);
    else
      MPI_Gather(&*first_individual, individual_per_process,
                 GA::individual_mpi(), first_buffer, individual_per_process,
                 GA::individual_mpi(), 0, MPI_COMM_WORLD);
  }
#endif

  template <typename PopulationIt, typename ParentIt, typename EvaluationsIt,
            class RNG>
  inline void cross_mut_eval(PopulationIt first_individual,
                             size_t population_size, ParentIt first_parent,
                             EvaluationsIt first_evaluation,
                             double mutation_probability, RNG &rng) {
    crossover(first_parent, population_size, first_individual, rng);
    mutate(first_individual, population_size, mutation_probability, rng);
    evaluate(first_individual, population_size, first_evaluation);
  }

  template <typename PopulationIt, typename ParentIt, typename EvaluationsIt,
            class RNG>
  void loop_from_parents(PopulationIt first_individual, size_t population_size,
                         ParentIt first_parent, EvaluationsIt first_evaluation,
                         size_t n_iterations, double mutation_probability,
                         RNG &rng) {
    using namespace indicators;
    if (n_iterations == 0)
      return;
    cross_mut_eval(first_individual, population_size, first_parent,
                   first_evaluation, mutation_probability, rng);

    for (size_t i = 1; i < n_iterations; i++) {
      select_parents(first_individual, population_size, first_parent,
                     first_evaluation, rng);
      cross_mut_eval(first_individual, population_size, first_parent,
                     first_evaluation, mutation_probability, rng);
    }
  }

  template <typename PopulationIt, typename BufferIt, typename EvaluationsIt,
            class RNG>
  void loop_from_start(PopulationIt first_individual, size_t population_size,
                       BufferIt first_buffer, EvaluationsIt first_evaluation,
                       size_t n_iterations, double mutation_probability,
                       RNG &rng) {
    if (n_iterations == 0)
      return;
    select_parents(first_individual, population_size, first_buffer,
                   first_evaluation, rng);
    loop_from_parents(first_individual, population_size, first_buffer,
                      first_evaluation, n_iterations, mutation_probability,
                      rng);
  }
};
} // namespace genetic

#endif // GENETIC_TSP_GENETIC_PROCESS_HPP
