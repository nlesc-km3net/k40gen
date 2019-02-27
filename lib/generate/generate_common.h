/*
 * Copyright 2018-2019 NWO-I
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <vector>
#include <numeric>
#include <array>
#include <random>
#include "storage.h"

namespace Constants {

  // 2 * pi
  const float two_pi = 2.0 * 3.14159265358979323846;

  // parameters of probability parameterisation for coincidences
  // between PMTs as a function of their opening angle.
  const float p2 =  2.4347f;
  const float p3 = -0.68884f;
  const float p4 = 1.3911f;

  const int n_dom = 115;
  const int n_mod = 18;
  const int n_pmt = 31;

  // from fit to Gaussian part of ToT distribution
  const float tot_mean = 26.936f;
  const float tot_sigma = 2.44078f;

  const int random_method = 2;
}

float cross_prob(const float ct);

struct Generators {
private:
  const std::array<int, 2> m_seeds;
  const std::array<float, 4> m_rates{7000., 700, 70., 7.};
  std::seed_seq m_seed_seq;

public:
  Generators(const int seed0, const int seed1, const std::array<float, 4> rates);

  std::mt19937_64 mt;
  const double coincidence_rate = 0.;
  std::geometric_distribution<long> coincidence;
  std::uniform_real_distribution<double> flat;
  std::array<float, 31> prob1D;
  std::array<std::array<float, 31>, 31> prob2D;
  std::array<float, 31> probND;

  const std::array<float, 4>& rates() const {
    return m_rates;
  }

  const std::array<int, 2>& seeds() const {
    return m_seeds;
  }

  float tau_l0() const {
     return 1.0e9f / m_rates[0];
  }

  size_t n_expect(const long interval, bool include_l0) const {
    using namespace Constants;
    float l0_rate = include_l0 ? m_rates[0] : 0.f;
    float l1_rate = std::accumulate(std::next(begin(m_rates)), end(m_rates), 0.f);
    // Fudge with 30% extra space to avoid reallocating
    double n_per_pmt = 1.3 * (l0_rate + 3.f * l1_rate) * (double)(interval) / 1e9;
    auto n_pmts = n_dom * n_mod * n_pmt;
    if (n_per_pmt > std::numeric_limits<float>::max() / n_pmts) {
      throw std::domain_error{"rate of " + std::to_string(n_per_pmt) + " is too large"};
    }
    return std::lround(n_pmts * (float)n_per_pmt);
  }

};

template<typename Container>
unsigned int random_index(const Container& buffer, const double random) {

  double x = std::accumulate(begin(buffer), end(buffer), 0.);

  if (x > 0.0) {
    x *= random;
    unsigned int index = 0;

    for (auto v : buffer) {
      if ((x -= v) < 0.0) {
        break;
      } else {
        ++index;
      }
    }

    if (index == buffer.size()) {
      --index;
    }
    return index;
  } else {
    throw std::domain_error("random_index(): zero or negative probability.");
  }
}

std::tuple<size_t, size_t>
fill_coincidences(storage_t& times, pmts_t& pmts, size_t idx,
                  const long time_start, const long time_end,
                  Generators& gens);

std::tuple<storage_t, storage_t> generate(const long start, const long end,
                                          Generators& gens, bool use_avx2);
