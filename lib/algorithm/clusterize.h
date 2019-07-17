#include<vector>
#include "storage.h"
using namespace std;

template<class Match>
inline int clusterize(int start, int end, Match match, int Nmin, storage_t &buffer_time, storage_t &buffer_id)
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
                   pmtID(buffer_id[start+i]), pmtID(buffer_id[start+j]))
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
                   pmtID(buffer_id[start+i]), pmtID(buffer_id[start+n]))
                    --count[i];
            }
        }

        if(n >= Nmin) return start+n;
        return start;
    }
    return start;
}