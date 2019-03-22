#include <math.h>

#include <array>
#include <vector>
#include <random>
#include <map>
#include <pair>

#include <range/v3/core.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/sort.hpp>

#include <generate.h>

namespace {

template <std::size_t N>
struct get_n {
  template <typename T>
  auto operator()(T&& t) const ->
    decltype(std::get<N>(std::forward<T>(t))) {
    return std::get<N>(std::forward<T>(t));
  }
};

const float PI = 3.1415927;

using namespace ranges;

using std::array;
using std::map;
using std::vector;
using std:pair;

}

/* Logic for rotation of PMTs

    Initially it is assumed and verified from the data that parameter r = 1

    Now, changing the coordinate system based on new polar and azimuthal angle is basically rotating and/or
    shifting the point or PMT by the angle through which the coordinate system is rotated. So, if the polar
    angle is shifted by 2 degress than it's equivalent of changing the PMT direction in standard coordinates
    by that many degress. Same thing goes for azimuthal angle.

    Thus, the above logic is used to shift the point from one coordinate system to other.
*/

array <float, 3> rotate(float theta, float phi,
                       const array <float, 3>& pmt) {

  array <float, 3> new_pmt;

  // x = sin(theta)*cos(phi)
  // y = sin(theta)*sin(phi)
  // z = cos(theta)

  org_phi = atan(pmt[1] / pmt[0])

  org_theta = atan(pmt[1] / (pmt[2] * sin(org_phi)));

  // del theta = change in theta w.r.t. theta in original coordinate system
  // del phi = change in phi angle w.r.t. phi in original coordinate system

  // X = sin(original theta + del theta) * cos(original phi + del phi)
  new_pmt[0] = sin(org_theta + theta) * cos(org_phi + phi);

  // Y = sin(original theta + del theta) * sin(original phi + del phi)
  new_pmt[1] = sin(org_theta + theta) * sin(org_phi + phi);

  // Z = cos(original theta + del theta)
  new_pmt[2] = cos(org_theta + theta)

  return new_pmt;
}

// Using the exact reverse logic used in rotate method
array <float, 3> test_rotate(float theta, float phi,
                              const array <float, 3>& rotated_pmt) {

  array <float, 3> org_pmt;

  rot_phi = atan(rotated_pmt[1] / rotated_pmt[0])
  rot_theta = atan(rotated_pmt[1] / (rotated_pmt[2] * sin(rot_phi)));

  org_pmt[0] = sin(rot_theta - theta) * cos(rot_phi - phi);
  org_pmt[1] = sin(rot_theta - theta) * sin(rot_phi - phi);
  org_pmt[2] = cos(org_theta - theta);

  return org_pmt;
}


