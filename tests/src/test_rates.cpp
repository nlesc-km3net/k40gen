#include <numeric>
#include <iostream>

#include <catch2/catch.hpp>

#include <test_functions.h>

using namespace std;

TEST_CASE( "Rates make sense", "[rates]" ) {
   auto check_rates = [](array<float, 4> rates, bool use_avx2) {
                         const auto [av_rate, av_n] = generate_times(rates, use_avx2);
                         auto coincidence_rate = std::accumulate(begin(rates), end(rates), 0.);
                         REQUIRE(std::abs(coincidence_rate - av_rate) / coincidence_rate < 1e-4);
                         cout << av_rate << " " << av_n << endl;
                      };

   check_rates({7000.}, false);
   check_rates({1000., 100.}, false);
   check_rates({7000., 700., 70., 7.}, false);
}
