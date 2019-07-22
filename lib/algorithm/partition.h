#ifndef PARTITION_H
#define PARTITION_H

#include "match1D.h"
using namespace std;
typedef vector<long> storage_t;

long pmtId(int id)
{
   int res = 0;
   for(int i=8; i<=12; i++) 
   {
        if((1<<i)&id)
            res |= (1<<(i-8));
   }
   return res;
}
/*
    Partitions the vector from [start,end) into 2 parts, left side being the true part and left being the false.
    
    returns -> first false index
*/
int partition(int start, int end, int root, Match1D &match, PMT &pmt, storage_t &time, storage_t &id)
{
    int i = start, j = end-1;

    while(i<j)
    {
        bool does_match = match(time[root], time[i], pmtId(id[root]), pmtId(id[i]),pmt);
        if(does_match) i++;
        else
        {
            while(j>i)
            {
                bool val = match(time[root], time[j], pmtId(id[root]), pmtId(id[j]),pmt);
                if(!val) j--;
                else break;
            }
            swap(time[i],time[j]);
            swap(id[i],id[j]);
        }
    }

    return i;
}

#endif