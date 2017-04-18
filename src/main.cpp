//#define GLUT_DISABLE_ATEXIT_HACK

int AmdPowerXpressRequestHighPerformance = 1;

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "mymath.h"
#include "lzi.h"
#include "lzo.h"
#include "lze.h"

#include <cstdio>
#include <sstream>
#include <fstream>
#include <string.h>

#define SHADER_FILE_SIZE 2000

using namespace std;
System s;

void kiirObj(ObjPack x){
  printf("Kiiring...\n");
	int i;
  printf("VAO: %u\n",x.vao);
	printf("\nMode: %d\n",x.mode);
	printf("    (TRI: %d)\n",GL_TRIANGLES);
	printf("    (QUAD: %d)\n",GL_QUADS);
	printf("\nType: %d\n",x.type);
	printf("    (UINT: %d)\n",GL_UNSIGNED_INT);
	printf("    (INT: %d)\n",GL_INT);
	printf("    (FLOAT: %d)\n",GL_FLOAT);
	printf("VAAs: 01234567890123456789\n      ");
	for(i=0;i<MAX_VAAC;i++){
		if(x.vaas[i])
			printf("1");
		else
			printf("0");
	}
	printf("\nNumber of vertices: %d\n",x.pc);
	printf("Program's ID: %d\n",x.prog);
  printf("Kiired.\n");
}

int initObjs(){
  return 1;
}

GLuint globprog;
//4x4-es matrixok
float dirl[3]={0,-1,-0.6};
GLuint lPM,lMM,lVM,
lTexture,lNormMap,lUseTex,
lUseNorm,lDirLight;//unifornisok (unifornis: egyszarvu oltony)
int gon;//globalobjects' N

void getError(int x){
	int ecode=glGetError();
	if(ecode!=0){
		printf("ERROR #%d\n",x);
		exit(1);
	}
}

void drawObj(ObjPack o, float* mdlmtrx){

  if(o.prog!=0)
    glUseProgram(o.prog);
/*  else
    glUseProgram(globprog);*/

  if(o.vaas[NORM_ATTRIB])
    glUniform1i(lUseNorm,1);
  else
    glUniform1i(lUseNorm,0);

  if(o.vaas[TC_ATTRIB])
    glUniform1i(lUseTex,1);
  else
    glUniform1i(lUseTex,0);

	int i;
	glBindVertexArray(o.vao);
	{
    glUniformMatrix4fv(lMM,1,GL_TRUE,mdlmtrx);
	}

	for(i=0;i<MAX_VAAC;i++)
		if(o.vaas[i])
			glEnableVertexAttribArray(i);

	glDrawElements(o.mode, o.pc, o.type, 0);

	for(i=0;i<MAX_VAAC;i++)
		if(o.vaas[i])
			glDisableVertexAttribArray(i);

	glBindVertexArray(0);
	getError(84);
}

char staticbuffer[SHADER_FILE_SIZE];
GLfloat campoz[9];//poz[xyz], rot[pyr]
bool keys[256];

