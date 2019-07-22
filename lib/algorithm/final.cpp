#include "algo.h"
#include "pmt_storage.h"
// #include "generate.h"
#include<fstream>
#include<iostream>
using namespace std;

int main()
{
    /*
        Storing PMT information;
    */
    PMT pmt;
    ifstream f;
    f.open("km3net_reference.detx");
    string str;
    int n_strings,flag=0;
  
    // ignore the first 3 lines
    while(flag < 3) {  
        getline(f,str);
        flag++;
    }

    f >> n_strings;

    int dom_id,mod_id,number,cnt_pmt;

    while(!f.eof()) {

        f >> dom_id >> mod_id >> number >> cnt_pmt ;
        int id; double x,y,z,dx,dy,dz,c;

        for(int i=0; i<cnt_pmt; i++)
        {
            f>>id>>x>>y>>z>>dx>>dy>>dz>>c;
            pmt.x.push_back(x);
            pmt.y.push_back(y);
            pmt.z.push_back(z);
            pmt.dx.push_back(dx);
            pmt.dy.push_back(dy);
            pmt.dz.push_back(dz);
        }
    }
    f.close();
    // cout<<pmt.x.size()<<endl;
    //-------------------------------------------------

    /*
        Initializing required objects
    */
    Parameters para;
    
    para.enabled = 1;
    para.numberOfHits = 1;
    para.numberOfModules = 1;
    para.TMaxEvent_ns = 1;
    para.MaxRoadWidth = 1;
    para.factoryLimit = 1;
    
    tuple<storage_t, storage_t> inp;
    vector<pair<long,long> > out;
    
    Trigger3N trig(para);
    // obj(inp,out,pmt);

    //----------------------------------------------
    /*
        Generate hits and call trigger
    */
   /*
    const array<float, 4> background_rates{7000.f, 700.f, 70.f, 7.f};
    Generators generators{41431, 2340, background_rates};

    const size_t n_slices = 10;
    const size_t n_candidates = 10;
    const long dt = lround(1e8);

    for(size_t i = 0; i < n_slices; ++i) 
    {
        auto hits = generate(i * dt, (i + 1) * dt, generators, "reference", false);

        // Call the trigger
        trig(hits, out, pmt);

        for(auto it: out) cout<<it.first<<" "<<it.second<<endl;
        cout<<endl;
    }
    */

    
    return 0;
}

