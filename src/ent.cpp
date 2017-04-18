#include <stdio.h>
#include "ent.h"
#include "lzo.h"
#include "mymath.h"

ent ents[MAX_ENTS];

void Comps::rem(int x){
  if(x<CARRAY_LENGTH){
    if(carray[x]){
      delete carray[x];
      carray[x]=0;
    }
  }else{
    printf("(C-REM)ERROR!\n");
  }
}

Component* Comps::add(Component* a){
  int id=a->ID;
  if(id<CARRAY_LENGTH){
    if(carray[id]){
      rem(id);
    }
    carray[id]=a;
  }else{
    printf("ERROR!\n");
    return 0;
  }
  return a;
}

Component* Comps::get(int id){
  if(id<CARRAY_LENGTH)
    return carray[id];
  printf("(C-GET) ERROR!\n");
  return 0;
}

entManager::entManager(){
  xID=2;
  for(int i=0;i<MAX_ENTS;i++){
    ents[i].dad=0;
    ents[i].son=0;
    ents[i].bro=0;
    ents[i].id=i;
  }
  ents[1].id=1;
  ents[1].a=new Comps();
  ents[1].a->add(new CName("world"));
  ents[1].dad=1;
  ents[1].bro=0;
  ents[1].son=0;
}

entManager::~entManager(){
  destroyEntity(&ents[1]);
}

ent* entManager::createEntity(ent* e,char* name){
  while(xID<MAX_ENTS){
    if(ents[xID].dad==0)break;
  }
  if(xID>=MAX_ENTS){
    printf("(EM-CE) failed.\n");
    return 0;
  }

  ents[xID].dad=e->id;
  int x=e->son;
  e->son=xID;
  ents[xID].bro=x;
  ents[xID].son=0;
  ents[xID].a=new Comps();
  CName* ab=new CName(name);
  ents[xID].addComponent(ab);
  return &ents[xID++];
}

void ent::addComponent(
    Component* x){
  a->add(x);
}

void ent::remComponent(
    int ID){
  a->rem(ID);
}

void entManager::destroyEntity(ent* e){
  if(!e)return;
  if(e->son!=0){
    ent::iterator it=e->begin(), end=e->end();
    //int y=e->bro;
    while(it!=end){
      destroyEntity(&*it);
      ++it;
    }
  }
  if(xID>e->id)xID=e->id;
  e->dad=e->son=e->bro=0;
}

entFactory::entFactory(){

}

entFactory::~entFactory(){
  m.destroyEntity(&ents[1]);
}

ent* entFactory::createEntity(ent* e,char* name){
  return m.createEntity(e, name);
}

void entFactory::addCPos
    (ent* e, float x,
    float y, float z){
  e->addComponent(new CPos(x,y,z));
}

void entFactory::remCPos(ent* e){
  e->remComponent(CPOS_ID);
}

CView* entFactory::addCView
    (ent* e, int id, float x){
  CView* ered=new CView(id, x);
  e->addComponent(ered);
  return ered;
}

void entFactory::remCView(ent* e){
  e->remComponent(CVIEW_ID);
}

void entFactory::addCRot
    (ent* e, float x,
    float y, float z){
  e->addComponent(new CRot(x,y,z));
}

void entFactory::remCRot(ent* e){
  e->remComponent(CROT_ID);
}

void entFactory::addCVisible
    (ent* e, int a, ObjPack* model){
  e->addComponent(new CVisible(a,model));
}

void entFactory::remCVisible(ent* e){
  e->remComponent(CVIS_ID);
}

void entFactory::addCMozg
    (ent* e, unsigned int a){
  e->addComponent(new CMozg(a));
}

void entFactory::remCMozg(ent* e){
  e->remComponent(CMOZG_ID);
}

void entFactory::addCScl
    (ent* e, float x,
    float y, float z){
  e->addComponent(new CScl(x,y,z));
}

void entFactory::remCScl(ent* e){
  e->remComponent(CSCL_ID);
}

void System::kirComps(ent& x){
  CName* a=(CName*)x.getComps()->get(CNAME_ID);
  if(a)
    printf("  %s", a->name);
}

void System::kiirFa(ent& ee){
  printf("+");
  kirComps(ee);
  ent::iterator end=ee.end();
  printf("\n");
  for(ent::iterator it=ee.begin();it!=end;++it){
    kiirFa(*it);
  }
//  int y=ents[e].getFSon();
//  while(y!=0){
//    kiirFa(y);
//    y=ents[y].getBro();
//  }
  printf("-\n");
}

void System::feldFa(float dt, ent& e){
//  int s=ents[e].getFSon();
//  feld(e);
//  if(s){
//    while(s!=0){
//      feldFa(s);
//      s=ents[s].getBro();
//    }
//  }

  feld(dt, e);
  ent::iterator end=e.end();
  for(ent::iterator it=e.begin();it!=end;++it){
    feldFa(dt, *it);
  }
}

System::System(){
/*  int x=f.createEntity(1);
  f.addCPos(x,1,0,0);*/
}

ent* ent::getEntByName(const char* x){
  iterator it=begin();
  iterator e=end();
  while(it!=e){
    CName* name=(CName*)((*it).a->get(CNAME_ID));
    if(name){
      if(startsWith(x,name->name)){
        int len=strlen(name->name);
        if(x[len]=='.')
          return (*it).getEntByName(&(x[len+1]));
        if(!x[len])
          return &(*it);
      }
    }
    ++it;
  }
  return 0;
}

System::~System(){
}

#ifdef LZTEST
void System::render(ent*, float*){}
#endif

void System::renderFa(ent& e, float* m){
  float mm[16];
  {
    float tmp[16];
    float vmi[9];
    CPos* p=(CPos*)e.getComponent(CPOS_ID);
    CRot* r=(CRot*)e.getComponent(CROT_ID);
    CScl* ss=(CScl*)e.getComponent(CSCL_ID);
    vmi[0]=vmi[1]=vmi[2]=0;
    if(p){
      vmi[0]=p->x;
      vmi[1]=p->y;
      vmi[2]=p->z;
    }
    vmi[3]=vmi[4]=vmi[5]=0;
    if(r){
      vmi[3]=r->x;
      vmi[4]=r->y;
      vmi[5]=r->z;
    }
    vmi[6]=vmi[7]=vmi[8]=1;
    if(ss){
      vmi[6]=ss->x;
      vmi[7]=ss->y;
      vmi[8]=ss->z;
    }

    createMatr(vmi,tmp);

    mxm(m, tmp, mm);

  }
  render(e,mm);//mm!!


  ent::iterator ee=e.end();
  for(ent::iterator it=e.begin();it!=ee;++it){
    renderFa(*it, mm);
  }
//  int f=s->getFSon();
//  s=&ents[f];
//  while(f){
//    renderFa(f, mm);//mm!!
//    f=s->getBro();
//    s=&ents[f];
//  }

}

void System::renderAll(){
  float mm[16];
  for(int i=0;i<16;i++){
    mm[i]=0;
    if(i%5==0)mm[i]=1;
  }
  renderFa(ents[1],mm);
}

Component* ent::getComponent(int ID){
  return a->get(ID);
}