void System::feld(float, ent& xy){

  CPos* p=(CPos*)xy.getComponent(CPOS_ID);
  CRot* r=(CRot*)xy.getComponent(CROT_ID);
  CMozg* m=(CMozg*)xy.getComponent(CMOZG_ID);
  CName* n=(CName*)xy.getComponent(CNAME_ID);
  if(n&&m){
    if(!strcmp(n->name,"fej")&&r){
      unsigned char* xyy=(unsigned char*)(&(m->chr));
      for(int i=0;i<4;i++){
        unsigned char ch=xyy[i];
        if(keys[ch]){
          int iy=(1-i%2)*(1-i/2*2);
          int ix=i%2*(1-i/2*2);
          r->y+=ix*0.05f;
          r->z+=iy*0.05f;
        }
      }
    }
    if(!strcmp(n->name,"embor")&&p){
      unsigned char* xyy=(unsigned char*)(&(m->chr));
      if(keys[xyy[0]]||keys[xyy[2]]){
/*        ent* lab1=xy.getEntByName("test.lab1");
        ent* lab2=xy.getEntByName("test.lab2");
        ent* kez1=xy.getEntByName("test.kez1");
        ent* kez2=xy.getEntByName("test.kez2");
        CRot* r1=(CRot*)lab1->getComponent(CROT_ID);
        CRot* r2=(CRot*)lab2->getComponent(CROT_ID);
        CRot* rk1=(CRot*)kez1->getComponent(CROT_ID);
        CRot* rk2=(CRot*)kez2->getComponent(CROT_ID);
        #define MAR_MAR 20
        #define ROT 0.6*/
        #define SEB 0.2
        if(keys[xyy[0]]){
/*          xyy[1]+=1;
          xyy[1]%=MAR_MAR;
          float aaa=xyy[1]*6.2831852/MAR_MAR;
          r1->z=sin(aaa)*ROT;
          r2->z=sin(-aaa)*ROT;
          rk1->z=sin(-aaa)*ROT;
          rk2->z=sin(aaa)*ROT;*/
          p->x-=SEB;
        }
        if(keys[xyy[2]]){
/*          float aaa=xyy[1]*6.2831852/MAR_MAR;
          xyy[1]+=MAR_MAR-1;
          xyy[1]%=MAR_MAR;
          r1->z=sin(aaa)*ROT;
          r2->z=sin(-aaa)*ROT;
          rk1->z=sin(-aaa)*ROT;
          rk2->z=sin(aaa)*ROT;*/
          p->x+=SEB;
        }
      }

    }
  }
}

void setViewMatr(float* pozrot){
	float d[16];
	createMatrInv(pozrot,d);
	glUniformMatrix4fv(lVM,1,GL_TRUE,d);
}

void setProjMatr(float fovv, float aspp, float nearr, float farr){
	float d[16];
	d [0]=fovv/aspp;
	d [1]=0;
	d [2]=0;
	d [3]=0;

	d [4]=0;
	d [5]=fovv;
	d [6]=0;
	d [7]=0;

	d [8]=0;
	d [9]=0;
	d[10]=(farr+nearr)/(nearr-farr);
	d[11]=2*farr*nearr/(nearr-farr);

	d[12]=0;
	d[13]=0;
	d[14]=-1;
	d[15]=0;
	glUniformMatrix4fv(lPM,1,GL_TRUE,d);
}

GLuint vao=0;

int loadFile(const char* fname,char* buffer){
	int length;
	ifstream t(fname,(ios::binary | ios::in));
	if(!t)
	{
		printf("ERROR! Unable to open file (%s)\n",fname);
		return 0;
	}

	t.seekg(0, ios::end);
	length = t.tellg();
	t.seekg(0, ios::beg);
	if(!t.read(buffer, length)){
	  printf("ERROR: Unable to read from file (%s)\n",fname);
	  printf("Bits: b%d f%d e%d g%d\n",
              t.bad(),t.fail(),t.eof(),t.good());
	  return 0;
	}
	t.close();
	buffer[length]=0;
	return 1;
}

