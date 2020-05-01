/* GLShaderManager.h

Copyright (c) 2009, Richard S. Wright Jr.
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

#include <GLTools.h>
#include <GLShaderManager.h>
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////
// Stock Shader Source Code
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Identity Shader (GLT_SHADER_IDENTITY)
// This shader does no transformations at all, and uses the current
// glColor value for fragments.
// It will shade between verticies.
static const char *szIdentityShaderVP = 
#ifndef OPENGL_ES						
                                        "#version 400\r\n"
#endif
                                        "in vec4 vVertex;"
										"void main(void) "
										"{ gl_Position = vVertex; "
										"}";
									
static const char *szIdentityShaderFP = 
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
										"precision mediump float;"
#endif
                                        "out vec4 vFragmentColor;"
										"uniform vec4 vColor;"
										"void main(void) "
                                        "{ vFragmentColor = vColor;"
										"}";
									

///////////////////////////////////////////////////////////////////////////////
// Flat Shader (GLT_SHADER_FLAT)
// This shader applies the given model view matrix to the verticies, 
// and uses a uniform color value.
static const char *szFlatShaderVP =	
#ifndef OPENGL_ES
                                    "#version 400\r\n"
#endif
                                    "uniform mat4 mvpMatrix;"
                                    "in vec4 vVertex;"
									"void main(void) "
									"{ gl_Position = mvpMatrix * vVertex; "
									"}";
									
static const char *szFlatShaderFP = 
#ifndef OPENGL_ES
                                    "#version 400\r\n"
#else
									"precision mediump float;"
#endif
                                    "out vec4 vFragmentColor;"
									"uniform vec4 vColor;"
									"void main(void) "
                                    "{ vFragmentColor = vColor; "
#ifndef OPENGL_ES
										// This is slow on iOS hardware, fast on desktop
                                        " if(vFragmentColor.a == 0.0) discard;"
#endif
									"}";

///////////////////////////////////////////////////////////////////////////////
// GLT_SHADER_SHADED
// Point light, diffuse lighting only
static const char *szShadedVP =		
#ifndef OPENGL_ES
                                    "#version 400\r\n"
#endif
                                    "uniform mat4 mvpMatrix;"
                                    "in vec4 vColor;"
                                    "in vec4 vVertex;"
                                    "out vec4 vFragColor;"
									"void main(void) {"
									"vFragColor = vColor; "
									" gl_Position = mvpMatrix * vVertex; "
									"}";

static const char *szShadedFP =     
#ifndef OPENGL_ES
                                    "#version 400\r\n"
#else
									"precision mediump float;"
#endif
                                    "out vec4 vFragmentColor;"
                                    "in vec4 vFragColor; "
									"void main(void) { "
                                    " vFragmentColor = vFragColor; "
#ifndef OPENGL_ES
                                        " if(vFragmentColor.a == 0.0) discard;"
#endif
									"}";
									
// GLT_SHADER_DEFAULT_LIGHT
// Simple diffuse, directional, and vertex based light
static const char *szDefaultLightVP = 
#ifndef OPENGL_ES
                                    "#version 400\r\n"
#endif
                                        "uniform mat4 mvMatrix;"
									  "uniform mat4 pMatrix;"
                                      "out vec4 vFragColor;"
                                      "in vec4 vVertex;"
                                      "in vec3 vNormal;"
									  "uniform vec4 vColor;"
									  "void main(void) { "
									  " mat3 mNormalMatrix;"
									  " mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
									  " mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
									  " mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
									  " vec3 vNorm = normalize(mNormalMatrix * normalize(vNormal));"
									  " vec3 vLightDir = vec3(0.0, 0.0, 1.0); "
									  " float fDot = max(0.0, dot(vNorm, vLightDir)); "
									  " vFragColor.rgb = vColor.rgb * fDot;"
									  " vFragColor.a = vColor.a;"
									  " mat4 mvpMatrix;"
									  " mvpMatrix = pMatrix * mvMatrix;"
									  " gl_Position = mvpMatrix * vVertex; "
									  "}";


static const char *szDefaultLightFP =	
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
										"precision mediump float;"
#endif
                                        "out vec4 vFragmentColor;"
                                        "in vec4 vFragColor; "
										"void main(void) { "
                                        " vFragmentColor = vFragColor; "
										"}";

//GLT_SHADER_POINT_LIGHT_DIFF
// Point light, diffuse lighting only
static const char *szPointLightDiffVP =	  
#ifndef OPENGL_ES
                                          "#version 400\r\n"
#endif
										  "uniform mat4 mvMatrix;"
										  "uniform mat4 pMatrix;"
										  "uniform vec3 vLightPos;"
										  "uniform vec4 vColor;"
                                          "in vec4 vVertex;"
                                          "in vec3 vNormal;"
                                          "out vec4 vFragColor;"
										  "void main(void) { "
										  " mat3 mNormalMatrix;"
										  " mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
										  " mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
										  " mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
										  " vec3 vNorm = normalize(mNormalMatrix * vNormal);"
										  " vec4 ecPosition;"
										  " vec3 ecPosition3;"
										  " ecPosition = mvMatrix * vVertex;"
										  " ecPosition3 = ecPosition.xyz /ecPosition.w;"
										  " vec3 vLightDir = normalize(vLightPos - ecPosition3);"
										  " float fDot = max(0.0, dot(vNorm, vLightDir)); "
										  " vFragColor.rgb = vColor.rgb * fDot;"
										  " vFragColor.a = vColor.a;"
										  " mat4 mvpMatrix;"
										  " mvpMatrix = pMatrix * mvMatrix;"
										  " gl_Position = mvpMatrix * vVertex; "
										  "}";


static const char *szPointLightDiffFP = 
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
										"precision mediump float;"
#endif
                                        "out vec4 vFragmentColor;"
                                        "in vec4 vFragColor; "
										"void main(void) { "
                                        " vFragmentColor = vFragColor; "
										"}";

//GLT_SHADER_TEXTURE_REPLACE
// Just put the texture on the polygons
static const char *szTextureReplaceVP =	
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#endif
                                        "uniform mat4 mvpMatrix;"
                                        "in vec4 vVertex;"
                                        "in vec2 vTexCoord0;"
                                        "out vec2 vTex;"
										"void main(void) "
										"{ vTex = vTexCoord0;" 
										" gl_Position = mvpMatrix * vVertex; "
										"}";
									
static const char *szTextureReplaceFP = 
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
										"precision mediump float;"
#endif
                                        "out vec4 vFragColor;"
                                        "in vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"void main(void) "
                                        "{ "
                                        "float l = texture(textureUnit0, vTex).r;"
                                        "vFragColor = vec4(l, l, l, 1.0); "
										"}";


// Just put the texture on the polygons
/*
static const char *szTextureRectReplaceVP =	
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#endif
                                        "uniform mat4 mvpMatrix;"
                                        "in vec4 vVertex;"
                                        "in vec2 vTexCoord0;"
                                        "out vec2 vTex;"
                                        "void main(void) "
                                        "{ vTex = vTexCoord0;" 
                                        " gl_Position = mvpMatrix * vVertex; "
                                        "}";

static const char *szTextureRectReplaceFP = 
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
                                        "precision mediump float;"
#endif
                                        "out vec4 vFragColor;"
                                        "in vec2 vTex;"
                                        "uniform sampler2DRect textureUnit0;"
                                        "void main(void) "
                                        "{ vFragColor = texture2DRect(textureUnit0, vTex); "
#ifndef OPENGL_ES
                                        " if(vFragColor.a == 0.0) discard;"
#endif
                                        "}";
*/

