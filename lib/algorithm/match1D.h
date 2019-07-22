#ifndef MATCH1D_H
#define MATCH1D_H

#include<math.h>
#include "constants.h"
#include "pmt_storage.h"

const double C_inv = getInverseSpeedOfLight();
const double Tan_c = getTanThetaC();

class Match1D
{
    public:
        double RMax_m;
        double TMax_ns;
        double TMaxExtra_ns;

        Match1D(const double roadWidth_m = 0.0, const double Tmax_ns = 0.0)
        {
            RMax_m = roadWidth_m;
            TMaxExtra_ns = Tmax_ns;
            TMax_ns = 0.5 * RMax_m * Tan_c * C_inv + TMaxExtra_ns;
        }

        bool operator()(const long f_time, const long s_time, 
                        const long f_id, const long s_id, const PMT &pmt) 
        {
            double x = pmt.x[f_id] - pmt.x[s_id];
            double y = pmt.y[f_id] - pmt.y[s_id];
            double z = pmt.z[f_id] - pmt.z[s_id];
            double d = sqrt(x*x + y*y);
            double t = fabs(f_time-s_time-z*C_inv);

            if(t>TMax_ns) return false;

            if(d<= 0.5*RMax_m)
                return t <= d*Tan_c*C_inv + TMaxExtra_ns;

            else if(d<=RMax_m)
                return t <= (RMax_m-d)*Tan_c*C_inv + TMaxExtra_ns;

            return false;
        }
};

#endif