void printShaderLog(GLint shader){
	int infoln = 0;
	int chrs = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoln);
	if (infoln > 1)
		{
		infoLog = new GLchar[infoln];

		glGetShaderInfoLog(shader, infoln, &chrs, infoLog);
		printf("InfoLog:\n%s\n",infoLog);
		delete [] infoLog;
	}
}
#ifndef GL_TESS_CONTROL_SHADER
#define GL_TESS_CONTROL_SHADER -1
#endif
#ifndef GL_TESS_EVALUATION_SHADER
#define GL_TESS_EVALUATION_SHADER -1
#endif
int loadProgram(const char** fnames, int fnamec, GLuint& e){
	GLint ell;
	char* buffer=staticbuffer;
	GLuint shs[fnamec];
	int i;
	for(i=0;i<fnamec;i++){
		if(fnames[i]==0)continue;
		int shaderType;
		switch(i){
			case 0:{
				shaderType=GL_VERTEX_SHADER;
				break;
			}
			case 1:{
				shaderType=GL_FRAGMENT_SHADER;
				break;
			}
			case 2:{
				shaderType=GL_GEOMETRY_SHADER;
				break;
			}
			case 3:{
				shaderType=GL_TESS_CONTROL_SHADER;
				break;
			}
			case 4:{
				shaderType=GL_TESS_EVALUATION_SHADER;
				break;
			}
		};
		if(!loadFile(fnames[i],buffer)){
			printf("ERROR! Failed to load file (%s)\n",fnames[i]);
			return -1;
		}
//		printf("shader file: (%d)\n%s",i,buffer);

		shs[i]=glCreateShader(shaderType);
		glShaderSource (shs[i], 1, (const GLchar**)&buffer, NULL);
		glCompileShader(shs[i]);
		glGetShaderiv(shs[i],GL_COMPILE_STATUS,&ell);
		if(!ell){
			printf("ERROR! Failed to compile the %d. shader\n",i+1);
			printShaderLog(shs[i]);
			glDeleteShader(shs[i]);
			return -1;
		}
	}

	e=glCreateProgram();
	for(i=0;i<fnamec;i++){
		if(fnames[i]==0)continue;
		glAttachShader(e,shs[i]);
	}
	glLinkProgram(e);
	glGetProgramiv(e,GL_LINK_STATUS,&ell);
	if(!ell){
		printf("ERROR! Failed to link shader\n");

		GLint length;
		glGetProgramiv(e,GL_INFO_LOG_LENGTH,&length);
		if(length>1){
			char* infolog=new char[length];
			glGetProgramInfoLog(e,length,&length,infolog);
			printf("%s\n",infolog);
			delete[] infolog;
		}
		for(i=0;i<fnamec;i++){
			if(fnames[i]==0)continue;
			glDetachShader(e,shs[i]);
			glDeleteShader(shs[i]);
		}
		return -1;
	}
	for(i=0;i<fnamec;i++){
		if(fnames[i]==0)continue;
		glDetachShader(e,shs[i]);
		glDeleteShader(shs[i]);
	}
	return 1;
}

void myExit(){
	//a tarhely felszabaditasa
}

void display(){

	getError(10);

  glUseProgram(globprog);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

/*	glBindVertexArray(vao);
	glUniformMatrix4fv(lMM,1,GL_TRUE,d);

	glEnableVertexAttribArray(VEC_ATTRIB);
	glEnableVertexAttribArray(TC_ATTRIB);
	glEnableVertexAttribArray(TANG_ATTRIB);
	glEnableVertexAttribArray(BITA_ATTRIB);
	glEnableVertexAttribArray(NORM_ATTRIB);
	//glUniform1i(lUseTex,1);
	//glUniform1i(lUseNorm,1);
	glUniform1i(lUseNorm,1);
	glUniform1i(lUseTex,1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glUniform1i(lUseNorm,0);
	glUniform1i(lUseTex,0);
	glDisableVertexAttribArray(NORM_ATTRIB);
	glDisableVertexAttribArray(BITA_ATTRIB);
	glDisableVertexAttribArray(TANG_ATTRIB);
	glDisableVertexAttribArray(TC_ATTRIB);
	glDisableVertexAttribArray(VEC_ATTRIB);*/

	glBindVertexArray(0);

	getError(8);

	getError(41);

   s.renderAll();

	getError(98);
	glutSwapBuffers();
}

void mozog(float dt){
	float d[16];
	createMatr(campoz,d);
	s.feldFa(dt, ents[1]);
	if(keys['w']&1){
		float a=campoz[4]+PI_D2;
		float ca=cos(a);
		float sa=sin(a);
		campoz[0]-=ca*10*dt;
		campoz[2]-=sa*10*dt;
	}else if(keys['s']&1){
		float a=campoz[4]+PI_D2;
		float ca=cos(a);
		float sa=sin(a);
		campoz[0]+=ca*10*dt;
		campoz[2]+=sa*10*dt;
	}
	if(keys['a']&1){
		float a=campoz[4];
		float ca=cos(a);
		float sa=sin(a);
		campoz[0]-=ca*10*dt;
		campoz[2]-=sa*10*dt;
	}else if(keys['d']&1){
		float a=campoz[4];
		float ca=cos(a);
		float sa=sin(a);
		campoz[0]+=ca*10*dt;
		campoz[2]+=sa*10*dt;
	}
	if(keys['i']&1){
    dirl[1]+=100*dt;
	}else if(keys['k']&1){
    dirl[1]-=100*dt;
	}
	if(keys['j']&1){
    dirl[0]+=100*dt;
	}else if(keys['l']&1){
    dirl[0]-=100*dt;
	}
	if(keys['z']&1){
    dirl[2]+=100*dt;
	}else if(keys['h']&1){
    dirl[2]-=100*dt;
	}
  getError(49);
  glUniform3fv(lDirLight,1,dirl);
  getError(48);

	if(keys['r']&1){
		campoz[1]+=10*dt;
	}else if(keys['f']&1){
		campoz[1]-=10*dt;
	}
	setViewMatr(campoz);
	glutPostRedisplay();
}

