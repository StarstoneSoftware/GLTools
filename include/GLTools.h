// GLTools.h
// OpenGL SuperBible
/* Copyright 1998 - 2003 Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list 
of conditions and the following disclaimer in the documentation and/or other 
materials provided with the distribution.

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

// Non Qt environments (for now...)
#ifndef SB_MOBILE_BUILD
#include <QOpenGLFunctions>
#endif

// There is a static block allocated for loading shaders to 
// prevent heap fragmentation
#define MAX_SHADER_LENGTH   8192

// Bring in OpenGL 
// Windows
#ifdef WIN32
#include <windows.h>		// Must have for Windows platform builds
#define OPENGL_ES			// Using Angle now for OpenGL ES 2.0 on desktop
#endif


// Mac OS X
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE | TARGET_IPHONE_SIMULATOR
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#define OPENGL_ES
//#define glMapBuffer glMapBufferOES
//#define glUnmapBuffer glUnmapBufferOES
//#define GL_WRITE_ONLY GL_WRITE_ONLY_OES
#else
#endif
#endif

// Linux
#ifdef SB_LINUX_BUILD
#ifndef SB_MOBILE_ANDROID
#define GLEW_STATIC
#else
//#include <qgl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
//#define OPENGL_ES -> now defined in the .pro file
//#include <esextensions.h>
//#define glMapBuffer glMapBufferOES
//#define glUnmapBuffer glUnmapBufferOES
//#define GL_WRITE_ONLY GL_WRITE_ONLY_OES
#endif
#endif


// Universal includes
#include <stdio.h>
#include <math.h>
#include <math3d.h>
#include <GLBatch.h>
#include <GLTriangleBatch.h>

class GLTools
        {
	public:
		GLTools() { pMe = NULL; }
		
		static GLTools* GetGLTools() {
			if(pMe == NULL) {
				pMe = new GLTools();
				}
			return pMe;
		}

		void InitializeGL(void) {

		strncpy((char *)szRenderer, (char*)glGetString(GL_RENDERER), 63); szRenderer[63] = 0;
		strncpy((char *)szVendor, (char *)glGetString(GL_VENDOR), 63);     szVendor[63] = 0;
		strncpy((char *)szVersion, (char *)glGetString(GL_VERSION), 63);   szVersion[63] = 0;
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

//#else
//#define GLTools::GetGLTools()-> ::   
//
//// Shader loading support
//void	gltLoadShaderSrc(const char *szShaderSrc, GLuint shader);
//bool	gltLoadShaderFile(const char *szFile, GLuint shader);
//
//GLuint	gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg);
//GLuint   gltLoadShaderPairWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);
//
//GLuint gltLoadShaderPairSrc(const char *szVertexSrc, const char *szFragmentSrc);
//GLuint gltLoadShaderPairSrcWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);
//
//bool gltCheckErrors(GLuint progName = 0);
//
//
//
//#endif

   
///////////////////////////////////////////////////////
// Macros for big/little endian happiness
// These are intentionally written to be easy to understand what they 
// are doing... no flames please on the inefficiency of these.
#ifdef __BIG_ENDIAN__
///////////////////////////////////////////////////////////
// This function says, "this pointer is a little endian value"
// If the value must be changed it is... otherwise, this
// function is defined away below (on Intel systems for example)
inline void LITTLE_ENDIAN_WORD(void *pWord)
	{
    unsigned char *pBytes = (unsigned char *)pWord;
    unsigned char temp;
    
    temp = pBytes[0];
    pBytes[0] = pBytes[1];
    pBytes[1] = temp;
	}

///////////////////////////////////////////////////////////
// This function says, "this pointer is a little endian value"
// If the value must be changed it is... otherwise, this
// function is defined away below (on Intel systems for example)
inline void LITTLE_ENDIAN_DWORD(void *pWord)
	{
    unsigned char *pBytes = (unsigned char *)pWord;
    unsigned char temp;
    
    // Swap outer bytes
    temp = pBytes[3];
    pBytes[3] = pBytes[0];
    pBytes[0] = temp;
    
    // Swap inner bytes
    temp = pBytes[1];
    pBytes[1] = pBytes[2];
    pBytes[2] = temp;
	}
#else

// Define them away on little endian systems
#define LITTLE_ENDIAN_WORD 
#define LITTLE_ENDIAN_DWORD 
#endif


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
