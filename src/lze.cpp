#include <stdio.h>
#include <string.h>
#include <map>
#include "lze.h"

bool lzstrCmp(char* a,char* b) {
  int f=strcmp(a,b);
  return f<0;
} // returns x>y

typedef std::map<char*, ObjPack*,bool(*)(char*,char*)> maptype;
maptype usedobjs(lzstrCmp);

#define CHARSPERLINE 40
int loadLZE(const char* fn, ent* dad, entFactory* ff){
  char* fnn=new char[strlen(fn)+strlen(RES_ENT_DIR)+3];
  int os=0;
  strcpy(fnn+os, RES_ENT_DIR);
  os+=strlen(RES_ENT_DIR);
  strcpy(fnn+os, fn);
  FILE* f=fopen(fnn,"r");
  delete[] fnn;
  if(!f){
    printf("Failed to load entity %s\n", fnn);
    return 0;
  }
  fseek(f, 0, SEEK_SET);

  char* line=new char[CHARSPERLINE+1];
  char* buf1=new char[CHARSPERLINE+1];
  char* buf2=new char[CHARSPERLINE+1];
  int ij=0;
  ent* cur=dad;
  int zz=0;
  ent* tmp=0;
  while(true){
    if(!fgets(line, CHARSPERLINE, f)){
      delete[] line;
      delete[] buf1;
      delete[] buf2;
      if(feof(f))
        return 1;
      printf("Failed to read line.\n");
      return 0;
      fclose(f);
    }
    char asd;
    int nn=0;
    if(sscanf(line, " }%c",&asd)==1){
      cur=cur->getDad();
      zz--;
      if(zz<0){
        printf("ERROR!\n");
        return 0;
      }
    }else if(sscanf(line, " %s {%c",buf1,&asd)==2){
      zz++;
      //printf("XY: %s\n",buf1);
      cur=ff->createEntity(cur,buf1);
    }else if(sscanf(line, " %s \"%n%[^\"]s",buf1,&nn,buf2)==2){
      if(strcmp(buf1,"Model")==0){
        //printf("MODEL %s\n",buf2);
        char* szov=new char[strlen(buf2)+3+strlen(RES_OBJ_DIR)];
        int os=0;
        strcpy(szov+os, RES_OBJ_DIR);
        os+=strlen(RES_OBJ_DIR);
        strcpy(szov+os, buf2);
        maptype::iterator it=usedobjs.find(szov);

        if(it==usedobjs.end()){
          ObjPack* model=new ObjPack();
          if(!loadLZO(szov, model)){
            printf("Load1 LZO failed.\n");
            return 0;
          }
          ff->addCVisible(cur, 1, model);
          usedobjs[szov]=model;
        }else{
          ff->addCVisible(cur, 1, (*it).second);
          delete[] szov;
        }
      }else if(strcmp(buf1,"Pos")==0){
        float x,y,z;
        if(sscanf(&line[nn], "%f, %f, %f",&x,&y,&z)==3){
          ff->addCPos(cur,x,y,z);
        }else{
          printf("Wrong format.\n");
        }
      }else if(strcmp(buf1,"Rot")==0){
        float x,y,z;
        if(sscanf(&line[nn], "%f, %f, %f",&x,&y,&z)==3){
          ff->addCRot(cur,x,y,z);
        }else{
          printf("Wrong format.\n");
        }
      }else if(strcmp(buf1,"Scl")==0){
        float x,y,z;
        if(sscanf(&line[nn], "%f, %f, %f",&x,&y,&z)==3){
          ff->addCScl(cur,x,y,z);
        }else{
          printf("Wrong format.\n");
        }
      }else if(strcmp(buf1,"View")==0){
        int x;
        float y;
        if(sscanf(&line[nn], "%i, %f",&x,&y)==3){
          //CView* vv=ff->addCView(cur,x,y);
          printf("View added.\n");
        }else{
          printf("Wrong format.\n");
        }
      }else if(strcmp(buf1,"Mozg")==0){
        unsigned char a[4];
        if(sscanf(&line[nn], "%c%c%c%c", &a[0], &a[1], &a[2], &a[3])==4){
          unsigned int* b=(unsigned int*)a;
          ff->addCMozg(cur, *b);
        }
      }else if(strcmp(buf1,"ADDTO")==0){
        ent* vmi=cur->getEntByName(buf2);
        if(tmp){
          printf("ERROR!\n");
          return 0;
        }
        if(vmi){
          tmp=cur;
          cur=vmi;
        }else{
          printf("ERROR! #3\n");
          return 0;
        }
      }else if(strcmp(buf1,"BACK")==0){
        cur=tmp;
        tmp=0;
      }else if(strcmp(buf1,"IMPORT")==0){
        if(!loadLZE(buf2, cur, ff)){
          printf("IMPORT failed.\n");
          return 0;
        }
      }
    }else if(sscanf(line, "//%c",&asd)==1){
    }
    ij++;
  }
  if(zz){
    printf("ERROR! #2\n");
    return 0;
  }
  delete[] line;

  fclose(f);
  return 1;
}
