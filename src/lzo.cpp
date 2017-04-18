#include <GL/glew.h>
#include "mymath.h"
#include "lzo.h"
#include "lzi.h"
#include <stdio.h>

//#define LZO_BUF_SIZE 10

/*
struct ObjPack
	GLuint vao;
    //OpenGL's vertex array object
	GLenum mode;//GL_TRIANGLES, etc.
	GLenum type;//GL_UNSIGNED_INT, etc.
	float mm[16];//modlMatr
	bool vaas[MAX_VAAC];
    //number of attribs

	GLsizei pc;//num of polys

	GLuint prog;//OpenGL program

[LZOA]

*/

/*extern GLuint glGenVertexArrays(GLuint, GLuint*);
extern void glBindVertexArray(GLuint);
extern GLuint glGenBuffers(GLuint, GLuint*);
extern void glBindBuffer(GLenum, GLuint);
extern void glBufferData(GLenum, GLuint, void*, GLenum);
extern void glVertexAttribPointer(GLuint, GLuint, GLenum, GLenum, GLuint, void*);

extern GLenum GL_ARRAY_BUFFER;
extern GLenum GL_STATIC_DRAW;
extern GLenum GL_ELEMENT_ARRAY_BUFFER;*/

/*
[LZOA] [VER1] (NUMV) (NUME) (VXYZ)
    (TEXC) (NORM) [ELEM] (ELEMS)
*/

/*
 ABCDEFGHIJKLMNOPQRSTUVWXYZ
0123456789ABCDEF0123456789A


*/
#include <vector>

struct harmas{
  unsigned int v,t,n;
  harmas(unsigned int vv, unsigned int tt, unsigned int nn){
    v=vv;
    t=tt;
    n=nn;
  }
};

bool comp3mas(const struct harmas x,const struct harmas y){
  if(x.v!=y.v)return false;
  if(x.t!=y.t)return false;
  return (x.n==y.n);
}

inline int ctfwrite(FILE* f, const char* buf, int N){
  if(fwrite(buf, 1, N, f)!=(size_t)N){
    return 0;
  }
  return 1;
}

