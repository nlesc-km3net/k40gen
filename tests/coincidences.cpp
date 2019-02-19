#include <array>
#include <numeric>
#include <random>
#include <iostream>

#include <generate.h>
#include <storage.h>

#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/sort.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace std;

template <std::size_t N>
struct get_n {
  template <typename T>
  auto operator()(T&& t) const ->
    decltype(std::get<N>(std::forward<T>(t))) {
      return std::get<N>(std::forward<T>(t));
  }
};

pair<double, double> coincidence_rate(array<float, 4> rates) {

   Generators gens{1052, 9523, rates};

   long dt = std::lround(1e9);

   const size_t n_expect = gens.n_expect(dt);
   storage_t times; times.resize(n_expect);
   storage_t values; values.resize(n_expect);

   size_t idx = 0;
   long time_start = 0, time_end = 0;
   double av = 0.;
   const int n_iter = 10000;
   for (int i = 0; i < n_iter; ++i) {
      time_end += dt;
      auto n_coincidence = fill_coincidences(times, idx, time_start, time_end, gens);
      idx = 0;
      av += n_coincidence / double{n_iter};
      time_start = time_end;
   }

   return {gens.coincidence_rate, av};
}

pair<double, double> generate_times(array<float, 4> rates, bool use_avx2) {

   Generators gens{1052, 9523, rates};

   long dt = std::lround(1e8);

   long time_start = 0, time_end = 0;
   time_end += dt;

   auto [times, values] = generate(time_start, time_end, gens, use_avx2);

   // Zip the ids and hits together to sort them together
   auto zipped = ranges::view::zip(times, values);

   // Sort the hits
   ranges::sort(zipped, std::less<long>{}, get_n<0>{});

   double av = 0.;
   const size_t n_times = times.size();
   for (size_t i = 0; i < n_times - 1; ++i) {
      av += static_cast<double>(times[i]) / n_times;
   }

   return {av, times.size()};
}

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

TEST_CASE( "Coincidences make sense", "[coincidence]" ) {
   auto check_coincidence = [](array<float, 4> rates) {
                               const auto [rate, av] = coincidence_rate(rates);
                               auto coincidence_rate = std::accumulate(std::next(begin(rates)), end(rates), 0.);
                               REQUIRE(rate == coincidence_rate);
                               if (rate != 0.) {
                                  REQUIRE(std::abs(rate - av) / rate < 1e-4);
                               }
                            };

   check_coincidence({7000., 700., 70., 7.});
   check_coincidence({1000., 100.});
   check_coincidence({7000.});
}
