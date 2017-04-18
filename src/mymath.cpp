#include "mymath.h"
#include <stdio.h>

void vxm(float* vect,float* matr, float* dest){//vect*matr=>dest
	int i;
	for(i=0;i<4;i++){
		dest[i]=
			matr[i+0]*vect[0]+
			matr[i+4]*vect[1]+
			matr[i+8]*vect[2]+
			matr[i+12]*vect[3];
	}
}

void mxv(float* vect,float* matr, float* dest){//vect*matr=>dest
	int i;
	for(i=0;i<4;i++){
		dest[i]=
			matr[i*4+0]*vect[0]+
			matr[i*4+1]*vect[1]+
			matr[i*4+2]*vect[2]+
			matr[i*4+3]*vect[3];
	}
}

void mxm(float* matr1,float* matr2, float* dest){
	int i;
	for(i=0;i<16;i++){
		int k=i%4,l=i/4;
		dest[i]=
			matr1[l*4+0]*matr2[k+0*4]+
			matr1[l*4+1]*matr2[k+1*4]+
			matr1[l*4+2]*matr2[k+2*4]+
			matr1[l*4+3]*matr2[k+3*4];
	}
}

//v[ind[0-2]*3+(0-2)]: vertices
//uv[ind[0-2]*2+(0-1)]: UV-coords
//ind[0-2]: indices
//t,b,n [0-2]: tangent, bitangent, normal
int calcTBN(
    float* v, float* uv, unsigned int* inds,
    float* t, float* b, float* n){
// v: v[0-2]: A.xyz  v[3-5]: B.xyz  v[6-8]: C.xyz
//uv: uv[0-1]: A.uv  uv[2-3]: B.uv  uv[4-5]: C.uv
//n=(B-A)x(C-A)
  float dax= v[inds[1]*3+0]- v[inds[0]*3+0];
  float day= v[inds[1]*3+1]- v[inds[0]*3+1];
  float daz= v[inds[1]*3+2]- v[inds[0]*3+2];

  float dbx= v[inds[2]*3+0]- v[inds[0]*3+0];
  float dby= v[inds[2]*3+1]- v[inds[0]*3+1];
  float dbz= v[inds[2]*3+2]- v[inds[0]*3+2];

  float dau=uv[inds[1]*2+0]-uv[inds[0]*2+0];
  float dav=uv[inds[1]*2+1]-uv[inds[0]*2+1];

  float dbu=uv[inds[2]*2+0]-uv[inds[0]*2+0];
  float dbv=uv[inds[2]*2+1]-uv[inds[0]*2+1];

  float r=1.0f/(dau*dbv-dav*dbu);
/*  tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
  bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

*/
  n[0]=+(day*dbz-daz*dby);
  n[1]=-(dax*dbz-daz*dbx);
  n[2]=+(dax*dby-day*dbx);

  t[0]=(dax*dbv-dbx*dav)*r;
  t[1]=(day*dbv-dby*dav)*r;
  t[2]=(daz*dbv-dbz*dav)*r;

  b[0]=(dbx*dau-dax*dbu)*r;
  b[1]=(dby*dau-day*dbu)*r;
  b[2]=(dbz*dau-daz*dbu)*r;

  return 0;//nincs meg megcsinalva
}

void norm(float* n){
  float length=sqrt(
    n[0]*n[0]+
    n[1]*n[1]+
    n[2]*n[2]);
  if(length==0)return;
  n[0]/=length;
  n[1]/=length;
  n[2]/=length;
}