int ObjtoLZO(const char* infn, const char* outfn){
  //obj file
  //normalized
  //pos, texcoord, normal
  FILE* f=fopen(infn,"r");
  if(!f){
    printf("Failed to load %s\n", infn);
    return 0;
  }
  float* poz1=new float[MAX_VERTS1*3];
  float* tc1=new float[MAX_VERTS1*2];
  float* norm1=new float[MAX_VERTS1*3];

  unsigned int* elem=new unsigned int[MAX_VERTS2*3];

  int pi=0,ti=0,ni=0;

  int ei=0;

  std::vector<harmas> halm;

  fseek (f, 0, SEEK_SET);
  char* line=new char[80];
  char* adat=new char[80];
  while(!feof(f)){
    fgets(line, 79, f);
    float a,b,c;
    if(line[0]=='v'){
      sscanf(line, "%*s %f %f %f", &a, &b, &c);
      if(line[1]==' '){
        poz1[pi*3+0]=a;
        poz1[pi*3+1]=b;
        poz1[pi*3+2]=c;
        pi++;
        if(pi>=MAX_VERTS1){
          printf("Tul sok xyz harmas\n");
          return 0;
        }
      }else if(line[1]=='t'){
        tc1[ti*2+0]=a;
        tc1[ti*2+1]=b;
        ti++;
        if(ti>=MAX_VERTS1){
          printf("Tul sok uv par\n");
          return 0;
        }
      }else if(line[1]=='n'){
        norm1[ni*3+0]=a;
        norm1[ni*3+1]=b;
        norm1[ni*3+2]=c;
        ni++;
        if(ni>=MAX_VERTS1){
          printf("Tul sok normalxyz harmas\n");
          return 0;
        }
      }
    }else if(line[0]=='f'){
      unsigned int a1,a2,a3,b1,b2,b3,c1,c2,c3;
      sscanf(line, "%*s %d/%d/%d %d/%d/%d %d/%d/%d",&a1,&a2,&a3,&b1,&b2,&b3,&c1,&c2,&c3);
      //A egy csucs, B egy csucs es C egy csucs
      int i=0;
      a1--;
      a2--;
      a3--;
      b1--;
      b2--;
      b3--;
      c1--;
      c2--;
      c3--;
      int N=halm.size();
      for(int j=0;j<3;j++){
        harmas x(0,0,0);
        if(j==0){
          x=harmas(a1,a2,a3);
        }else if(j==1){
          x=harmas(b1,b2,b3);
        }else{
          x=harmas(c1,c2,c3);
        }
        bool p=false;//van ilyen
        while(i<N&&!p){
          p=comp3mas(x,halm[i]);
          if(!p)i++;
        }
        if(p){
          printf("  volt mar ilyen elem (%d)\n", i);
          elem[ei]=i;
          ei++;
        }else{
          printf("  nem volt meg ilyen elem (%d)\n", i);
          halm.push_back(x);
          elem[ei]=N;
          ei++;
          N++;
        }
      }
    }
  }

  FILE* ff=fopen(outfn,"wb");
  if(!ctfwrite(ff,"LZOAVER1",8)){
    fclose(f);
    return 0;
  }

  int pn=halm.size();
  int en=ei;

  if(!ctfwrite(ff,(char*)&pn,4)){
    fclose(f);
    return 0;
  }

  if(!ctfwrite(ff,(char*)&en,4)){
    fclose(f);
    return 0;
  }

  printf("pi=%d\n", pi);
  for(int i=0;i<pn;i++){
    int x=halm[i].v;
    printf("halm[%d].t=%d\n", i, x);
    if(!ctfwrite(ff,(char*)&poz1[x*3+0],4)){
      fclose(f);
      return 0;
    }
    if(!ctfwrite(ff,(char*)&poz1[x*3+1],4)){
      fclose(f);
      return 0;
    }
    if(!ctfwrite(ff,(char*)&poz1[x*3+2],4)){
      fclose(f);
      return 0;
    }
  }

  for(int i=0;i<pn;i++){
    int x=halm[i].t;
    printf("halm[%d].t=%d\n", i, x);
    if(!ctfwrite(ff,(char*)&tc1[x*2+0],4)){
      fclose(f);
      return 0;
    }
    if(!ctfwrite(ff,(char*)&tc1[x*2+1],4)){
      fclose(f);
      return 0;
    }
  }

  printf("ni=%d\n", ni);
  for(int i=0;i<pn;i++){
    int x=halm[i].n;
    printf("halm[%d].n=%d\n", i, x);
    if(!ctfwrite(ff,(char*)&norm1[x*3+0],4)){
      fclose(f);
      return 0;
    }
    if(!ctfwrite(ff,(char*)&norm1[x*3+1],4)){
      fclose(f);
      return 0;
    }
    if(!ctfwrite(ff,(char*)&norm1[x*3+2],4)){
      fclose(f);
      return 0;
    }
  }

  if(!ctfwrite(ff,"ELEM",4)){
    fclose(f);
    return 0;
  }

  for(int i=0;i<en;i++){
    if(!ctfwrite(ff,(char*)&elem[i],4)){
      fclose(f);
      return 0;
    }
  }

  delete[] poz1;
  delete[] tc1;
  delete[] norm1;
  delete[] elem;

  delete[] line;
  delete[] adat;
  return 1;
}

unsigned int test[6]={
0, 2, 1,
1, 2, 3};

