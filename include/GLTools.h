// GLTools.h
// OpenGL SuperBible
/* Copyright 1998 - 2023 Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Neither the name of Richard S. Wright Jr. nor the names of other contributors may be used 
to endorse or promote products derived from this software without specific prior 
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __GLTOOLS__LIBRARY
#define __GLTOOLS__LIBRARY

#ifdef QT_IS_AVAILABLE
#include <qopenglextrafunctions.h>
#endif

// Mac OS X
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE | TARGET_IPHONE_SIMULATOR
#include <OpenGLES/ES3/gl.h>
#define OPENGL_ES
#else
#include <GL/glew.h>
#include <OpenGL/gl.h>		// Apple OpenGL haders (version depends on OS X SDK version)
#endif
#endif


#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif

// There is a static block allocated for loading shaders to 
// prevent heap fragmentation, as this could happend frequently
// and is likely long enough.
#define MAX_SHADER_LENGTH   8192

// Universal includes
#include <stdio.h>
#include <math.h>
#include "math3d.h"
#include "GLBatch.h"
#include "GLTriangleBatch.h"

#ifdef QT_IS_AVAILABLE
class GLTools : public QOpenGLExtraFunctions
#else
class GLTools
#endif
    {
	public:
		GLTools() { pMe = NULL; }
		
		static GLTools* GetGLTools() {
			if(pMe == NULL) {
				pMe = new GLTools();
                pMe->InitializeGL();
				}
			return pMe;
		}

		void InitializeGL(void) {
            #ifdef QT_IS_AVAILABLE
                initializeOpenGLFunctions();
            #endif
    }

	// Shader loading support
	void gltLoadShaderSrc(const char *szShaderSrc, GLuint shader);
    bool gltLoadShaderFile(const char *szFile, GLuint shader);

	GLuint	gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg);
	GLuint   gltLoadShaderPairWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);

	GLuint gltLoadShaderPairSrc(const char *szVertexSrc, const char *szFragmentSrc);
	GLuint gltLoadShaderPairSrcWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);

	bool gltCheckErrors(GLuint progName = 0);

	protected:
		static GLTools*	pMe;

	public:
		static GLubyte szVendor[64];
		static GLubyte szRenderer[64];
		static GLubyte szVersion[64];
	};



///////////////////////////////////////////////////////////////////////////////
//         THE LIBRARY....
///////////////////////////////////////////////////////////////////////////////

// Get the OpenGL version
void gltGetOpenGLVersion(GLint &nMajor, GLint &nMinor);

// Set working directory to /Resources on the Mac
void gltSetWorkingDirectory(const char *szArgv);

///////////////////////////////////////////////////////////////////////////////
// Win32 Only
#ifdef WIN32
BYTE* gltReadBMPBits(const char *szFileName, int *nWidth, int *nHeight);
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Load a .TGA file
GLbyte *gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat);

// Capture the frame buffer and write it as a .tga
// Does not work on the iPhone
#ifndef OPENGL_ES
GLint gltGrabScreenTGA(const char *szFileName);
#endif


void gltComputeNormalMatrix(M3DMatrix33f& mNormal, const M3DMatrix44f& mModelView);


class GLTriangleBatch;

// Make Objects
void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
void gltMakeSphere(GLTriangleBatch& sphereBatch, GLfloat fRadius, GLint iSlices, GLint iStacks);
void gltMakeDisk(GLTriangleBatch& diskBatch, GLfloat innerRadius, 
                    GLfloat outerRadius, GLint nSlices, GLint nStacks, GLfloat fDegrees = 360.0f);
void gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, 
                            GLfloat fLength, GLint numSlices, GLint numStacks, GLfloat fDegrees = 360.0f);
void gltMakeCube(GLBatch& cubeBatch, GLfloat fRadius);


#endif
