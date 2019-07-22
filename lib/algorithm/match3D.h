#ifndef MATCH3D_H
#define MATCH3D_H

#include<math.h>
#include "pmt_storage.h"
#include "constants.h"

const double c_inv = getInverseSpeedOfLight();
const double ind_refraction = getIndexOfRefraction();

class Match3D
{
    public:
        double TMaxExtra_ns;

        Match3D(const double Tmax_ns = 0.0)
        {
            TMaxExtra_ns = Tmax_ns;
        }

        bool operator()(const long f_time, const long s_time, 
                     const long f_id, const long s_id, const PMT &pmt) 
        {
            double x = pmt.x[f_id] - pmt.x[s_id];
            double y = pmt.y[f_id] - pmt.y[s_id];
            double z = pmt.z[f_id] - pmt.z[s_id];
            double d = sqrt(x*x + y*y + z*z);
            double t = fabs(f_time-s_time);

            return t <= d * ind_refraction * c_inv + TMaxExtra_ns;
        }
};

#endif