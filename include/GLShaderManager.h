// GLShaderManager.h
/*
Copyright (c) 2009-2023, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
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

#ifndef __GLT_SHADER_MANAGER
#define __GLT_SHADER_MANAGER

#ifdef QT_IS_AVAILABLE
#include <QOpenGLExtraFunctions>
#endif

#include <stdarg.h>
#include <string.h>

// Maximum length of shader name
#define MAX_SHADER_NAME_LENGTH	64

enum GLT_STOCK_SHADER { GLT_SHADER_IDENTITY = 0, GLT_SHADER_FLAT, GLT_SHADER_SHADED, GLT_SHADER_DEFAULT_LIGHT, GLT_SHADER_POINT_LIGHT_DIFF, GLT_SHADER_TEXTURE_REPLACE, GLT_SHADER_TEXTURE_MODULATE, GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                GLT_SHADER_POINT_SPRITES, GLT_POINT_SPRITES_PLAIN, GLT_SHADER_LAST };


enum GLT_SHADER_ATTRIBUTE { GLT_ATTRIBUTE_VERTEX = 0, GLT_ATTRIBUTE_COLOR, GLT_ATTRIBUTE_NORMAL, 
                                    GLT_ATTRIBUTE_TEXTURE0, GLT_ATTRIBUTE_TEXTURE1, GLT_ATTRIBUTE_TEXTURE2, GLT_ATTRIBUTE_TEXTURE3,
                                    GLT_ATTRIBUTE_LAST};


struct SHADERLOOKUPENTRY {
	char szVertexShaderName[MAX_SHADER_NAME_LENGTH];
	char szFragShaderName[MAX_SHADER_NAME_LENGTH];
	GLuint uiShaderID;
	};

#ifdef QT_IS_AVAILABLE
    class GLShaderManager: protected QOpenGLExtraFunctions
#else
class GLShaderManager
#endif
	{
	public:
		GLShaderManager(void);
		~GLShaderManager(void);

        void freeGL(void); // Free GL resources
		
		// Call before using
		bool InitializeStockShaders(void);
	
		// Use a stock shader, and pass in the parameters needed
		GLint UseStockShader(int nShaderID, ...);

		// Load a shader pair from file, return NULL or shader handle. 
		// Vertex program name (minus file extension)
		// is saved in the lookup table
		GLuint LoadShaderPair(const char *szVertexProgFileName, const char *szFragProgFileName);

		// Load shaders from source text.
		GLuint LoadShaderPairSrc(const char *szName, const char *szVertexSrc, const char *szFragSrc);

		// Ditto above, but pop in the attributes
		GLuint LoadShaderPairWithAttributes(const char *szVertexProgFileName, const char *szFragmentProgFileName, ...);
		GLuint LoadShaderPairSrcWithAttributes(const char *szName, const char *szVertexProg, const char *szFragmentProg, ...);

	
	protected:
		GLuint	uiStockShaders[GLT_SHADER_LAST];
	};


#endif