//GLT_SHADER_TEXTURE_MODULATE
// Just put the texture on the polygons, but multiply by the color (as a unifomr)
static const char *szTextureModulateVP =
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#endif
                                        "uniform mat4 mvpMatrix;"
                                        "in vec4 vVertex;"
                                        "in vec2 vTexCoord0;"
                                        "out vec2 vTex;"
										"void main(void) "
										"{ vTex = vTexCoord0;" 
										" gl_Position = mvpMatrix * vVertex; "
										"}";
									
static const char *szTextureModulateFP = 
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
										"precision mediump float;"
#endif
                                        "out vec4 vFragmentColor;"
                                        "in vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"uniform vec4 vColor;"
										"void main(void) "
                                        "{ vFragmentColor = vColor * texture(textureUnit0, vTex); "
#ifndef OPENGL_ES
                                        " if(vFragmentColor.a < 0.001) discard;"
#endif
										"}";




//GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF
// Point light (Diffuse only), with texture (modulated)
static const char *szTexturePointLightDiffVP =
#ifndef OPENGL_ES
                                                 "#version 400\r\n"
#endif
                                                  "uniform mat4 mvMatrix;"
                                                  "uniform mat4 pMatrix;"
                                                  "uniform vec3 vLightPos;"
                                                  "uniform vec4 vColor;"
                                                  "in vec4 vVertex;"
                                                  "in vec3 vNormal;"
                                                  "out vec4 vFragColor;"
                                                  "in vec2 vTexCoord0;"
                                                  "out vec2 vTex;"
                                                  "void main(void) { "
                                                  " mat3 mNormalMatrix;"
                                                  " mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
                                                  " mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
                                                  " mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
                                                  " vec3 vNorm = normalize(mNormalMatrix * vNormal);"
                                                  " vec4 ecPosition;"
                                                  " vec3 ecPosition3;"
                                                  " ecPosition = mvMatrix * vVertex;"
                                                  " ecPosition3 = ecPosition.xyz /ecPosition.w;"
                                                  " vec3 vLightDir = normalize(vLightPos - ecPosition3);"
                                                  " float fDot = max(0.0, dot(vNorm, vLightDir)); "
                                                  " vFragColor.rgb = (vColor.rgb * fDot);"
                                                  " vFragColor.a = vColor.a;"
                                                  " vTex = vTexCoord0;"
                                                  " mat4 mvpMatrix;"
                                                  " mvpMatrix = pMatrix * mvMatrix;"
                                                  " gl_Position = mvpMatrix * vVertex; "
                                                  "}";


