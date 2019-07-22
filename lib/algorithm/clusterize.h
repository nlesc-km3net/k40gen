#include<vector>
#include<math.h>
#include "pmt_storage.h"
using namespace std;
typedef vector<long> storage_t;

long pmtID(int id)
{
   int res = 0;
   for(int i=8; i<=12; i++) 
   {
        if((1<<i)&id)
            res |= (1<<(i-8));
   }
   return res;
}


template<typename Match>
inline int clusterize(int start, int end, Match match, int Nmin, storage_t &buffer_time, storage_t &buffer_id, PMT &pmt)
{
    int N = abs(end-start+1);

    if(N >= Nmin)
    {
        int i, j, n = N;
        vector<int> count(N,1);

        for(i=0; i!=N; i++)
        {
            for(j=i+1; j!=N; j++)
            {
                if(match(buffer_time[start+i], buffer_time[start+j],
                   pmtID(buffer_id[start+i]), pmtID(buffer_id[start+j]),pmt))
                {
                    ++count[i];
                    ++count[j];
                }
                else if(n == Nmin)
                {
                    return start;
                }
            }

            if(count[i] < Nmin)
            {
                --n;
                if(n < Nmin)
                    return start;
            }
        }

        n = N;

        for(; n >= Nmin;)
        {
            j = 0;
            for(i=1; i!=n; ++i)
            {
                if(count[i]<count[j]) 
                    j = i;
            }

            if(count[j] == n) break;

            --n;

            swap(buffer_time[start+j],buffer_time[start+n]);
            swap(buffer_id[start+j],buffer_id[start+n]);
            swap(count[j],count[n]);

            for(i=0; i!=n; ++i)
            {
                if(match(buffer_time[start+i], buffer_time[start+n],
                   pmtID(buffer_id[start+i]), pmtID(buffer_id[start+n]), pmt))
                    --count[i];
            }
        }

        if(n >= Nmin) return start+n;
        return start;
    }
    return start;
}