int createTestFile(const char* fn){
  //[LZOA[VER1
  //(0008(
  //+-1,+-1
  FILE* f=fopen(fn,"wb");
  if(!ctfwrite(f,"LZOAVER1",8)){
    fclose(f);
    return 0;
  }
  int a=4;
  if(!ctfwrite(f,(char*)&a,4)){
    fclose(f);
    return 0;
  }
  a=6;
  if(!ctfwrite(f,(char*)&a,4)){
    fclose(f);
    return 0;
  }
  float ax[3];
  if(sizeof(float)!=4){
    printf("FUCK YOU!\n");
  }
  for(int i=0;i<4;i++){
    ax[0]=i/2*2-1;
    ax[1]=i%2*2-1;
    ax[2]=0;
    if(!ctfwrite(f,(char*)&ax,sizeof(float)*3)){
      fclose(f);
      return 0;
    }
  }
  for(int i=0;i<4;i++){
    ax[0]=i/2;
    ax[1]=i%2;
    if(!ctfwrite(f,(char*)&ax,sizeof(float)*2)){
      fclose(f);
      return 0;
    }
  }
  for(int i=0;i<4;i++){
    ax[0]=0;
    ax[1]=0;
    ax[2]=1;
    if(!ctfwrite(f,(char*)&ax,sizeof(float)*3)){
      fclose(f);
      return 0;
    }
  }
  a=0x4D454C45;
  if(!ctfwrite(f,(char*)&a,sizeof(float))){
    fclose(f);
    return 0;
  }
  if(!ctfwrite(f,(char*)test,sizeof(test))){
    fclose(f);
    return 0;
  }
  return 1;
}