int calcTBN(
    float* v, float* uv,
    float* t, float* b, float* n){
  float dax= v[1*3+0]- v[0*3+0];
  float day= v[1*3+1]- v[0*3+1];
  float daz= v[1*3+2]- v[0*3+2];

  float dbx= v[2*3+0]- v[0*3+0];
  float dby= v[2*3+1]- v[0*3+1];
  float dbz= v[2*3+2]- v[0*3+2];

  float dau=uv[1*2+0]-uv[0*2+0];
  float dav=uv[1*2+1]-uv[0*2+1];

  float dbu=uv[2*2+0]-uv[0*2+0];
  float dbv=uv[2*2+1]-uv[0*2+1];

  if(dau*dbv!=dav*dbu)return 1;

  float r=1.0f/(dau*dbv-dav*dbu);

  n[0]=+(day*dbz-daz*dby);
  n[1]=-(dax*dbz-daz*dbx);
  n[2]=+(dax*dby-day*dbx);

  t[0]=(dax*dbv-dbx*dav)*r;
  t[1]=(day*dbv-dby*dav)*r;
  t[2]=(daz*dbv-dbz*dav)*r;

  b[0]=(dbx*dau-dax*dbu)*r;
  b[1]=(dby*dau-day*dbu)*r;
  b[2]=(dbz*dau-daz*dbu)*r;

  for(int i=0;i<3;i++)printf("%f ",n[i]);printf("\n");
  for(int i=0;i<3;i++)printf("%f ",t[i]);printf("\n");
  for(int i=0;i<3;i++)printf("%f ",b[i]);printf("\n");

  return 0;
}

int createMatrInv(float* prs, float* dest2){
  float dest[16];
  {
	  float x[16]={
	    1,  0,  0,  -prs[0],
	    0,  1,  0,  -prs[1],
		  0,  0,  1,  -prs[2],
		  0,  0,  0,  1
	  };
    for(int i=0;i<16;i++)
      dest2[i]=x[i];
  }
  {
	  float ca=cos(-prs[5]);//roll
	  float sa=sin(-prs[5]);
	  float x[16]={
	   ca,-sa,  0,  0,
	   sa, ca,  0,  0,
		  0,  0,  1,  0,
		  0,  0,  0,  1
	  };
    mxm(x,dest2,dest);
  }
  {
	  float ca=cos(-prs[4]);//yaw
	  float sa=sin(-prs[4]);
	  float x[16]={
	   ca,  0,-sa,  0,
		  0,  1,  0,  0,
	   sa,  0, ca,  0,
		  0,  0,  0,  1
	  };
    mxm(x,dest,dest2);
  }
  {
	  float ca=cos(-prs[3]);//pitch
	  float sa=sin(-prs[3]);
	  float x[16]={
		  1,  0,  0,  0,
		  0, ca,-sa,  0,
		  0, sa, ca,  0,
		  0,  0,  0,  1
	  };
    mxm(x,dest2,dest);
  }
  {
    if(!prs[6]||!prs[7]||!prs[8]){
      printf("CMI failed.\n");
      return 0;
    }
    float x[16]={
      1/prs[6], 0, 0, 0,
      0, 1/prs[7], 0, 0,
      0, 0, 1/prs[8], 0,
      0, 0, 0, 1
    };
    mxm(x, dest, dest2);
  }
  return 1;
}

int createMatr(float* prs, float* dest){
  float dest2[16];
  {
    if(!prs[6]||!prs[7]||!prs[8]){
      printf("CM failed.\n");
      return 0;
    }
    float x[16]={
      prs[6], 0, 0, 0,
      0, prs[7], 0, 0,
      0, 0, prs[8], 0,
      0, 0, 0, 1
    };
    for(int i=0;i<16;i++)
      dest[i]=x[i];
  }
  {
	  float ca=cos(prs[3]);//pitch
	  float sa=sin(prs[3]);
	  float x[16]={
		  1,  0,  0,  0,
		  0, ca,-sa,  0,
		  0, sa, ca,  0,
		  0,  0,  0,  1
	  };
    mxm(x, dest, dest2);
  }
  {
	  float ca=cos(prs[4]);//yaw
	  float sa=sin(prs[4]);
	  float x[16]={
	   ca,  0,-sa,  0,
		  0,  1,  0,  0,
	   sa,  0, ca,  0,
		  0,  0,  0,  1
	  };
    mxm(x,dest2,dest);
  }
  {
	  float ca=cos(prs[5]);//roll
	  float sa=sin(prs[5]);
	  float x[16]={
	   ca,-sa,  0,  0,
	   sa, ca,  0,  0,
		  0,  0,  1,  0,
		  0,  0,  0,  1
	  };
    mxm(x,dest,dest2);
  }
  {
	  float x[16]={
	    1,  0,  0,  prs[0],
	    0,  1,  0,  prs[1],
		  0,  0,  1,  prs[2],
		  0,  0,  0,  1
	  };
    mxm(x,dest2,dest);
  }
  return 1;
}

