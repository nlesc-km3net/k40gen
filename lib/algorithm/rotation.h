#include<vector>
#include<math.h>
#include<utility>
using namespace std;

typedef vector<vector<float> > matrix; 

class Rotation
{
    public:
        vector<pair<float, float> > angle_grid(float theta_min, float theta_max, float grid_angle);
        matrix matmul(const matrix &mat1, const matrix &mat2);
        matrix matadd(const matrix &mat1, const matrix &mat2);
        matrix scalar_mul(float val, const matrix &mat);
        matrix rotation_matrix(float theta, float phi, vector<float> vec);
        vector<float> rotate(const matrix& mat, const vector<float>& dir);

    private:
        const float PI = 3.1415927;
};

// function to return grid of angles
vector<pair<float, float> > Rotation::angle_grid(float theta_min, float theta_max, float grid_angle) 
{
    vector<pair<float, float>> angles;
    
    if (theta_min < 0.0) { theta_min = 0.0; }
    if (theta_min > PI)  { theta_min = PI; }
    if (theta_max < 0.0) { theta_max = 0.0; }
    if (theta_max > PI)  { theta_max = PI; }

    if(theta_max < theta_min)
        swap(theta_max, theta_min);

    const float rad  = theta_max - theta_min;
    const float bin  = rad / floor(rad / (1.4 * grid_angle) + 0.5);    // polar angle step size
    const float unit = 2.0 * sqrt(1.0 - cos(grid_angle));              // azimuth angle unit step size

    for (float theta = theta_min; theta < theta_max + 0.5 * bin; theta += bin) 
    {
        float step = 0.f;

        if(theta > 0.5 * bin && PI - theta > 0.5 * bin) 
        {
            step = PI / floor(PI * sin(theta) / unit + 0.5);              // polar angle dependent step size
        }
        else 
        {
            step = 2.0 * PI;                                            // pole
        }

        for(float phi = 0.f; phi < 2.f * PI - 0.5f * step; phi += step)
        {
            angles.push_back({theta,phi});
        }
    }
    angles.shrink_to_fit();
    return angles;
}

// function to multiply 2 matrices
matrix Rotation::matmul(const matrix &mat1, const matrix &mat2) 
{ 
    matrix res(3,vector<float>(3,0.f));

    for(int i=0; i<3; i++) 
    { 
        for(int j=0; j<3; j++) 
        { 
            for(int k=0; k<3; k++) 
                res[i][j] += mat1[i][k]*mat2[k][j]; 
        } 
    } 

    return res;
}

// function to add 2 matrices
matrix Rotation::matadd(const matrix &mat1, const matrix &mat2) 
{ 
    matrix res(3,vector<float>(3,0.f));

    for(int i=0; i<3; i++) 
    { 
        for(int j=0; j<3; j++)  
            res[i][j] = mat1[i][j]+mat2[i][j]; 
    } 

    return res;
} 

// function for multiplication of a matrix with a scalar
matrix Rotation::scalar_mul(float val, const matrix &mat)
{
    matrix result;

    for(int i=0; i<3; i++) 
        for(int j=0; j<3; j++) 
            result[i][j] = mat[i][j]*val; 

    return result;
}


/*
    Rotation matrix is returned for given theta and phi
    Rodrigues Rotation is used.
*/
matrix Rotation::rotation_matrix(float theta, float phi, vector<float> vec)
{
    matrix R_a(3, vector<float>(3));
    R_a = {{cos(phi), (-1)*sin(phi), 0.f},
           {sin(phi), cos(phi)     , 0.f},
           {0.f     , 0.f          , 1.f}};

    matrix I(3, vector<float>(3));
    I = {{1.f, 0.f, 0.f},   
         {0.f, 1.f, 0.f},
         {0.f, 0.f, 1.f}};

    matrix K(3, vector<float>(3));
    K = {{0.f, 0.f, (-1)*vec[0]},  
         {0.f, 0.f, (-1)*vec[1]},
         {vec[0], vec[1], 0.f}};
                
    matrix K_2 = matmul(K,K);

    K = scalar_mul(sin(theta), K);
    K_2 = scalar_mul(1-cos(theta), K_2);

    matrix R_b = matadd(K, K_2); 

    R_b = matadd(I,R_b);

    matrix transformation_matrix = matmul(R_a, R_b);

    return transformation_matrix;
}

// performs actual rotation
vector<float> Rotation::rotate(const matrix& mat,const vector<float>& dir)
{
    vector<float> res(3);

    /*
        VEC_new = Transformation_matrix * VEC_old
    */

    for(int i=0; i<3; i++) 
    { 
        for(int k=0; k<3;k++) 
            res[i] += mat[i][k] * dir[k]; 
    } 

    return res;
}