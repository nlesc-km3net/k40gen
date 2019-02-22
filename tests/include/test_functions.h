#pragma once

#include <array>

template <std::size_t N>
struct get_n {
  template <typename T>
  auto operator()(T&& t) const ->
    decltype(std::get<N>(std::forward<T>(t))) {
      return std::get<N>(std::forward<T>(t));
  }
};

std::pair<double, double> generate_l0(float l0_rate, bool use_avx2);

std::pair<double, double> coincidence_rate(std::array<float, 4> rates);
