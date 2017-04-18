#ifndef LZO_H
#define LZO_H

#ifndef MAX_VERTS1
#define MAX_VERTS1 10000
#endif

#ifndef MAX_VERTS2
#define MAX_VERTS2 20000
#endif

#include <GL/gl.h>

#define MAX_VAAC 7
#define VEC_ATTRIB 0
#define COL_ATTRIB 1
#define TC_ATTRIB 2
#define TANG_ATTRIB 3
#define BITA_ATTRIB 4
#define NORM_ATTRIB 5

typedef struct ObjPack{
	GLuint vao;
	GLenum mode;//GL_TRIANGLES, etc.
	GLenum type;//GL_UNSIGNED_INT, etc.
	//float mm[16];//modlMatr
	bool vaas[MAX_VAAC];

	GLsizei pc;

	GLuint prog;
} ObjPack;

int ObjtoLZO(const char*, const char*);
int createTestFile(const char* fn);
int loadLZO(const char* fn, ObjPack*);

#endif
