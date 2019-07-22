#include "match1D.h"
#include "match3D.h"
#include "pmt_storage.h"
#include "constants.h"
#include "rotation.h"
// #include "generate/generate.h"
#include<iostream>
#include<array>
#include<algorithm>
using namespace std;

// static const double C_inv = getInverseSpeedOfLight();
// static const double Ind_Refraction = getIndexOfRefraction();
// static const double Tan_c = getTanThetaC();
const float grid_angle = 5.f * 2.f * PI / 360.f;
struct X
{
    int a;
    int b;
};
class P
{
    public:
        P(int a)
        {
            x = a;
        }
        int get()
        {
            return x;
        }
    private:
        int x;
};
int partition(int start, int end, vector<int> &v)
{
    int i = start, j = end-1;

    while(i<j)
    {
        bool does_match = v[i]%2;
        if(does_match) i++;
        else
        {
            while(j>i)
            {
                bool val = v[j]%2;
                if(!val) j--;
                else break;
            }
            swap(v[i],v[j]);
        }
    }

    return i;
}

int main()
{
    // PMT pmt;
    // Match3D<PMT> obj(C_inv,Ind_Refraction,Tan_c,0.01);
    // Match1D<PMT> obj2(C_inv,Tan_c,0.1,0.01);
    // Rotation rot;

    // (pmt.x).resize(6);
    // (pmt.y).resize(6);
    // (pmt.z).resize(6);
    // (pmt.x) = {1,2,3,4,5,6};
    // (pmt.y) = {4,5,6,1,3,2};
    // (pmt.z) = {3,2,5,6,1,4};
    
    // cout<<obj(0.01,0.005,1,4,pmt)<<endl;
    // cout<<obj2(0.01,0.005,1,4,pmt)<<endl;

    // auto angle = rot.angle_grid(0,PI,grid_angle);
    // cout<<angle.size()<<endl;
    //-----------------------------------------------
    /*
        Generate and call trigger.
    */
    // const array<float, 4> background_rates{7000.f, 700.f, 70.f, 7.f};
    // Generators generators{41431, 2340, background_rates};

    // const size_t n_slices = 10;
    // const size_t n_candidates = 10;
    // const long dt = lround(1e8);

    // for(size_t i = 0; i < n_slices; ++i) 
    // {
    //     auto hits = generate(i * dt, (i + 1) * dt, generators, "reference", false);
    //     // cout<<get<0>(hits).size()<<endl;

    // }
    // X a,b;
    // cin>>a.a>>a.b;
    // b = a;
    // cout<<b.a<<" "<<b.b<<endl;

    // P q(100);
    // // cout<<p.get()<<endl;
    // cout<<q.get()<<endl;
    vector<int> v = {2,1,6,5,4,8};
    auto p = partition(v.begin(),v.end(),[](int x) 
    { 
        return x%2;  
    });
    cout<<p-v.begin()<<endl;
    for(auto it: v) cout<<it<<" ";
    // vector<int> v = {2,1,6,5,4,8};
    // cout<<partition(0,6,v)<<endl;
    // for(auto it: v) cout<<it<<" ";
    // cout<<endl;
    return 0;
}