#define ETA_MAX 30
int lt=0;
void idle(){
	int t=glutGet(GLUT_ELAPSED_TIME);
	if(t-lt>ETA_MAX){
		mozog((t-lt)/1000.0);
		lt=t;
	}
}

#define CAM_FOV 1.f
#define CAM_NEAR 0.01f
#define CAM_FAR 1000.f

void resize(int x, int y){
	setProjMatr(CAM_FOV, ((float)x)/y, CAM_NEAR, CAM_FAR);
	glViewport(0,0,x,y);
}


int lx,ly;
void mouse(int, int, int x, int y){
	lx=x;
	ly=y;
}

#define ROT_RATE 0.01f
void mouseDrag(int x, int y){
	float dx=(float)(x-lx),dy=(float)(y-ly);
	campoz[3]-=dy*0.01f;
	if(campoz[3]>PI_D2)campoz[3]=PI_D2;
	if(campoz[3]<-PI_D2)campoz[3]=-PI_D2;
	campoz[4]+=dx*0.01f;
	setViewMatr(campoz);
	lx=x;
	ly=y;
}

void keyboardUp(unsigned char ch, int, int){
	keys[ch]=0;
}

void keyboard(unsigned char ch, int, int){
	if(ch=='q'){
		printf("Good bye!\n");
		myExit();
		glutLeaveMainLoop();
	}
	keys[ch]=1;
}

float vert[12]={//vagy forditva!!
-1,-1,-0.5,
-1,+1,-0.5,
+1,+1,-0.5,
+1,-1,-0.5
};

/*
1 1 0
1 0 0

0 0 -1
*/

// (0,1,0)x
//x(1,1,0)= (0,

float col[16]={
1,0.5,0,0.5,
1,  0,1,0.5,
1,  1,0,0.5,
0,  1,1,0.5
};

float tc[8]={
  1.f,0.f,
  1.f,1.f,
  0.f,1.f,
  0.f,0.f,
};

unsigned int elems[6]={
	2,0,1,
	0,2,3,
};