static const char *szTexturePointLightDiffFP =    
#ifndef OPENGL_ES
                                                "#version 400\r\n"
#else
                                                "precision mediump float;"
#endif
                                                "out vec4 vFragmentColor;"
                                                "in vec4 vFragColor;"
                                                "in vec2 vTex;"
                                                "uniform sampler2D textureUnit0;"
                                                "void main(void) { "
                                                " vFragmentColor = vFragColor * texture(textureUnit0, vTex);"
                                                "}";



// GLT_SHADER_PLANET
// Derived from above, but specifically for planets in TheSky.
// Light source is directional, not positional, and textures are upside down.
static const char *szPlanetVP =              
#ifndef OPENGL_ES
                                                  "#version 400\r\n"
#endif
                                                  "uniform mat4 mvMatrix;"
												  "uniform mat4 pMatrix;"
												  "uniform vec3 vLightPos;"
												  "uniform vec4 vColor;"
                                                  "in vec4 vVertex;"
                                                  "in vec3 vNormal;"
                                                  "out vec4 vFragColor;"
                                                  "in vec2 vTexCoord0;"
                                                  "out vec2 vTex;"
												  "void main(void) { "
												  " mat3 mNormalMatrix;"
												  " mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
												  " mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
												  " mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
												  " vec3 vNorm = normalize(mNormalMatrix * vNormal);"
												  " float fDot = max(0.0, dot(vNorm, normalize(vLightPos))); "
												  " vFragColor.rgb = vColor.rgb * fDot;"
												  " vFragColor.a = vColor.a;"
												  " vTex = vec2(vTexCoord0.s, 1.0 - vTexCoord0.t);"
												  " mat4 mvpMatrix;"
												  " mvpMatrix = pMatrix * mvMatrix;"
												  " gl_Position = mvpMatrix * vVertex; "
												  "}";


static const char *szPlanetFP =	
#ifndef OPENGL_ES
                                                "#version 400\r\n"
#else
												"precision mediump float;"
#endif
                                                "out vec4 vFragmentColor;"
                                                "in vec4 vFragColor;"
                                                "in vec2 vTex;"
												"uniform sampler2D textureUnit0;"
												"void main(void) { "
                                                " vFragmentColor = vFragColor * texture(textureUnit0, vTex);"
												"}";


// GLT_SHADER_POINT_SPRITES
// Draws point sprites of a given size and color. Custom texture coordinates are required, as they are generally a lookup.
static const char *szPointSpriteVP =
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#endif
                                        "uniform mat4 mvpMatrix;"
                                        "in vec4 vVertex;"   // XYZ, and size
                                        "in vec4 vTexCoord0;"
                                        "in vec4 vColor;"
                                        "out vec4 vTex;"
                                        "out vec4 vPointColor;"
										"void main(void) "
										"{ vTex = vTexCoord0;"  // Pass through
                                        "  vPointColor = vColor;"
                                        "  gl_PointSize = vVertex.w;"
										" gl_Position = mvpMatrix * vec4(vVertex.xyz, 1.0); "
										"}";
									
