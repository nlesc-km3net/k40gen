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
#include <cassert>
#include <stdexcept>
#include <vector>
#include <functional>
#include <iostream>

#include <tuple>
#include <optional>
#include <storage.h>
#include <generate_common.h>
#include <generate_scalar.h>

namespace {
// from fit to Gaussian part of ToT distribution
  const float tot_mean = Constants::tot_mean;
  const float tot_sigma = Constants::tot_sigma;

  using std::cout;
  using std::array;
}

float GenScalar::dot_product(const std::array<float, 3>& left, const std::array<float, 3>& right) {
  float r = 0.f;
  for (size_t i = 0; i < left.size(); ++i) {
    r += left[i] * right[i];
  }
  return r;
}

void fill_values_scalar(long idx_start, long idx_end, storage_t& values, Generators& gens, int dom, int mod,
                        std::function<unsigned int(size_t)> pmt_fun) {
   // fill values
  auto& mt = gens.mt;
  auto& flat = gens.flat;

  size_t n = 0;
  for (long vidx = idx_start; vidx < idx_end; vidx += 2) {
    auto pmt1 = pmt_fun(n++);
    auto pmt2 = pmt_fun(n++);

    auto u1 = flat(mt);
    auto u2 = flat(mt) * Constants::two_pi;

    auto fact = sqrt(-2.0f * log(u1));
    float z0 = sin(u2);
    float z1 = cos(u2);

    z0 = fact * tot_sigma * z0 + tot_mean;
    z1 = fact * tot_sigma * z1 + tot_mean;

    auto val0 = static_cast<int>(z0) | (pmt1 << 8) | ((100 * (dom + 1) + mod + 1) << 13);
    values[vidx] = val0;

    auto val1 = static_cast<int>(z1) | (pmt2 << 8) | ((100 * (dom + 1) + mod + 1) << 13);
    values[vidx + 1] = val1;
  }
}

std::tuple<storage_t, storage_t> generate_scalar(const long time_start, const long time_end,
                                                 Generators& gens) {

  auto& mt = gens.mt;
  auto& flat = gens.flat;

  const size_t n_expect = gens.n_expect(time_end - time_start, true);
  const float tau_l0 = gens.tau_l0();

  storage_t times(n_expect);
  storage_t values(n_expect + 1);
  const size_t n_expect_pmts = gens.n_expect(time_end - time_start, false);
  pmts_t pmts(n_expect_pmts, 0);
  size_t idx = 0;

  // First generate some data
  for (int dom = 0; dom < Constants::n_dom; ++dom) {
    for (int mod = 0; mod < Constants::n_mod; ++mod) {
      for (int pmt = 0; pmt < Constants::n_pmt; ++pmt) {
        size_t pmt_start = idx;
        long last = time_start;
        while(last < time_end && idx < times.size() - 2) {
          // Generate times
          float r = -1.f * tau_l0 * log(flat(mt));
          last += static_cast<long>(r + 0.5);
          times[idx++] = last;
        }
        fill_values_scalar(pmt_start, idx, values, gens, dom, mod,
                           [pmt](size_t) { return pmt; });
      }

      // Coincidences
      auto [n_times, _] = fill_coincidences(times, pmts, idx, time_start, time_end, gens);
      fill_values_scalar(idx, idx + n_times, values, gens, dom, mod,
                         [&pmts](size_t n) {
                           assert(n < pmts.size());
                           return pmts[n];
                         });
      idx += n_times;
    }
  }
  if (idx >= times.size() - 2) {
   std::cerr << "Warning: allocated space exceeded, event is truncated." << std::endl;
  }
  times.resize(idx);
  values.resize(idx);
  return {std::move(times), std::move(values)};
}
