#include <numeric>
#include <iostream>
#include <array>

#include <generate.h>
#include <storage.h>

#include <test_functions.h>

#include <TApplication.h>
#include <TH1.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TCanvas.h>
#include <TROOT.h>

using namespace std;

int main(int argc, char* argv[]) {

   gROOT->SetBatch();

   map<bool, unique_ptr<TH1I>> histos;

   array<float, 4> rates{7000., 0., 0., 0.};

   for (bool use_avx2 : {false, true}) {
      string name = string{"diffs_"} + (use_avx2 ? "avx2" : "scalar");
      auto r = histos.emplace(use_avx2, make_unique<TH1I>(name.c_str(), name.c_str(), 100, 0, 1000000));
      auto& time_histo = r.first->second;

      Generators gens{1052, 9523, rates};

      long dt = std::lround(1e8);

      long time_start = 0, time_end = time_start + dt;

      auto [times, values] = generate(time_start, time_end, gens, use_avx2);

      const size_t n_times = times.size();
      for (size_t i = 0; i < n_times - 1; ++i) {
         if (((values[i + 1]) >> 13) == (values[i] >> 13)) {
            time_histo->Fill(times[i + 1] - times[i]);
         }
      }
      TF1 expo{"exp", "expo", time_histo->GetBinCenter(1),
            time_histo->GetBinCenter(1 + time_histo->GetNbinsX())};
      auto fit = time_histo->Fit(&expo, "RS");
      // parameter is negative
      cout << std::fabs(rates[0] + (fit->Parameter(1) * 1e9)) / rates[0]  << endl;
   }

   TCanvas canvas{"canvas", "canvas", 600, 800};
   canvas.Divide(1, 2);

   for (auto& [arch, histo] : histos) {
      canvas.cd(arch + 1);
      histo->Draw();
   }
   canvas.Print("distributions.png");
}
