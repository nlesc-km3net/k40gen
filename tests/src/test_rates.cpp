#include <numeric>
#include <iostream>

#include <catch2/catch.hpp>

#include <test_functions.h>

using namespace std;

TEST_CASE( "Rates makes sense", "[rates]" ) {
   long dt = std::lround(1e8);
   auto check_rates = [dt](float l0_rate, bool use_avx2) {
                         const auto [av_rate, av_n] = generate_l0(l0_rate, dt, use_avx2);
                         const float l0_lam = 1.f / l0_rate;
                         REQUIRE(std::fabs(l0_lam - av_rate / std::lround(1e9)) / l0_lam < 1e-2);
                      };

      for (auto [rate, avx2] : {make_pair(7000.f, false),
                             make_pair(1000.f, false),
                             make_pair(10000.f, false)
#ifdef USE_AVX2
                           , make_pair(7000.f, true),
                             make_pair(10000.f, true)
#endif
                            }) {
      check_rates(rate, avx2);
   }
}
