#include "constants.h"
#include<math.h>
#include "pmt_storage.h"

static const double C_inv = getInverseSpeedOfLight();
static const double Ind_Refraction = getIndexOfRefraction();
PMT pmt;

class Match3D
{
    public:
        double TMaxExtra_ns;

        Match3D(const double Tmax_ns = 0.0)
        {
            TMaxExtra_ns = Tmax_ns;
        }
        /*
            Arguments:
                time of first hit
                pmt_id of first hit
                time of second hit
                pmt_id of second hit

        */
        bool operator()(const long f_time, const long s_time, 
                        const long f_id, const long s_id) 
        {
            double x = pmt.x[f_id] - pmt.x[s_id];
            double y = pmt.y[f_id] - pmt.y[s_id];
            double z = pmt.z[f_id] - pmt.z[s_id];
            double d = sqrt(x*x + y*y + z*z);
            double t = fabs(f_time-s_time);

            return t <= d * Ind_Refraction * C_inv + TMaxExtra_ns;
        }
};


