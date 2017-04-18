#include "mymath.h"
#include "lzo.h"
#include "ent.h"
#include <stdio.h>

void System::feld(float, ent&){}
void System::render(ent&, float*){}

int x01(){
  //ObjPack x;
  //createTestFile("../tesst");
  //System s;
  //loadLZE("../kocka.lze", 1, &(s.f)); modelt nem tolt be, mert nincs OpenGL inicializalva
  //s.kiirFa(1);
  float ered[16];
  /*float a[6]=
  {
    1.f,
    0.f,
    0.f,
    0.f,
    0.f,
    0.f,
  };*/
  float a[6]=
    {
      0.2f,
      0.3f,
      0.5f,
      0.8f,
      0.17f,
      0.9f};
  {
    float mn[16];
    createMatr(a, mn);

    float mi[16];
    createMatrInv(a, mi);

    mxm(mi,mn,ered);
  }
  for(int i=0;i<16;i++){
    if(i%4==0){
      printf("\n");
    }
    printf("%f\t", ered[i]);
  }
  //loadLZO("../hello",&x);
  return 0;
}

void tomgyar(const char* fn){
//b {
//  Pos "4,0,0"
//  Scl "2,2,2"
//  IMPORT "../ember.lze"
//}
  FILE* f=fopen(fn,"w");
  for(int i=0;i<1000;i++){
    float a,b,c;
    a=i%10+1;
    b=i/100+1;
    c=i/10%10+1;
    a*=3;
    b*=3;
    c*=3;
    fprintf(f, "hullajelolt%03d {\n",i);
    fprintf(f, "  Pos \"%f, %f, %f\"\n",a,b,c);
    fprintf(f, "  IMPORT \"ember.lze\"\n");
    fprintf(f, "}\n");
  }
  fclose(f);
}

int main(){
  //ObjtoLZO("../monkey.obj","../monkey.lzo");
  tomgyar(RES_ENT_DIR "map02.lze");
  return 0;
}
