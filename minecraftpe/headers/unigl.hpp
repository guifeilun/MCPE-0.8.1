#ifndef UNIGL_H
#define UNIGL_H

#ifdef USEGLES
	#include <GLES/gl.h>
	#include <EGL/egl.h>
	#include <GLES/glext.h>

	void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat znear, GLfloat zfar);
#else

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

void initGlFuncs();
extern void (*glDeleteBuffers)(GLsizei n, const GLuint* buffers);
extern void (*glGenBuffers)(GLsizei n, GLuint* buffers);
extern void (*glBindBuffer)(GLenum target, GLuint buffer);
extern void (*glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);

#endif

int glhUnProjectf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate);

#endif