void initBuffers(){
	GLuint bufs[7];
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	glGenBuffers(7,bufs);

	glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glVertexAttribPointer(
					VEC_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,// normalized?
					0,// stride
					(void*)0// array buffer offset
				);

	glBindBuffer(GL_ARRAY_BUFFER, bufs[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tc), tc, GL_STATIC_DRAW);
	glVertexAttribPointer(
					TC_ATTRIB,
					2,// size
					GL_FLOAT,// type
					GL_FALSE,
					0,// stride
					(void*)0// array buffer offset
				);

//v[ind[0-2]*3+(0-2)]: vertices
//uv[ind[0-2]*2+(0-1)]: UV-coords
//ind[0-2]: indices
//t,b,n [0-2]: tangent, bitangent, normal
//int calcTBN(
//float* v,float* uv, float* inds, float* t, float* b, float* n){
  int NTBLENGTH=4*3*sizeof(float);
  float* n=new float[4*3];
  float* t=new float[4*3];
  float* b=new float[4*3];

  calcTBN(vert,tc,elems,t,b,n);
  norm(n);
  norm(t);
  norm(b);
  for(int i=1;i<4;i++){
    t[i*3+0]=t[0];
    t[i*3+1]=t[1];
    t[i*3+2]=t[2];

    b[i*3+0]=b[0];
    b[i*3+1]=b[1];
    b[i*3+2]=b[2];

    n[i*3+0]=n[0];
    n[i*3+1]=n[1];
    n[i*3+2]=n[2];
  }

	glBindBuffer(GL_ARRAY_BUFFER, bufs[4]);
	glBufferData(GL_ARRAY_BUFFER, NTBLENGTH, t, GL_STATIC_DRAW);
	glVertexAttribPointer(
					TANG_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,
					0,// stride
					(void*)0// array buffer offset
				);

	glBindBuffer(GL_ARRAY_BUFFER, bufs[5]);
	glBufferData(GL_ARRAY_BUFFER, NTBLENGTH, b, GL_STATIC_DRAW);
	glVertexAttribPointer(
					BITA_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,
					0,// stride
					(void*)0// array buffer offset
				);

	glBindBuffer(GL_ARRAY_BUFFER, bufs[6]);
	glBufferData(GL_ARRAY_BUFFER, NTBLENGTH, n, GL_STATIC_DRAW);
	glVertexAttribPointer(
					NORM_ATTRIB,
					3,// size
					GL_FLOAT,// type
					GL_FALSE,
					0,// stride
					(void*)0// array buffer offset
				);

  delete[] t;
  delete[] b;
  delete[] n;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void initShaders(){
	char fname[100];
	char* fns[5];
	int os=0;
	int i=0;
	strcpy(&fname[os],RES_SHDR_DIR VERT_PATH);
	fns[i++]=fname+os;
	os+=strlen(RES_SHDR_DIR VERT_PATH)+1;
	strcpy(&fname[os],RES_SHDR_DIR FRAG_PATH);
	fns[i++]=fname+os;
	os+=strlen(RES_SHDR_DIR FRAG_PATH)+1;
/*#ifdef GEOMETRY_PATH
	strcpy(&fname[os],GEOMETRY_PATH);
	fns[i++]=fname+os;
	os+=strlen(GEOMETRY_PATH)+1;
#else
	fns[i++]=0;
#endif*/

/*#ifdef TC_PATH
	strcpy(fname+os,TC_PATH);
	fns[i++]=fname+os;
	os+=strlen(TC_PATH)+1;
#else
	fns[i++]=0;
#endif*/

/*#ifdef TE_PATH
	strcpy(fname+os,TE_PATH);
	fns[i++]=fname+os;
	os+=strlen(TE_PATH)+1;
#else
	fns[i++]=0;
#endif*/

	int err=loadProgram((const char**)fns,i,globprog);
	if(err==-1){
		printf("ERROR! Failed to init shaders\n");
#ifdef _MSC_VER
		char x[8];
		scanf("%s\n",x);
#endif
		exit(1);
	}
	glUseProgram(globprog);
}

void initUnifs(){
	lPM=glGetUniformLocation(globprog,"projMatr");
	lVM=glGetUniformLocation(globprog,"viewMatr");
	lMM=glGetUniformLocation(globprog,"modlMatr");
	lTexture=glGetUniformLocation(globprog,"sampler");
	lNormMap=glGetUniformLocation(globprog,"sampler2");
	lUseTex=glGetUniformLocation(globprog,"useTex");
	lUseNorm=glGetUniformLocation(globprog,"useNorm");
	lDirLight=glGetUniformLocation(globprog,"dirlight");
}

void initOther(){
	lt=glutGet(GLUT_ELAPSED_TIME);
	for(int i=0;i<6;i++)campoz[i]=0;
	for(int i=6;i<9;i++)campoz[i]=1;
  gon=1;

//vec3(0,-1,-0.1)
  glUniform3fv(lDirLight,1,dirl);
  glUniform1i(lUseTex,0);
  glUniform1i(lUseNorm,0);
}

//#ifndef TEX_BUFFER_SIZE
#define TEX_BUFFER_SIZE 1024*1048*3
//#endif
void initTextures(){
  GLuint texs[2];
  glActiveTexture(GL_TEXTURE0);

  glGenTextures(2,texs);
  glBindTexture(GL_TEXTURE_2D,texs[0]);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  unsigned int w=0,h=0;
  unsigned char bpp=0;
  char* texturebuffer=new char[TEX_BUFFER_SIZE];
  if(!texturebuffer){
    printf("NULL POINTER/n");
    exit(1);
  }
  if(loadLZI(RES_IMG_DIR TEX01_PATH, texturebuffer, (unsigned long)TEX_BUFFER_SIZE, w, h, bpp)){
    printf("Failed to load %s\n",RES_IMG_DIR TEX01_PATH);
    exit(1);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, texturebuffer);

/*  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/

  glUniform1i(lTexture,0);
  //glFlush();

  glActiveTexture(GL_TEXTURE0+1);

  glBindTexture(GL_TEXTURE_2D,texs[1]);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  w=0;h=0;
  bpp=0;
  /*if(loadLZI(RES_IMG_DIR TEX02_PATH, texturebuffer, (unsigned int)TEX_BUFFER_SIZE, w, h, bpp)){
    printf("Failed to load %s\n", RES_IMG_DIR TEX02_PATH);
    exit(1);
//    BMPtoLZI(
//int BMPtoLZI(const char* bmp, const char* lzi, int width, int height, int bmpoffset, unsigned char bmpbpp, unsigned char lzibpp);
  }*/
  w=h=1;
  bpp=3;
  texturebuffer[0]=255;
  texturebuffer[1]=127;
  texturebuffer[2]=127;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, texturebuffer);

  glUniform1i(lNormMap,1);
  glFlush();

  getError(83);
  delete[] texturebuffer;
}