static const char *szPointSpriteFP =
#ifndef OPENGL_ES
                                        "#version 400\r\n"
#else
										"precision mediump float;"
#endif
                                        "out vec4 vFragmentColor;"
                                        "in vec4 vTex;"
                                        "in vec4 vPointColor;"
										"uniform sampler2D textureUnit0;"

										"void main(void) "
                                        " { vec2 vTexCoord;"
                                        "   float w = gl_PointCoord.s * vTex.z;"    // Width scaled to texture cell width
                                        "   float h = gl_PointCoord.t * vTex.w;"    // Height scaled to texture cell height
                                        "   vTexCoord.x = vTex.x + w; "
                                        "   vTexCoord.y = vTex.y + h; "
                                        "   vFragmentColor = vPointColor * texture(textureUnit0, vTexCoord); "
#ifndef OPENGL_ES
                                        " if(vFragmentColor.a == 0.0) discard;"
#endif
										"}";


// GLT_SHADER_RINGS
// Saturn ring shader
static const char *szRingsVP =
#ifdef OPENGL_ES
                                "precision highp float;"
#endif
"#version 400\r\n"
"in vec4 vVertex;"
"uniform mat4 mvMatrix;"
"uniform mat4 pMatrix;"
"uniform vec4 vClipPlane;"

"out float fClipDistance;"

"void main()   {"
"	vec4 vTransVert = mvMatrix * vVertex;"
"	vec4 vTransClip = mvMatrix * vClipPlane;"
"	fClipDistance = dot(vTransVert.xyz, vTransClip.xyz); "

"    gl_Position = pMatrix * vTransVert; }";

static const char *szRingsFP =
#ifdef OPENGL_ES
"precision highp float;"
#endif
"#version 400\r\n"
"in float fClipDistance;"
"out vec4 vFragmentColor;"
"void main() { "
"	if(fClipDistance < 0.0)"
"		discard; "
"    vFragmentColor = vec4(1.0, 1.0, 1.0, 1.0); }";





// GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF
// Modulate texture with diffuse point light

///////////////////////////////////////////////////////////////////////////////
// Constructor, just zero out everything
GLShaderManager::GLShaderManager(void)
	{
	// Set stock shader handles to 0... uninitialized
	for(unsigned int i = 0; i < GLT_SHADER_LAST; i++)
		uiStockShaders[i] = 0;
	}
	
///////////////////////////////////////////////////////////////////////////////
// Destructor, turn loose of everything
GLShaderManager::~GLShaderManager(void)
	{
	// Stock shaders is the general (are you initialized test)
//	if(uiStockShaders[0] != 0) {
//		unsigned int i;
//		for(i = 0; i < GLT_SHADER_LAST; i++)
//			glDeleteProgram(uiStockShaders[i]);
//		}
	}
	
	
///////////////////////////////////////////////////////////////////////////////
// Initialize and load the stock shaders
bool GLShaderManager::InitializeStockShaders(void)
	{
	// Be warned, going over 128 shaders may cause a
	// hickup for a reallocation.
//	shaderTable.reserve(128);

#ifndef SB_MOBILE_BUILD
    initializeOpenGLFunctions();
#endif
	
	uiStockShaders[GLT_SHADER_IDENTITY]			= GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szIdentityShaderVP, szIdentityShaderFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
	uiStockShaders[GLT_SHADER_FLAT]				= GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szFlatShaderVP, szFlatShaderFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
	uiStockShaders[GLT_SHADER_SHADED]			= GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szShadedVP, szShadedFP, 2,
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");


	uiStockShaders[GLT_SHADER_DEFAULT_LIGHT]	= GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szDefaultLightVP, szDefaultLightFP, 2,
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
	
	uiStockShaders[GLT_SHADER_POINT_LIGHT_DIFF] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szPointLightDiffVP, szPointLightDiffFP, 2,
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");

	uiStockShaders[GLT_SHADER_TEXTURE_REPLACE]  = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szTextureReplaceVP, szTextureReplaceFP, 2, 
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_TEXTURE_MODULATE] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szTextureModulateVP, szTextureModulateFP, 2,
														GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szTexturePointLightDiffVP, szTexturePointLightDiffFP, 3,
																GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_PLANET] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szPlanetVP, szPlanetFP, 3,
																GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
	
