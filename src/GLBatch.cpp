/*
GLBatch.cpp
 
Copyright (c) 2009, Richard S. Wright Jr.
GLTools Open Source Library
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

#ifndef __GLT_BATCH
#define __GLT_BATCH

#include <GLTools.h>
#include <GLBatch.h>
#include <GLShaderManager.h>


GLBatch::GLBatch(void):uiVertexArray(0), uiNormalArray(0), uiColorArray(0), uiTextureCoordArray(0), nVertsBuilding(0),
            nNumVerts(0), bBatchDone(false), pVerts(NULL), pNormals(NULL), pColors(NULL), pTexCoords(NULL)
	{
#ifndef SB_MOBILE_BUILD
    initializeOpenGLFunctions();
#endif
	}


GLBatch::~GLBatch(void)
	{
	// Vertex buffer objects... this could actually leak... zero is a valid buffer object
	if(uiVertexArray != 0)
		glDeleteBuffers(1, &uiVertexArray);
	
	if(uiNormalArray != 0)
		glDeleteBuffers(1, &uiNormalArray);
	
	if(uiColorArray != 0)
		glDeleteBuffers(1, &uiColorArray);
	
    if(uiTextureCoordArray != 0)
        glDeleteBuffers(1, &uiTextureCoordArray);
        
    if(pVerts != NULL && pVerts != (M3DVector3f *)0xbadf00d)
        delete [] pVerts;
        
    if(pNormals != NULL && pNormals != (M3DVector3f *)0xbadf00d)
        delete [] pNormals;

    if(pColors != NULL && pColors != (M3DVector4f *)0xbadf00d)
        delete [] pColors;
        
    if(pTexCoords != NULL && pTexCoords != (M3DVector2f *)0xbadf00d)
        delete [] pTexCoords;
	}


// Start the primitive batch.
void GLBatch::Begin(GLenum primitive, GLuint nVerts)
	{
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &uiLastBuffer);

    primitiveType = primitive;
    nNumVerts = nVerts;
    nVertsBuilding = 0;
    glGenBuffers(1, &uiVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector3f) * nVerts, NULL, GL_STREAM_DRAW);
    
    glGenBuffers(1, &uiColorArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector4f)* nVerts, NULL, GL_STREAM_DRAW);
    
    glGenBuffers(1, &uiNormalArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector3f)* nVerts, NULL, GL_STREAM_DRAW);
    
    glGenBuffers(1, &uiTextureCoordArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector2f) * nVerts, NULL, GL_STREAM_DRAW);
    }


void GLBatch::Reset(GLenum primitive)
    {
    primitiveType = primitive;
    nVertsBuilding = 0;
    bBatchDone = false;
    }

	
// Block Copy in vertex data
void GLBatch::CopyVertexData3f(M3DVector3f *vVerts) 
	{
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * nNumVerts, vVerts);
    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pVerts = (M3DVector3f*) 0xbadf00d;
    }
        
// Block copy in normal data
void GLBatch::CopyNormalDataf(M3DVector3f *vNorms) 
	{
    glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * nNumVerts, vNorms);
    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pNormals = (M3DVector3f*) 0xbadf00d;
	}

void GLBatch::CopyColorData4f(M3DVector4f *vColors) 
	{
    glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector4f) * nNumVerts, vColors);
    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pColors = (M3DVector4f*)0xbadf00d;
    }

void GLBatch::CopyTexCoordData2f(M3DVector2f *vTexCoords) 
	{
    glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector2f) * nNumVerts, vTexCoords);
    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pTexCoords = (M3DVector2f*)0xbadf00d;
    }
	
// Bind everything up in a little package
void GLBatch::End(void)
	{
    if(nVertsBuilding > 0) {
        // Check to see if items have been added one at a time
        if(pVerts != (M3DVector3f *)0xbadf00d && pVerts != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * nVertsBuilding, pVerts);
            }
            
        if(pColors != (M3DVector4f *)0xbadf00d && pColors != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * nVertsBuilding, pColors);
            }
            
        if(pNormals != (M3DVector3f *)0xbadf00d && pNormals != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * nVertsBuilding, pNormals);
            }
            
        if(pTexCoords != (M3DVector2f *)0xbadf00d && pTexCoords != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * nVertsBuilding, pTexCoords);
            }
        }
        
	bBatchDone = true;
	}


// Add a single vertex to the end of the array
void GLBatch::Vertex3f(GLfloat x, GLfloat y, GLfloat z)
	{
    M3DVector3f v = { x, y, z };
    Vertex3fv(v);
	}
        
void GLBatch::Vertex3fv(M3DVector3f vVertex)
	{
	// First see if the vertex array buffer has been created...
	if(pVerts == NULL) 	// Nope, we need to create it
		pVerts = new M3DVector3f[nNumVerts];
		
	// Ignore if we go past the end, keeps things from blowing up
	if(nVertsBuilding >= nNumVerts)
		return;
	
	// Copy it in...
	memcpy(pVerts[nVertsBuilding], vVertex, sizeof(M3DVector3f));
	nVertsBuilding++;
	}
        
// Unlike normal OpenGL immediate mode, you must specify a normal per vertex
// or you will get junk...
void GLBatch::Normal3f(GLfloat x, GLfloat y, GLfloat z)
	{
    M3DVector3f v = { x, y, z };
    Normal3fv(v);
	}
        
// Ditto above
void GLBatch::Normal3fv(M3DVector3f vNormal)
	{
	// First see if the vertex array buffer has been created...
	if(pNormals == NULL) 	// Nope, we need to create it
		pNormals = new M3DVector3f[nNumVerts];
	
	// Ignore if we go past the end, keeps things from blowing up
	if(nVertsBuilding >= nNumVerts)
		return;
	
	// Copy it in...
	memcpy(pNormals[nVertsBuilding], vNormal, sizeof(M3DVector3f));
	}
	

void GLBatch::Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
	{
    M3DVector4f v = { r, g, b, a };
    Color4fv(v);
	}
	
void GLBatch::Color4fv(M3DVector4f vColor)
	{
	// First see if the vertex array buffer has been created...
	if(pColors == NULL) 	// Nope, we need to create it
        pColors = new M3DVector4f[nNumVerts];
	
	// Ignore if we go past the end, keeps things from blowing up
	if(nVertsBuilding >= nNumVerts)
		return;
	
	// Copy it in...
	memcpy(pColors[nVertsBuilding], vColor, sizeof(M3DVector4f));
	}
        
void GLBatch::TexCoord2f(GLclampf s, GLclampf t)
	{
    M3DVector2f tex = { s, t };
    TexCoord2fv(tex);
    }
   
// Ditto above  
void GLBatch::TexCoord2fv(M3DVector2f vTexCoord)
	{	
    if(pTexCoords == NULL) 	// Nope, we need to create it
        pTexCoords = new M3DVector2f[nNumVerts];
        
	// Ignore if we go past the end, keeps things from blowing up
	if(nVertsBuilding >= nNumVerts)
		return;
	
	// Copy it in...
    memcpy(pTexCoords[nVertsBuilding], vTexCoord, sizeof(M3DVector2f));
	}


void GLBatch::Draw(bool bToggleAttribs)
	{
	if(!bBatchDone)
		return;
    
    if(pVerts) {
        if(bToggleAttribs)
            glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
        
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

    if(pColors) {
        if(bToggleAttribs)
            glEnableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
        
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
        }

    if(pNormals) {
        if(bToggleAttribs)
            glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
        
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

    if(pTexCoords) {
        if(bToggleAttribs)
            glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0);
        
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }

    if(nVertsBuilding != 0)
        glDrawArrays(primitiveType, 0, nVertsBuilding);
    
    if(bToggleAttribs) {
        glDisableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
        glDisableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
        glDisableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
        glDisableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0);
        }
        
    glBindBuffer(GL_ARRAY_BUFFER, uiLastBuffer);
    }

#endif
