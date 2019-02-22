#include <numeric>
#include <iostream>

#include <catch2/catch.hpp>

#include <test_functions.h>

using namespace std;

TEST_CASE( "Rates make sense", "[rates]" ) {
   auto check_rates = [](float l0_rate, bool use_avx2) {
                         const auto [av_rate, av_n] = generate_l0(l0_rate, use_avx2);
                         REQUIRE(std::abs(l0_rate - av_rate) / l0_rate < 1e-4);
                         cout << av_rate << " " << av_n << endl;
                      };

   for (auto [rate, avx2] : {make_pair(7000.f, false),
                             make_pair(1000.f, false)
#ifdef USE_AVX2
                           , make_pair(7000.f, true),
                             make_pair(1000.f, true)
#endif
                            }) {
      check_rates(rate, avx2);
   }
}