//    uiStockShaders[GLT_SHADER_TEXTURE_RECT_REPLACE] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szTextureRectReplaceVP, szTextureRectReplaceFP, 2, 
  //                                                                                           GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
        
    uiStockShaders[GLT_SHADER_POINT_SPRITES] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szPointSpriteVP, szPointSpriteFP, 3,
																GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
        
        
	uiStockShaders[GLT_SHADER_RINGS] = GLTools::GetGLTools()->gltLoadShaderPairSrcWithAttributes(szRingsVP, szRingsFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
        

    if(uiStockShaders[0] != 0)
		return true;
		
	return false;
	}
	

///////////////////////////////////////////////////////////////////////
// Use a specific stock shader, and set the appropriate uniforms
GLint GLShaderManager::UseStockShader(int nShaderID, ...)
	{
	// Check for out of bounds
	if(nShaderID >= GLT_SHADER_LAST)
		return -1;

	// List of uniforms
	va_list uniformList;
	va_start(uniformList, nShaderID);

	// Bind to the correct shader
	glUseProgram(uiStockShaders[nShaderID]);

	// Set up the uniforms
	GLint iTransform, iModelMatrix, iProjMatrix, iColor, iLight, iTextureUnit, iClipPlane;
	int				iInteger;
	M3DMatrix44f* mvpMatrix;
	M3DMatrix44f*  pMatrix;
	M3DMatrix44f*  mvMatrix;
	M3DVector4f*  vColor;
    M3DVector4f*  vClipPlane;
	M3DVector3f*  vLightPos;

	switch(nShaderID)
		{
		case GLT_SHADER_FLAT:			// Just the modelview projection matrix and the color
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;

//        case GLT_SHADER_TEXTURE_RECT_REPLACE:
		case GLT_SHADER_TEXTURE_REPLACE:	// Just the texture place
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;

		case GLT_SHADER_TEXTURE_MODULATE: // Multiply the texture by the geometry color
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);			

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;
            
        case GLT_SHADER_POINT_SPRITES:
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;
        
		case GLT_SHADER_DEFAULT_LIGHT:
			iModelMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "mvMatrix");
		    mvMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iModelMatrix, 1, GL_FALSE, *mvMatrix);

			iProjMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "pMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, *pMatrix);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;

		case GLT_SHADER_POINT_LIGHT_DIFF:
        case GLT_SHADER_PLANET:
			iModelMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "mvMatrix");
		    mvMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iModelMatrix, 1, GL_FALSE, *mvMatrix);

			iProjMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "pMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, *pMatrix);

			iLight = glGetUniformLocation(uiStockShaders[nShaderID], "vLightPos");
			vLightPos = va_arg(uniformList, M3DVector3f*);
			glUniform3fv(iLight, 1, *vLightPos);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;


        case GLT_SHADER_RINGS:
			iModelMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "mvMatrix");
		    mvMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iModelMatrix, 1, GL_FALSE, *mvMatrix);

			iProjMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "pMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, *pMatrix);

			iClipPlane = glGetUniformLocation(uiStockShaders[nShaderID], "vClipPlane");
			vClipPlane = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iClipPlane, 1, *vClipPlane);
			break;


		case GLT_SHADER_SHADED:		// Just the modelview projection matrix. Color is an attribute
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *pMatrix);
			break;

		case GLT_SHADER_IDENTITY:	// Just the Color
			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
		default:
			break;
		}
	va_end(uniformList);

	return uiStockShaders[nShaderID];
	}


