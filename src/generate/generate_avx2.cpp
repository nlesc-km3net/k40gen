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
#include <tuple>
#include <functional>

#include <Vc/Vc>
#include <instrset.h>
#include <vectori256.h>
#include <ranvec1.h>

#include <storage.h>
#include <generate_common.h>
#include <generate_avx2.h>


namespace {
// from fit to Gaussian part of ToT distribution
  const float_t tot_mean = Constants::tot_mean;
  const float_t tot_sigma = Constants::tot_sigma;

   using Vc::int_v;
   using Vc::float_v;
   using Vc::long_v;

   using std::pair;
}

float GenAVX2::dot_product(const Vc::SimdArray<float, 3>& left, const Vc::SimdArray<float, 3>& right) {
  return (left * right).sum();
}

inline int_v scan_AVX(int_v x) {
  // first shift then add
  auto t0 = permute8i<3, 0, 1, 2, 7, 4, 5, 6>(x.data());
  auto t1 = permute8i<-1, -1, -1, -1, 0, 1, 2, 3>(t0);
  x += _mm256_blend_epi32(t0, t1, 0x11);

  // second shift then add
  t0 = permute8i<2, 3, 0, 1, 6, 7, 4, 5>(x.data());
  t1 = permute8i<-1, -1, -1, -1, 0, 1, 2, 3>(t0);
  x += _mm256_blend_epi32(t0, t1, 0x33);

  // final shift and add
  x += int_v{permute8i<-1, -1, -1, -1, 0, 1, 2, 3>(x.data())};
  return x;
}

auto int_v_to_long_v(const int_v& in) -> pair<long_v, long_v>
{
   return {Vc::AvxIntrinsics::cvtepi32_epi64(Vc::AVX::lo128(in.data())),
           Vc::AvxIntrinsics::cvtepi32_epi64(Vc::AVX::hi128(in.data()))};
};

void fill_values_avx2(long idx_start, long idx_end, storage_t& values, Ranvec1& random,
                      int dom_id, std::function<int_v(size_t)> pmt_fun) {
   // fill values
   size_t n = 0;
   const long value_end = idx_end + (2 * long_v::size() - (idx_end % 2 * long_v::size()));
   for (long vidx = idx_start; vidx < value_end; vidx += 2 * long_v::size()) {
      int_v pmt1 = pmt_fun(n++);
      int_v pmt2 = pmt_fun(n++);

      auto u1 = float_v{random.random8f()};
      auto u2 = float_v{random.random8f()} * Constants::two_pi;

      auto fact = sqrt(-2.0f * log(u1));
      float_v z0, z1;
      sincos(u2, &z0, &z1);

      z0 = fact * tot_sigma * z0 + tot_mean;
      z1 = fact * tot_sigma * z1 + tot_mean;

      auto val0 = simd_cast<int_v>(z0) | (pmt1 << 8) | dom_id << 13;
      auto [val0_first, val0_second] = int_v_to_long_v(val0);

      val0_first.store(&values[vidx]);
      val0_second.store(&values[vidx + long_v::size()]);

      auto val1 = simd_cast<int_v>(z1) | (pmt2 << 8) | dom_id << 13;
      auto [val1_first, val1_second] = int_v_to_long_v(val1);
      val1_first.store(&values[vidx + 2 * long_v::size()]);
      val1_second.store(&values[vidx + 3 *long_v::size()]);
   }
}

std::tuple<storage_t, storage_t> generate_avx2(const long time_start, const long time_end,
                                               Generators& gens, dom_fun_t dom_fun) {

  Ranvec1 random{Constants::random_method};
  random.init(gens.seeds()[0], gens.seeds()[1]);

  // Assume times.size() is multiple of 8 here
  // assert(storage::n_hits % 16 == 0);

  const size_t n_expect = gens.n_expect(time_end - time_start, true);
  const size_t n_expect_pmts = gens.n_expect(time_end - time_start, false);
  const float tau_l0 = gens.tau_l0();
  size_t storage_size = n_expect + long_v::size() - n_expect % long_v::size();

  storage_t times; times.resize(storage_size);
  storage_t values; values.resize(storage_size + 2 * long_v::size());
  pmts_t pmts(n_expect_pmts + long_v::size() - n_expect_pmts % long_v::size(), 0);

  size_t idx = 0;

  // First generate some data
  for (int dom = 0; dom < Constants::n_dom; ++dom) {
    for (int mod = 0; mod < Constants::n_mod; ++mod) {
      auto dom_id = dom_fun(dom, mod);
      for (int pmt = 0; pmt < Constants::n_pmt; ++pmt) {
        size_t pmt_start = idx;
        long_v offset;
        offset.data() = _mm256_set1_epi64x(time_start);
        long last = time_start;
        while(last < time_end && idx < times.size() - 2 * long_v::size()) {
          // Generate times
          auto tmp = random.random8f();
          float_v r{tmp};

          // Exponentially distributed numbers
          r = -1.f * tau_l0 * log(r);

          auto tmp_i = simd_cast<int_v>(r + 0.5f);
          // Prefix sum from stackoverflow:
          // https://stackoverflow.com/questions/19494114/parallel-prefix-cumulative-sum-with-sse
          auto out = scan_AVX(tmp_i);

          auto [first, second] = int_v_to_long_v(out);
          first.data() = Vc::AVX::add_epi64(first.data(), offset.data());
          second.data() = Vc::AVX::add_epi64(second.data(), offset.data());

          first.store(&times[idx]);
          second.store(&times[idx + long_v::size()]);
          last = second[long_v::size() - 1];

          //broadcast last element
          offset.data() = permute4q<3, 3, 3, 3>(second.data());

          // Generate ToT as a gauss and pmt flat.
          // Only do it every other pass to make use of the double
          // output of the Box-Muller method
          // When filling, fill the past and current indices
          idx += 2 * long_v::size();
        }

        fill_values_avx2(pmt_start, idx, values, random, dom_id,
                         [pmt](size_t) { return int_v(pmt); });

      }

      // Coincidences
      auto [n_times, _] = fill_coincidences(times, pmts, idx, time_start, time_end, gens);
      fill_values_avx2(idx, idx + n_times, values, random, dom_id,
                       [&pmts](size_t n) {
                          return int_v(pmts.data() + n * int_v::size());
                       });
      idx += n_times;

    }
  }
  if (idx >= times.size() - 2 * long_v::size()) {
     std::cerr << "Warning: allocated space exceeded, event is truncated." << std::endl;
  }
  times.resize(idx);
  values.resize(idx);
  return {std::move(times), std::move(values)};
}
