//
// Created by Davide Nicoli on 25/05/22.
//

#ifndef GENETIC_TSP_UTILS_HPP
#define GENETIC_TSP_UTILS_HPP

#include <algorithm>
#include <cstdlib>
#include <istream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

template <typename It> inline constexpr auto snext(It it, size_t N) {
  using diff_t = typename std::iterator_traits<It>::difference_type;
  return std::next(it, static_cast<diff_t>(N));
}

template <typename InputIt, typename OutputIt, typename Compare>
constexpr auto argsort(InputIt first, InputIt last, OutputIt indices_first,
                       Compare compare) {
  using std::next;
  using diff_t = typename std::iterator_traits<InputIt>::difference_type;
  const auto N = std::distance(first, last);
  const auto indices_last = next(indices_first, N);
  std::iota(indices_first, indices_last, 0);
  return std::sort(indices_first, indices_last,
                   [&](const auto left, const auto right) -> bool {
                     // sort indices according to corresponding array element
                     return compare(*next(first, diff_t(left)),
                                    *next(first, diff_t(right)));
                   });
}

template <typename InputIt, typename OutputIt, typename Compare>
constexpr inline auto argsort_n(InputIt first, size_t N, OutputIt indices_first,
                                Compare compare) {
  return argsort(first, snext(first, N), indices_first, compare);
}

template <typename InputIt, typename OutputIt>
constexpr inline auto argsort(InputIt first, InputIt last,
                              OutputIt indices_first) {
  return argsort(first, last, indices_first, std::less<>());
}

template <typename InputIt, typename OutputIt>
constexpr inline auto argsort_n(InputIt first, size_t N,
                                OutputIt indices_first) {
  return argsort(first, snext(first, N), indices_first);
}

template <typename InputIt, typename OrderIt>
constexpr inline auto order_by_n(InputIt first, size_t N,
                                 OrderIt indices_first) {
  const std::vector<typename std::iterator_traits<InputIt>::value_type> elems(
      first, snext(first, N));
  for (auto i = 0UL; i < N; i++) {
    *std::next(first, signed(*snext(indices_first, i))) = elems[i];
  }
}

template <typename InputIt, typename OrderIt>
constexpr inline auto order_by(InputIt first, InputIt last,
                               OrderIt indices_first) {
  const auto N = std::distance(first, last);
  return order_by_n(first, unsigned(N), indices_first);
};

template <typename InputIt, typename OrderIt>
constexpr inline auto order_to(InputIt first, InputIt last,
                               OrderIt indices_first) {
  using std::next;
  const auto N = std::distance(first, last);
  const std::vector<typename std::iterator_traits<InputIt>::value_type> elems(
      first, last);
  for (auto i = 0; i < N; i++) {
    *next(first, i) = elems[size_t(*next(indices_first, i))];
  }
}

template <typename InputIt, typename OrderIt>
constexpr inline auto order_to_n(InputIt first, size_t N,
                                 OrderIt indices_first) {
  return order_to(first, snext(first, N), indices_first);
}

// TODO: there must be a better way
template <typename InputIt, typename RankIt, typename Compare>
constexpr auto rank(InputIt first, InputIt last, RankIt rank_first,
                    Compare compare) {
  using std::distance, std::next;
  const auto N = distance(first, last);
  using index_type = typename std::iterator_traits<RankIt>::value_type;
  std::vector<index_type> argsort_indices((size_t(N)));
  argsort(first, last, argsort_indices.begin(), compare);
  std::iota(rank_first, next(rank_first, N), 0);
  order_by(rank_first, next(rank_first, N), argsort_indices.cbegin());
}

template <typename InputIt, typename RankIt, typename Compare>
constexpr inline auto rank_n(InputIt first, size_t N, RankIt rank_first,
                             Compare compare) {
  return rank(first, snext(first, N), rank_first, compare);
}

template <typename InputIt, typename RankIt>
constexpr inline auto rank(InputIt first, InputIt last, RankIt rank_first) {
  return rank(first, last, rank_first, std::less<>());
}

template <typename InputIt, typename RankIt>
constexpr inline auto rank_n(InputIt first, size_t N, RankIt rank_first) {
  return rank(first, snext(first, N), rank_first);
}

template <typename InputIt1, typename InputIt2, typename Compare>
void swap_order_by_rank_n(InputIt1 first_1, size_t N, InputIt2 first_2,
                          Compare compare) {
  using std::next, std::distance;
  std::vector<int> rank_1(N);
  rank_n(first_1, N, rank_1.begin(), compare);
  order_by_n(first_1, N, rank_1.cbegin());
  std::vector<int> rank_2(N);
  rank_n(first_2, N, rank_2.begin(), compare);
  order_by_n(first_2, N, rank_2.cbegin());

  order_to_n(first_1, N, rank_2.cbegin());
  order_to_n(first_2, N, rank_1.cbegin());
}

// TODO: there must be a better way
template <typename InputIt1, typename InputIt2, typename Compare>
void swap_order_by_rank(InputIt1 first_1, InputIt1 last_1, InputIt2 first_2,
                        Compare compare) {
  return swap_order_by_rank_n(first_1, size_t(std::distance(first_1, last_1)),
                              first_2, compare);
}

template <typename InputIt1, typename InputIt2>
inline void swap_order_by_rank_n(InputIt1 first_1, size_t N, InputIt2 first_2) {
  return swap_order_by_rank_n(first_1, N, first_2, std::less<>());
}

template <typename InputIt1, typename InputIt2>
inline void swap_order_by_rank(InputIt1 first_1, InputIt1 last_1,
                               InputIt2 first_2) {
  swap_order_by_rank(first_1, last_1, first_2, std::less<>());
}

#endif // GENETIC_TSP_UTILS_HPP
