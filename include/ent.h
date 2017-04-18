#ifndef ENT_H
#define ENT_H

#include <string.h>
#include "lzo.h"
class ent;
class PComp;
class System;

#ifndef MAX_ENTS
#define MAX_ENTS 10000
#endif

extern ent ents[MAX_ENTS];

struct Component{
public:
  const int ID;
  Component(int x) : ID(x){
  }
};

typedef struct PComp{
  Component* ez;
  PComp* az;
  PComp(Component* x){ez=x;az=0;}
  PComp(Component* x, PComp* a)
    {ez=x;az=a;}
  ~PComp(){
    delete ez; delete az;
  }
} PComp;

#define CPOS_ID 0
#define CROT_ID 1
#define CSCL_ID 2
#define CVIS_ID 4
#define CVIEW_ID 5
#define CMOZG_ID 6
#define CNAME_ID 3

#define CARRAY_LENGTH 7

struct CPos: Component{
  float x,y,z;
  CPos(float xx, float yy, float zz):
      Component(CPOS_ID){
    x=xx;y=yy;z=zz;
  }
};

struct CName: Component{
  char* name;
  CName(const char* namex):
      Component(CNAME_ID){
    name=new char[strlen(namex)+1];
    strcpy(name,namex);
  }
  ~CName(){
    delete[] name;
  }
};

struct CRot: Component{
  float x,y,z;
  CRot(float xx, float yy, float zz):
      Component(CROT_ID){
    x=xx;y=yy;z=zz;
  }
};

struct CScl: Component{
  float x,y,z;
  CScl(float xx, float yy, float zz):
      Component(CSCL_ID){
    x=xx;y=yy;z=zz;
  }
};

struct CVisible: Component{
  int v;
  ObjPack* model;
  CVisible(int p, ObjPack* m):
      Component(CVIS_ID){
    v=p;
    model=m;
  }
};

struct CView: Component{
  float fov;
  int idd;
  CView(int id, float xx):
      Component(CVIEW_ID){
    fov=xx;
    idd=id;
  }
};

struct CMozg: Component{
  unsigned int chr;
  CMozg(unsigned int x):
      Component(CMOZG_ID){
    chr=x;
  }
};

//CPos, CRot, CScl
//(CHealth, CHud)
//CVisible

class Comps{
  Component* carray[CARRAY_LENGTH];
public:
  class iterator{
    Comps* x;
    int ID;
    iterator(Comps* a, int id){
      x=a;
      ID=id;
    }
  public:
    Component* operator*(){
      return x->carray[ID];
    }
    void operator++(){
      do{
        ID++;
      }while(!x->carray[ID]&&ID<CARRAY_LENGTH);
    }
    bool operator==(iterator it2){
      if(x!=it2.x)return 0;
      return ID==it2.ID;
    }
    bool operator!=(iterator it2){
      if(x!=it2.x)return 1;
      return ID!=it2.ID;
    }
    friend class Comps;
  };
  Component* add(Component* a);
  void rem(int x);
  Component* get(int id);
  iterator begin(){
    int e=0;
    while(e<CARRAY_LENGTH&&!carray[e]){
      e++;
    }
    return iterator(this, e);
  }
  iterator end(){
    return iterator(this, CARRAY_LENGTH);
  }
};

class ent{
  int dad, son, bro, id;
  Comps* a;
public:
  class iterator{
    ent* xy;
    iterator(ent* f){
      xy=f;
    }
  public:
    ent& operator*(){
      return *xy;
    }
    void operator++(){//++it
      if(xy->bro)
        xy=&ents[xy->bro];
      else
        xy=0;
    }
    bool operator==(iterator it2){
      return xy==it2.xy;
    }
    bool operator!=(iterator it2){
      return xy!=it2.xy;
    }
    friend class ent;
  };

  iterator begin(){
    if(!son)return 0;
    iterator it(&ents[son]);
    return it;
  }

  iterator end(){
    return 0;
  }

  Comps* getComps(){
    return a;
  }

  bool startsWith(const char* x, const char* n){
    int i=0;
    while(n[i]){
      if(!x[i])return false;//x len<n len
      if(n[i]!=x[i])return false;
      i++;
    }
    return true;
  }

  ent* getEntByName(const char*);
  ent* getDad(){return &ents[dad];}
  Component* getComponent(int ID);
  void addComponent(Component*);
  void remComponent(int id);
  friend class entManager;
};

class entManager{
  int xID;
//  int genID();
//  void remEntity(int);
public:
  ent*  createEntity(ent*, char* name);
  void destroyEntity(ent*);
  entManager();
  ~entManager();
};

class entFactory{
  entManager m;
public:

  entFactory();
  ~entFactory();

  ent*  createEntity(ent*, char*);
  void addCPos//modify IS!
      (ent*, float, float, float);
  void remCPos(ent*);

  void addCRot//modify IS!
      (ent*, float, float, float);
  void remCRot(ent*);

  CView* addCView//modify IS!
      (ent*, int, float);
  void remCView(ent*);

  void addCMozg//modify IS!
      (ent*, unsigned int);
  void remCMozg(ent*);

  void addCVisible//modify IS!
      (ent*, int, ObjPack* model);
  void remCVisible(ent*);

  void addCScl//modify IS!
      (ent*, float, float, float);
  void remCScl(ent*);

  /*void loadEnt(const char* fn);
  void saveEnt(const char* fn);*/
  //friend class System;
};

class System{

  void kirComps(ent&);
  void feld(float dt, ent&);
  void render(ent&, float*);
public:
  void feldFa(float dt, ent&);
  void renderFa(ent&, float*);
  void renderAll();
  entFactory f;
  void kiirFa(ent&);
  System();
  ~System();
  void tick(int dt);
};

int getDad(int);

#endif