int loadLZO(const char* fn, ObjPack* x){
  FILE* f=fopen(fn,"rb");
  if(!f){
    printf("Failed to load %s\n", fn);
    return 0;
  }
  fseek (f, 0, SEEK_SET);
  //unsigned int *buf=
  //    new unsigned int[LZO_BUF_SIZE];
  int bb[2];
  if(!fread((void *)bb, sizeof(int),
      2, f)){
    printf("Nee!\n");
    fclose(f);
    return 0;
  }
  if(bb[0]!=0x414F5A4C){// [LZOA]
    fclose(f);
    return 0;
  }
  if(bb[1]==0x31524556){// [VER1]
    //prog nincs
    //be: numv, nume, vecs, polys,
    //      lzi file (relative)
    //elem: GL_UNSIGNED_INT
    //polys: TRIANGLE
    //vaas: pos, uv, norm
    //vao-t generalni
    x->prog=0;
    x->mode=GL_TRIANGLES;
    x->type=GL_UNSIGNED_INT;
    bool* y=x->vaas;
    for(int i=0;i<MAX_VAAC;i++)y[i]=0;

    y[VEC_ATTRIB]=y[TC_ATTRIB]=true;//-COL (!!)
    y[NORM_ATTRIB]=y[BITA_ATTRIB]=y[TANG_ATTRIB]=true;
    unsigned int numv,nume;

    if(!fread((void *)&numv,
        sizeof(unsigned int), 1, f)){
      printf("Read numv failed\n");
      fclose(f);
      return 0;
    }
    if(!fread((void *)&nume,
        sizeof(unsigned int), 1, f)){
      printf("Read nume failed\n");
      fclose(f);
      return 0;
    }
    x->pc=nume;

    //GL
    glGenVertexArrays(1,&(x->vao));
    glBindVertexArray(x->vao);

    GLuint bufs[6];
    glGenBuffers(6,bufs);

    //gl

    GLfloat *vec=
        new GLfloat[numv*3];

    GLfloat *tc=
        new GLfloat[numv*2];

    GLfloat *tang=
        new GLfloat[numv*3];

    GLfloat *bita=
        new GLfloat[numv*3];

    GLfloat *normal=
        new GLfloat[numv*3];

    unsigned int *elem=
        new unsigned int[nume];

    if(!fread((void *)vec,
        sizeof(float), numv*3, f)){
      printf("read vert-XYZ failed.\n");
      delete[] vec;
      delete[] tc;
      delete[] tang;
      delete[] bita;
      delete[] normal;
      delete[] elem;
      fclose(f);
      return 0;
    }

    //GL
    glBindBuffer(GL_ARRAY_BUFFER,bufs[1]);//8*3*4
    glBufferData(GL_ARRAY_BUFFER,numv*3*sizeof(float),vec,GL_STATIC_DRAW);
    glVertexAttribPointer(
					VEC_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,// normalized?
					0,// stride
					(void*)0// array buffer offset
				);

    //gl

    if(!fread((void *)tc,
        sizeof(float), numv*2, f)){//8*2*4
      printf("read texcoord failed.\n");
      delete[] vec;
      delete[] tc;
      delete[] tang;
      delete[] bita;
      delete[] normal;
      delete[] elem;
      fclose(f);
      return 0;
    }

    //GL
    glBindBuffer(GL_ARRAY_BUFFER,bufs[2]);
    glBufferData(GL_ARRAY_BUFFER,numv*2*sizeof(float),tc,GL_STATIC_DRAW);
    glVertexAttribPointer(
					TC_ATTRIB,
					2,// size
					GL_FLOAT,// type
					GL_FALSE,// normalized?
					0,// stride
					(void*)0// array buffer offset
				);

    //gl

    if(!fread((void *)normal,
        sizeof(unsigned int), numv*3, f)){
      printf("read normal failed.\n");
      delete[] vec;
      delete[] tc;
      delete[] tang;
      delete[] bita;
      delete[] normal;
      delete[] elem;
      fclose(f);
      return 0;
    }

    //GL
    glBindBuffer(GL_ARRAY_BUFFER,bufs[3]);
    glBufferData(GL_ARRAY_BUFFER,numv*3*sizeof(float),normal,GL_STATIC_DRAW);
    glVertexAttribPointer(
					NORM_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,// normalized?
					0,// stride
					(void*)0// array buffer offset
				);
    //gl

    if(!fread((void *)&bb[0],
        sizeof(unsigned int), 1, f)){
      printf("read check string failed.\n");
      delete[] vec;
      delete[] tc;
      delete[] tang;
      delete[] bita;
      delete[] normal;
      delete[] elem;
      fclose(f);
      return 0;
    }

    if(bb[0]!=0x4D454C45){//ELEM
      printf("check string wrong.\n");
      delete[] vec;
      delete[] tc;
      delete[] tang;
      delete[] bita;
      delete[] normal;
      delete[] elem;
      fclose(f);
      return 0;
    }

    if(!fread((void *)elem,
        sizeof(unsigned int), nume, f)){
      printf("read check string failed.\n");
      delete[] vec;
      delete[] tc;
      delete[] tang;
      delete[] bita;
      delete[] normal;
      delete[] elem;
      fclose(f);
      return 0;
    }

    //GL
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bufs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,nume*sizeof(unsigned int),elem,GL_STATIC_DRAW);
    //gl

    for(unsigned int i=0;i<nume/3;i++){
      float aa[9];
      /*int calcTBN(
float* v,float* uv, unsigned int* inds,
    float* t, float* b, float* n);*/
      calcTBN(vec,tc,elem+i*3,aa,aa+3,aa+6);
      norm(aa);
      norm(aa+3);
      for(int j=0;j<3;j++){
        tang[elem[i*3+j]*3+0]=aa[0];
        tang[elem[i*3+j]*3+1]=aa[1];
        tang[elem[i*3+j]*3+2]=aa[2];
      }

      for(int j=0;j<3;j++){
        bita[elem[i*3+j]*3+0]=aa[3];
        bita[elem[i*3+j]*3+1]=aa[4];
        bita[elem[i*3+j]*3+2]=aa[5];
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER,bufs[4]);
    glBufferData(GL_ARRAY_BUFFER,numv*3*sizeof(float),tang,GL_STATIC_DRAW);
    glVertexAttribPointer(
					TANG_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,// normalized?
					0,// stride
					(void*)0// array buffer offset
				);

    glBindBuffer(GL_ARRAY_BUFFER,bufs[5]);
    glBufferData(GL_ARRAY_BUFFER,numv*3*sizeof(float),bita,GL_STATIC_DRAW);
    glVertexAttribPointer(
					BITA_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,// normalized?
					0,// stride
					(void*)0// array buffer offset
				);
/*
    for(int i=0;i<16;i++)
      x->mm[i]=0;
    for(int i=0;i<4;i++)
      x->mm[i*5]=1;
    x->mm[0]=1;*/

    delete[] vec;
    delete[] tc;
    delete[] tang;
    delete[] bita;
    delete[] normal;
    delete[] elem;


  }else{
    printf("Not supported.\n");
    fclose(f);
    return 0;
  }
  fclose(f);
  return 1;
}
