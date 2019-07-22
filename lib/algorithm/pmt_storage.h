#ifndef PMT_STORAGES
#define PMT_STORAGES

#include<vector>
using namespace std;

struct PMT
{
    vector<float> x;
    vector<float> y;
    vector<float> z;
    vector<float> dx;
    vector<float> dy;
    vector<float> dz;
};

struct Parameters
{
    bool enabled;
    int numberOfHits;
    int numberOfModules;
    double TMaxEvent_ns;
    double MaxRoadWidth;
    int factoryLimit;
};

#endif
