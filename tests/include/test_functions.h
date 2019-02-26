#pragma once

#include <array>
#include <unordered_map>
#include <tuple>

template <std::size_t N>
struct get_n {
  template <typename T>
  auto operator()(T&& t) const ->
    decltype(std::get<N>(std::forward<T>(t))) {
      return std::get<N>(std::forward<T>(t));
  }
};

std::pair<double, double> generate_l0(float l0_rate, long dt, bool use_avx2);

std::tuple<double, double, std::unordered_map<size_t, double>>
coincidence_rate(std::array<float, 4> rates);