///////////////////////////////////////////////////////////////////////////////
// Load a shader pair from file. The shader pair root is added to the shader
// lookup table and can be found again if necessary with LookupShader.
GLuint GLShaderManager::LoadShaderPair(const char *szVertexProgFileName, const char *szFragProgFileName)
	{
	SHADERLOOKUPETRY shaderEntry;

	// Load shader and test for fail
	shaderEntry.uiShaderID = GLTools::GetGLTools()->gltLoadShaderPair(szVertexProgFileName, szFragProgFileName);
	if(shaderEntry.uiShaderID == 0)
		return 0;
		
	// Add to the table
	strncpy(shaderEntry.szVertexShaderName, szVertexProgFileName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szFragProgFileName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load shaders from source text. If the szName is NULL, just make it and return the handle
// (useful for stock shaders). Otherwize, make sure it's not already there, then add to list
GLuint GLShaderManager::LoadShaderPairSrc(const char *szName, const char *szVertexSrc, const char *szFragSrc)
	{
	// Just make it and return
	if(szName == NULL)
		return GLTools::GetGLTools()->gltLoadShaderPairSrc(szVertexSrc, szFragSrc);
		
			
	// Ok, make it and add to table
	SHADERLOOKUPETRY shaderEntry;
	shaderEntry.uiShaderID = GLTools::GetGLTools()->gltLoadShaderPairSrc(szVertexSrc, szFragSrc);
	if(shaderEntry.uiShaderID == 0)
		return 0;	// Game over, won't compile

	// Add it...
	strncpy(shaderEntry.szVertexShaderName, szName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;		
	}

	
///////////////////////////////////////////////////////////////////////////////////////////////
// Load the shader file, with the supplied named attributes
GLuint GLShaderManager::LoadShaderPairWithAttributes(const char *szVertexProgFileName, const char *szFragmentProgFileName, ...)
	{
	SHADERLOOKUPETRY shaderEntry;

    // Temporary Shader objects
    GLuint hVertexShader;
    GLuint hFragmentShader;   
    GLint testVal;
	
    // Create shader objects
    hVertexShader = glCreateShader(GL_VERTEX_SHADER);
    hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
    // Load them. If fail clean up and return null
    if(GLTools::GetGLTools()->gltLoadShaderFile(szVertexProgFileName, hVertexShader) == false)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
	
    if(GLTools::GetGLTools()->gltLoadShaderFile(szFragmentProgFileName, hFragmentShader) == false)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    // Compile them
    glCompileShader(hVertexShader);
    glCompileShader(hFragmentShader);
    
    // Check for errors
    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    // Link them - assuming it works...
	shaderEntry.uiShaderID = glCreateProgram();
    glAttachShader(shaderEntry.uiShaderID, hVertexShader);
    glAttachShader(shaderEntry.uiShaderID, hFragmentShader);


	// List of attributes
	va_list attributeList;
	va_start(attributeList, szFragmentProgFileName);

	char *szNextArg;
	int iArgCount = va_arg(attributeList, int);	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
		{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(shaderEntry.uiShaderID, index, szNextArg);
		}

	va_end(attributeList);

    glLinkProgram(shaderEntry.uiShaderID);
	
    // These are no longer needed
    glDeleteShader(hVertexShader);
    glDeleteShader(hFragmentShader);  
    
    // Make sure link worked too
    glGetProgramiv(shaderEntry.uiShaderID, GL_LINK_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
		glDeleteProgram(shaderEntry.uiShaderID);
		return 0;
		}
    

	// Add it...
	strncpy(shaderEntry.szVertexShaderName, szVertexProgFileName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szFragmentProgFileName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;		
	}


///////////////////////////////////////////////////////////////////////////////////////////////
// Load the shader from source, with the supplied named attributes
GLuint GLShaderManager::LoadShaderPairSrcWithAttributes(const char *szName, const char *szVertexProg, const char *szFragmentProg, ...)
	{
	SHADERLOOKUPETRY shaderEntry;

    // Temporary Shader objects
    GLuint hVertexShader;
    GLuint hFragmentShader;  
    GLint testVal;
	
    // Create shader objects
    hVertexShader = glCreateShader(GL_VERTEX_SHADER);
    hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
    // Load them. 
    GLTools::GetGLTools()->gltLoadShaderSrc(szVertexProg, hVertexShader);
    GLTools::GetGLTools()->gltLoadShaderSrc(szFragmentProg, hFragmentShader);
   
    // Compile them
    glCompileShader(hVertexShader);
    glCompileShader(hFragmentShader);
    
    // Check for errors
    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    // Link them - assuming it works...
	shaderEntry.uiShaderID = glCreateProgram();
    glAttachShader(shaderEntry.uiShaderID, hVertexShader);
    glAttachShader(shaderEntry.uiShaderID, hFragmentShader);

	// List of attributes
	va_list attributeList;
	va_start(attributeList, szFragmentProg);

	char *szNextArg;
	int iArgCount = va_arg(attributeList, int);	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
		{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(shaderEntry.uiShaderID, index, szNextArg);
		}
	va_end(attributeList);


    glLinkProgram(shaderEntry.uiShaderID);
	
    // These are no longer needed
    glDeleteShader(hVertexShader);
    glDeleteShader(hFragmentShader);  
    
    // Make sure link worked too
    glGetProgramiv(shaderEntry.uiShaderID, GL_LINK_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
		glDeleteProgram(shaderEntry.uiShaderID);
		return 0;
		}
     
	// Add it...
	strncpy(shaderEntry.szVertexShaderName, szName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;		
	}
