#include "match1D.h"
#include "match3D.h"
#include "storage.h"
#include<tuple>
using namespace std;
typedef tuple<storage_t, storage_t> Input;
typedef tuple<long, long, long> Output;


struct Parameters
{
    Parameters(const bool option, const int number_of_hits,
		      const int number_of_modules, const double Tmax_ns,
		      const int factory_limit)
    {
        enabled = option;
        numberOfHits = number_of_hits;
        numberOfModules = number_of_modules;
	    TMaxEvent_ns = Tmax_ns;
	    factoryLimit = factory_limit;
    }

    bool enabled;
    int numberOfHits;
    int numberOfModules;
    double TMaxEvent_ns;
    // JRotator3D rotator;
    Match1D match1D;
    Match3D match3D;
    int factoryLimit;
};



class Trigger3N
{
    Parameters parameters;

    Trigger3N(const Parameters& input): parameters(input)
    {
        if(parameters.numberOfHits==0) 
            parameters.numberOfHits = 1;
      
        if(parameters.numberOfHits==1)
	        parameters.factoryLimit = 1;

        // buffer.resize(parameters.factoryLimit);
        get<0>(buffer).resize(parameters.factoryLimit);
        get<1>(buffer).resize(parameters.factoryLimit);
    }

    long getNumModules(int p, int q); //Returns number of unique mod_id's between hit p and hit q

    long pmtId(int id); //return the pmt id


    void operator()(Input input,Output &output) const
    {
        int size = input[0].size();
        storage_t id = get<1>(input);
        storage_t time = get<0>(input);

        if(parameters.enabled && size >= parameters.numberOfHits)
        {
            Match3D match3D;
            Match1D match1D;

            for(int p=0, int q=p; p<size;)
            {
                /*
                    Increase the index until the difference of time of hit(q) 
                    and current hit(p) is greater than TMaxEvent_ns 
                */
                while(time[q] - time[p] < parameters.TMaxEvent_ns) 
                    q++;

                if(q-p >=parameters.numberOfHits && getNumModules(p,q,input)>=parameters.numberOfModules)
                {
                    if(q-p < parameters.factoryLimit)
                    {
                        do
                        {
                            if(q-p >=parameters.numberOfHits && getNumModules(p,q,input)>=parameters.numberOfModules)
                            {
                                int root = 0, itp = 1, itq = 1;
                                buffer_time[root] = time[p];
                                buffer_id[root] = id[p];
		                    
                                for(int i = p+1; i!= q; i++) 
                                {
		                            if(match3D(time[p], time[i], pmtId(id[p]),pmtId(id[i])))
                                    {
                                        buffer_time[itq] = time[i];
                                        buffer_id[itq] = id[i];
		                                ++itq;
		                            }
		                        }
                                if(abs(itq-root) >= parameters.numberOfHits && 
                                   getNumModules(min(root,itq),max(itq,root),buffer_id) >= parameters.numberOfModules)
                                {
                                    itq = clusterize(itp, itq, match3D, parameters.numberOfHits - 1);

                                    if(abs(root-itq) >= parameters.numberOfHits &&
                                       getNumModules(min(root,itq),max(itq,root),buffer_id) >= parameters.numberOfModules)
                                    {
                                        
                                    }                
                                }
                            }
                        } while(time[q]-time[p] >= parameters.TMaxEvent_ns);
                    }
                    else
                    {
                        // out = 
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


long Trigger3N::getNumModules(int p, int q,const storage_t id)
{
    vector<long> temp(q-p+1,0);

    for(int i=p; i<=q; i++)
    {
        for(int j=13; j<=31; j++)
        {
            if((1<<j)&id[i])
                temp[i-p] |= (1<<(j-13))
        }
    }

    sort(temp.begin(), temp.end());
    return distance(temp.begin(), unique(temp.begin(), temp.end()));
}

int Trigger3N::pmtID(int id)
{
   int res = 0;
   for(int i=8; i<=12; i++) 
   {
        if((1<<i)&id)
            res |= (1<<(i-8));
   }
   return res;
}