int prog0(int argc,char** argv){
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE );

  glutInitContextVersion(4, 3);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	glutCreateWindow("LATZ3D");

	getError(6);

	printf("OpenGL driver version: %s\n", glGetString(GL_VERSION));

	getError(3);

	glewExperimental=GL_TRUE;
	GLenum err=glewInit();
	if(err!=GLEW_OK){
		printf("ERROR! Failed to initialize GLEW\n");
		exit(1);
	}
	glGetError();
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutMotionFunc(mouseDrag);
	glutMouseFunc(mouse);

	initBuffers();
	initShaders();
	initUnifs();
	initTextures();
	initObjs();

  loadLZE("map02.lze", &ents[1], &(s.f));
  printf("Searching...\n");
  //s.kiirFa(ents[1]);
/*  ent* aa=ents[1].getEntByName("x");
  if(aa)
    s.kiirFa(*aa);
  else{
    printf("Not found.\n");
  }*/

	initOther();

	getError(1);


	glutMainLoop();
	return 0;
}

void System::render(ent& e, float* pm){//parent's matrix
  CVisible* a=(CVisible*)e.getComponent(CVIS_ID);
  if(a)
    if(a->v){
      drawObj(*(a->model), pm);
    }
}

int main(int argc, char** argv){
  int x=0;
  printf("Convert (1: BMP to LZI, 2: OBJ to LZO) or play (3)?\n");
  scanf("%d",&x);
  if(x==1){
    char* fn=new char[100];
    char* fn2=new char[100];
    int w=0,h=0,b=0,db=0,offset=0;
    printf("Source filename: ");
    scanf("%s",fn);
    printf("width: ");
    scanf("%d",&w);
    printf("height: ");
    scanf("%d",&h);
    printf("BMP offset: ");
    scanf("%d",&offset);
    printf("BPP: ");
    scanf("%d",&b);
    printf("Destination BPP: ");
    scanf("%d",&db);
    printf("Destination filename: ");
    scanf("%s",fn2);
    if(BMPtoLZI(fn,fn2,w,h,offset,b,db)){
      printf("Failed to convert\n");
    }
    return 0;
  }else if(x==2){
    char* infn=new char[100];
    char* outfn=new char[100];
    printf("obj file:\n");
    scanf("%99s", infn);
    printf("lzo file:\n");
    scanf("%99s", outfn);
    if(!ObjtoLZO(infn, outfn)){
      printf("Failed to convert\n");
    }
    delete[] infn;
    delete[] outfn;
    return 0;
  }else if(x==3){
    return prog0(argc,argv);
  }
}

/*
01 C
02 C++
03 Brainfuck
04 Python
05 Pascal
06 Delphi
07 Java
08 PHP
09 Haskell
10 Bash
11 batch
12 Lua
13 javascript
*/
