#include <numeric>

#include <catch2/catch.hpp>

#include <test_functions.h>

using namespace std;

TEST_CASE( "Coincidences make sense", "[coincidence]" ) {
   auto check_coincidence = [](array<float, 4> rates) {
                               const auto [rate, av] = coincidence_rate(rates);
                               auto coincidence_rate = std::accumulate(std::next(begin(rates)), end(rates), 0.);
                               REQUIRE(rate == coincidence_rate);
                               if (rate != 0.) {
                                  REQUIRE(std::abs(rate - av) / rate < 1e-3);
                               }
                            };

   check_coincidence({7000., 700., 70., 7.});
   check_coincidence({10000., 1000.});
   check_coincidence({7000.});
}
