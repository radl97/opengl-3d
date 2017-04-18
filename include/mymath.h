#ifndef MY_MATH_VER
#define MY_MATH_VER v1.0
#define PI 3.14159265358
#define PI_D2 1.57079137679

#include <math.h>

/*void vxm(float* vect,float* matr, float* dest);
void mxm(float* matr1,float* matr2, float* dest);*/
int createMatr(float* pozrot, float* dest);
int createMatrInv(float* pozrot, float* dest);
void norm(float* n);
int calcTBN(
float* v,float* uv, unsigned int* inds,
    float* t, float* b, float* n);
int calcTBN(float* v,float* uv,
    float* t, float* b, float* n);
void mxm(float* matr1,float* matr2, float* dest);
#endif