int main() {

  // Part 1: parse km3net_reference.detx into storage

  freopen("km3net_reference.detx","r",stdin);
  
  int num;
  cin >> num;

  string version;
  cin >> version;

  float x, y;
  cin >> x >> y;

  string t, m, n;
  cin >> t >> m >> n;

  float s1, s2, s3;
  cin >> s1 >> s2 >> s3;

  // Number of strings in the detector
  int num_strings;
  cin >> num_strings;

  vector <int> DOM_ID, MOD_ID, DOM_IN_MOD, PMT_IN_DOM;

  /* To store all the data in single variable to access it easily
     Structure of storing data - 

     Storing all the different DOMs data
     | 
     |     Storing data for a single DOM
     |     |
     |     |       Store the data in pair of index value with the corresponding positions and directions of PMT
     |     |       |
     |     |       |           Store the position and directions of single PMT in array format
     |     |       |           |
  */
  vector <vector <pair <int, vector <float> > > > data;

  // PMTs are also stored differently for easy access for the particular problem statement
  vector <array<float, 3> > PMTs;

  // 61*x + x = 128339 => x is approx 2070
  for(int i = 0; i < 2070; i++) {

    /*
    // DOM - DOM_ID
    // MOD - MOD_ID
    // DIM - No. of DOM within MOD
    // PID - No. of PMTs within this DOM
    */
    int DOM, MOD, DIM, PID;
    cin >> DOM >> MOD >> DIM >> PID;

    DOM_ID.push_back(DOM);
    MOD_ID.push_back(MOD);
    DOM_IN_MOD.push_back(DIM);
    PMT_IN_DOM.push_back(PID);

    vector <pair <int, vector<float> > > sub_data;

    // 60 entries for each DOM
    for(int j = 0; j < PID; j++) {

      // index of each entry
      int index; 
      cin>>index;

      vector<float> temp_data;
      array<float, 3> extra_PMT;

      // Postion and Directions for each particle
      float x, y, z, x_d, y_d, z_d;
      cin >> x >> y >> z >> x_d >> y_d >> z_d;
      temp_data.push_back(x);
      temp_data.push_back(y);
      temp_data.push_back(z);
      temp_data.push_back(x_d);
      temp_data.push_back(y_d);
      temp_data.push_back(z_d);

      extra_PMT[0] = x_d;
      extra_PMT[1] = y_d;
      extra_PMT[2] = z_d;

      // Calibration Constant
      float calibration_const;
      cin >> calibration_const;
      temp_data.push_back(calibration_const);

      pair <int, vector<float> > p (index, temp_data);

      sub_data.push_back(p);
      PMTs.push_back(extra_PMT);
    }

    data.push_back(sub_data);
  }

  fclose(stdin);

  // Generate random hits
  // The rates are Hz per number of hits on neighbouring PMTs; this
  // needn't be changed.
  const array<float, 4> background_rates{7000.f, 700.f, 70.f, 7.f};
  Generators generators{41431, 2340, background_rates};

  // KM3NeT data arrives in time slices of 100ms. Here this data is
  // generated to a reasonable approximation of reality. How that
  // works is outside the scope of this challenge. Here we look at the
  // equivalent of 1 second of data.
  const size_t n_slices = 10;

  // Select 10 sets of hits per slice
  const size_t n_candidates = 10;

  // Generate slices of 100 ms (1e8 ns)
  const long dt = std::lround(1e8);

  for (size_t i = 0; i < n_slices; ++i) {
    // Values are three numbers packed into a single 32bit integer
    // - 8 bits: time over threshold of this hit (ToT)
    // - 5 bits: PMT_ID
    // - rest  : 100 * (DOM_ID + 1) + MOD_ID + 1
    // The ToT time and ToT are not relevant for this challenge
    auto [times, values] = generate(i * dt, (i + 1) * dt, generators, true);

    // Sort by time, this ensures there is some spread in PMTs within
    // clusters
    ranges::sort(view::zip(times, values), std::less<>{}, get_n<0>{});

    // Part 2: randomly select hits to use as candidates
    // Suggestion: use a normally distributed number of hits with a mean
    // of 20 an a width of 5, but no less than 5
    for (size_t candidate = 0; candidate < n_candidates; ++candidate) {

      /*
        width >= 5 => S.D. >= 2.5
        So, taking Standard Deviation = 6 and Mean = 2, we get
        Range = [S.D. - 3 * Mean, S.D. + 3 * Mean] => Assuming almost 100% data lies in the range of [-3, 3]
        Thus, Range = [0, 12]
      */

      //                                        DOM indices
      //                                        |            No. of PMTs per DOM
      //                                        |            |
      //                                        |            |     Range is 12 so keeping the buffer distance
      //                                        |            |     |

      int random_start = 0 + ( std::rand() % ( (11518-101) * 31 - 12 - 0 ) )
      size_t hit_start = random_start, hit_end = random_start + 12;

      // Part 3: rotate selected hits over a grid of directions
      vector<array<float, 3>> result(hit_end - hit_start);

      float d_tp = 5.f * 2.f * PI / (360.f * 360.f);
      for (float theta = 0.f; theta < PI; theta += d_tp) {
        for (float phi = 0.f; phi < 2 * PI; phi += d_tp) {
          for (size_t i = hit_start; i < hit_end; ++i) {

            // Obtain PMT ID
            size_t pmt_id = i;
            
            // Apply rotation
            result[i] = pmt_rotation(theta, phi, PMTs[pmt_id]);
          }
        }
      }
    }
  }
}