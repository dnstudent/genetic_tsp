#include <cmath>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include <xtensor/xfixed.hpp>
#include <xtensor/xindex_view.hpp>

#include "ariel_random.hpp"
#include "config.hpp"
#include "genetic_algorithms/tsp_ga_static.hpp"
#include "genetic_process.hpp"
#include "utils.hpp"

#include <rapidcsv.h>

#define N_ITER 100ULL
#define N_BLOCKS 1000ULL
#define N_CITIES 34ULL
#define POPULATION_SIZE 1000ULL
#if POPULATION_SIZE <= 1000ULL
#define CONTAINER(t, name) std::array<t, POPULATION_SIZE> name
#else
#define CONTAINER(t, name) std::vector<t> name(POPULATION_SIZE)
#endif

namespace csv = rapidcsv;

template <typename It>
[[maybe_unused]] constexpr auto make_circle_points(double radius, It first,
                                                   size_t N) {
  using point = xt::xtensor_fixed<double, xt::xshape<2>>;
  for (size_t i = 0; i < N; i++) {
    *snext(first, i) =
        radius * point{std::cos(2 * double(i) * M_PI / double(N)),
                       std::sin(2 * double(i) * M_PI / double(N))};
  }
}

template <typename It, class RNG>
[[maybe_unused]] auto make_square_points(double side, It first, size_t N,
                                         RNG &rng) {
  using point = xt::xtensor_fixed<double, xt::xshape<2>>;
  std::uniform_real_distribution<double> m_pdist(0, side);
  std::generate_n(first, N, [&]() {
    return point{m_pdist(rng), m_pdist(rng)};
  });
}

int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);
  int process_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

  using Rng = ARandom;
  Rng rng(SEEDS_PATH "seed.in", PRIMES_PATH "primes32001.in",
          size_t(process_rank));
  //  std::minstd_rand rng((unsigned(process_rank)));

  using point = xt::xtensor_fixed<double, xt::xshape<2>>;
  //  const std::vector<point> coordinates{{0, 0}, {2, 0}, {2, 2}, {0, 2}};
  std::array<point, N_CITIES> coordinates;
  make_circle_points(1., coordinates.begin(), N_CITIES);

  TSP<point, N_CITIES> ga(coordinates);
  genetic::Process gp(std::move(ga));

  using Individual = typename TSP<point, N_CITIES>::Individual;
  CONTAINER(Individual, population){};
  CONTAINER(double, evaluations){};

  gp.mpi_run<POPULATION_SIZE>(population.begin(), evaluations.begin(), N_ITER,
                              N_BLOCKS, 0.05, rng);
  /*
  #if POPULATION_SIZE <= 1000
    gp.run<POPULATION_SIZE>(population.begin(), evaluations.begin(), N_ITER,
  0.1, rng); #else gp.run(population.begin(), POPULATION_SIZE,
  evaluations.begin(), N_ITER, 0.1, rng); #endif
   */

  if (process_rank == 0) {
    std::vector<int> ranks(POPULATION_SIZE);
    rank(evaluations.cbegin(), evaluations.cend(), ranks.begin(),
         std::greater<>());
    order_by(population.begin(), population.end(), ranks.cbegin());
    order_by(evaluations.begin(), evaluations.end(), ranks.cbegin());

    for (size_t i = 0; i < std::min(50ULL, POPULATION_SIZE); i++) {
      for (auto j : population[i]) {
        std::cout << j << ' ';
      }
      std::cout << '\t' << evaluations[i] << '\n';
    }
  }

  /*csv::Document table;
  for (auto i = 0U; i < population.size(); i++) {
    table.InsertColumn(i, std::vector<unsigned int>(population[i].cbegin(),
                                                    population[i].cend()));
  }
  table.Save("p.csv");*/
  MPI_Finalize();
  return 0;
}
