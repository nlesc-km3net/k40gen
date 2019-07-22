#include "constants.h"
#include "match1D.h"
#include "match3D.h"
#include "rotation.h"
#include "clusterize.h"
#include "partition.h"
#include<tuple>
#include<algorithm>
#include<iostream>
#include "pmt_storage.h"

using namespace std;

typedef vector<long> storage_t;
typedef tuple<storage_t, storage_t> Input;
typedef vector<pair<long, long> > Output;
typedef vector<vector<float> > matrix; 

const float grid_angle = 5.f * 2.f * PI / 360.f;

class Trigger3N
{
    public:
        Parameters parameters;
        Match3D match3d;
        Match1D match1d;
        Rotation rot;

        Trigger3N(Parameters input)
        {
            parameters = input;
            
            if(parameters.numberOfHits==0) 
                parameters.numberOfHits = 1;
      
            if(parameters.numberOfHits==1)
	            parameters.factoryLimit = 1;

            match3d = Match3D(parameters.TMaxEvent_ns);
            match1d = Match1D(parameters.MaxRoadWidth, parameters.TMaxEvent_ns);

            buffer_time.resize(parameters.factoryLimit);
            buffer_id.resize(parameters.factoryLimit);
        }

        long getNumModules(int p, int q, storage_t id); //Returns number of unique mod_id's between hit p and hit q

        long pmtId(int id); //return the pmt id

        void operator()(Input input,Output &output, PMT &pmt)
        {
            storage_t id = get<1>(input);
            storage_t time = get<0>(input);
            int size = id.size();
            vector<pair<float,float> > angles = rot.angle_grid(0.f, PI, grid_angle);

            if(parameters.enabled && size >= parameters.numberOfHits)
            {
                for(int p=0, q=p; p<size;)
                {
                    /*
                        Increase the index until the difference of time of hit(q) 
                        and current hit(p) is greater than TMaxEvent_ns 
                    */
                    while(time[q] - time[p] < parameters.TMaxEvent_ns) 
                        q++;

                    if(q-p+1 >=parameters.numberOfHits && getNumModules(p,q,id)>=parameters.numberOfModules)
                    {
                        if(q-p+1 < parameters.factoryLimit)
                        {
                            do
                            {
                                if(q-p+1 >=parameters.numberOfHits && getNumModules(p,q,id)>=parameters.numberOfModules)
                                {
                                    int root = 0, itp = 1, itq = 1;
                                    buffer_time[root] = time[p];
                                    buffer_id[root] = id[p];
                                
                                    for(int i = p+1; i!= q; i++) 
                                    {
                                        if(match3d(time[p], time[i], pmtId(id[p]),pmtId(id[i]),pmt))
                                        {
                                            buffer_time[itq] = time[i];
                                            buffer_id[itq] = id[i];
                                            ++itq; //itq me be out of bounds
                                        }
                                    }
                                    if((itq-root+1) >= parameters.numberOfHits && 
                                       getNumModules(root,itq,buffer_id) >= parameters.numberOfModules)
                                    {
                                        itq = clusterize(itp, itq, match3d, parameters.numberOfHits-1, buffer_time, buffer_id, pmt);

                                        if(itq-root+1 >= parameters.numberOfHits &&
                                            getNumModules(root,itq,buffer_id) >= parameters.numberOfModules)
                                        {
                                            for(auto it: angles)
                                            {
                                                for(int i=root; i<=itq; i++)
                                                {
                                                    long idx = pmtId(buffer_id[i]);
                                                    vector<float> dir(3);
                                                    dir = {pmt.dx[idx], pmt.dy[idx], pmt.dz[idx]};
                                                    matrix rot_mat = rot.rotation_matrix(it.first, it.second, dir);
                                                    dir = rot.rotate(rot_mat, dir);
                                                    // Update the directions
                                                    pmt.dx[idx] = dir[0];
                                                    pmt.dy[idx] = dir[1];
                                                    pmt.dz[idx] = dir[2];
                                                }
                                                int itz = partition(itp, itq, root, match1d, pmt, buffer_time, buffer_id);
                                                if(itz-root+1>= parameters.numberOfHits && 
                                                   getNumModules(root, itz, buffer_id) >= parameters.numberOfModules)
                                                {
                                                    itz = clusterize(itp, itz, match1d, parameters.numberOfHits-1, buffer_time,buffer_id, pmt);

                                                    if(itz-root+1>= parameters.numberOfHits && 
                                                       getNumModules(root, itz, buffer_id) >= parameters.numberOfModules)
                                                    {
                                                        output.push_back({buffer_time[root], buffer_time[itq]});
                                                        break;
                                                    }
                                                }
                                            }
                                        }                
                                    }
                                }
                            } while(time[q]-time[p] >= parameters.TMaxEvent_ns);
                        }
                        else
                        {
                            output.push_back({time[p], time[q]});
                            p = q;
                        }
                    }
                    else
                    {
                        while(time[q]-time[p] >= parameters.TMaxEvent_ns)
                            p++;
                    }
                }
            }
        }

    private:
        storage_t buffer_time; 
        storage_t buffer_id; 
        
};


long Trigger3N::getNumModules(int p, int q, storage_t id)
{
    vector<long> temp(q-p+1,0);

    for(int i=p; i<=q; i++)
    {
        for(int j=13; j<=31; j++)
        {
            if((1<<j)&id[i])
                temp[i-p] |= (1<<(j-13));
        }
    }

    sort(temp.begin(), temp.end());
    return distance(temp.begin(), unique(temp.begin(), temp.end()));
}


long Trigger3N::pmtId(int id)
{
   int res = 0;
   for(int i=8; i<=12; i++) 
   {
        if((1<<i)&id)
            res |= (1<<(i-8));
   }
   return res;
}
