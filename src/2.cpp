#include <fstream>
#include <vector>

#include <xtensor/xfixed.hpp>
#include <xtensor/xindex_view.hpp>

#include <cxxopts.hpp>
#include <rapidcsv.h>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "ariel_random.hpp"
#include "config.hpp"
#include "genetic_algorithms/tsp_ga_static.hpp"
#include "genetic_process.hpp"
#include "utils.hpp"

#define N_CITIES 50ULL
#define POPULATION_SIZE 994ULL
#if POPULATION_SIZE <= 1'000ULL
#define CONTAINER(t, name)                                                     \
  std::array<t, POPULATION_SIZE> name {}
#else
#define CONTAINER(t, name) std::vector<t> name(POPULATION_SIZE)
#endif

namespace csv = rapidcsv;

int main(int argc, char *argv[]) {
  cxxopts::Options options("Exercise 10.2", "How to run exercise 10.2");
  using cxxopts::value;
  // clang-format off
  options.add_options()
      ("m,n_iterations", "Number of iterations per block", value<size_t>()->default_value("6000"))
      ("n,n_recomb", "Number of recombinations", value<size_t>()->default_value("20"))
      ("h,help", "Print this message");
  // clang-format on
  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  const auto N_ITER = result["m"].as<size_t>();
  const auto N_BLOCKS = result["n"].as<size_t>();

  int process_rank = 0;
#ifdef USE_MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
  if (process_rank == 0) {
    std::cout << "Using MPI\n";
  }
#else
  std::cout << "Not using MPI\n";
#endif

  using Rng = ARandom;
  Rng rng(SEEDS_PATH "seed.in", PRIMES_PATH "primes32001.in",
          size_t(process_rank));
  //  std::minstd_rand rng((unsigned(process_rank)));

  using point = xt::xtensor_fixed<double, xt::xshape<2>>;
  std::array<point, N_CITIES> coordinates;
  csv::Document capitals(TSP_PATH "American_capitals.csv");
  const auto longitudes = capitals.GetColumn<double>("longitude");
  const auto latitudes = capitals.GetColumn<double>("latitude");
  std::transform(longitudes.cbegin(), longitudes.cend(), latitudes.cbegin(),
                 coordinates.begin(), [](const auto lon, const auto lat) {
                   return point{lon, lat};
                 });

  TSP<point, N_CITIES> ga(coordinates);
  genetic::Process gp(std::move(ga));

  using Individual = typename TSP<point, N_CITIES>::Individual;
  CONTAINER(Individual, population);
  CONTAINER(double, evaluations);

  gp.mpi_run<POPULATION_SIZE>(population.begin(), evaluations.begin(), N_ITER,
                              N_BLOCKS, 0.05, rng);

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

    csv::Document solution;
    for (auto i = 0U; i < std::min(50ULL, POPULATION_SIZE); i++) {
      solution.InsertRow(i, std::vector<unsigned int>(population[i].cbegin(),
                                                      population[i].cend()));
    }
    solution.Save("p_2fit.csv");
  }
#ifdef USE_MPI
  MPI_Finalize();
#endif
  return 0;
}
