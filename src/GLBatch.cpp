/*
GLBatch.cpp
 
Copyright (c) 2009-2023, Richard S. Wright Jr.
GLTools Open Source Library
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

#ifndef __GLT_BATCH
#define __GLT_BATCH

#include <GLTools.h>
#include <GLBatch.h>

// Highest 64-bit address. No memory allocation would return this address
#define NOT_VALID_BUT_USED 0xFFFFFFFFFFFFFFFF


GLBatch::GLBatch(void):uiVertexArray(0), uiNormalArray(0), uiColorArray(0), uiTextureCoordArray(0), nVertsBuilding(0),
            nNumVerts(0), bBatchDone(false)
	{
#ifdef QT_IS_AVAILABLE
    initializeOpenGLFunctions();
#endif
    glGenVertexArrays(1, &uiVertexArrayObject);
	}


GLBatch::~GLBatch(void)
	{
    glDeleteVertexArrays(1, &uiVertexArrayObject);

    // This means the buffer is being used
    if(pVerts == (M3DVector3f *)NOT_VALID_BUT_USED)
		glDeleteBuffers(1, &uiVertexArray);
	
    if(pNormals == (M3DVector3f*)NOT_VALID_BUT_USED)
		glDeleteBuffers(1, &uiNormalArray);
	
    if(pColors == (M3DVector4f*)NOT_VALID_BUT_USED)
		glDeleteBuffers(1, &uiColorArray);
	
    if(pTexCoords == (M3DVector2f*)NOT_VALID_BUT_USED)
        glDeleteBuffers(1, &uiTextureCoordArray);

    // In case of error... the pointers might not be null,
    // and not NOT_VALID_BUT_USED. In this case, make sure
    // the memory is freed (started, didn't End(), delete object)
    if(pVerts != nullptr && pVerts != (M3DVector3f*)NOT_VALID_BUT_USED)
        delete [] pVerts;

    if(pNormals != nullptr && pNormals != (M3DVector3f*)NOT_VALID_BUT_USED)
        delete [] pNormals;

    if(pColors != nullptr && pColors != (M3DVector4f*)NOT_VALID_BUT_USED)
        delete [] pColors;

    if(pTexCoords != nullptr && pTexCoords != (M3DVector2f*)NOT_VALID_BUT_USED)
        delete [] pTexCoords;
}


// Start the primitive batch.
void GLBatch::Begin(GLenum primitive, GLuint nVerts)
    {
    // Do we want a test to make sure we don't call this twice?
    // tbd

    primitiveType = primitive;
    nNumVerts = nVerts;
    nVertsBuilding = 0;
    glBindVertexArray(uiVertexArrayObject);
    glGenBuffers(1, &uiVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector3f) * nVerts, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &uiColorArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector4f)* nVerts, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(GLT_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &uiNormalArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector3f)* nVerts, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &uiTextureCoordArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector2f) * nVerts, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, 0);
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
    glBindVertexArray(uiVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * nNumVerts, vVerts);
    glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);

    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pVerts = (M3DVector3f*)NOT_VALID_BUT_USED;
    }
        
// Block copy in normal data
void GLBatch::CopyNormalDataf(M3DVector3f *vNorms) 
	{
    glBindVertexArray(uiVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * nNumVerts, vNorms);
    glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);

    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pNormals = (M3DVector3f*) NOT_VALID_BUT_USED;
	}

void GLBatch::CopyColorData4f(M3DVector4f *vColors) 
	{
    glBindVertexArray(uiVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector4f) * nNumVerts, vColors);
    glEnableVertexAttribArray(GLT_ATTRIBUTE_COLOR);

    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pColors = (M3DVector4f*)NOT_VALID_BUT_USED;
    }

void GLBatch::CopyTexCoordData2f(M3DVector2f *vTexCoords) 
	{
    glBindVertexArray(uiVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector2f) * nNumVerts, vTexCoords);
    glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0);

    nVertsBuilding = nNumVerts; // Make sure this get's drawn
    pTexCoords = (M3DVector2f*)NOT_VALID_BUT_USED;
    }
	
// Bind everything up in a little package
void GLBatch::End(void)
	{
    glBindVertexArray(uiVertexArrayObject);
    if(nVertsBuilding > 0) {
        // Check to see if items have been added one at a time
        if(pVerts != (M3DVector3f *)NOT_VALID_BUT_USED && pVerts != NULL) {
            glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
            glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * nVertsBuilding, pVerts);
            delete [] pVerts; pVerts = (M3DVector3f*)NOT_VALID_BUT_USED;
            }
            
        if(pColors != (M3DVector4f *)NOT_VALID_BUT_USED && pColors != NULL) {
            glEnableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * nVertsBuilding, pColors);
            delete [] pColors; pColors = (M3DVector4f*)NOT_VALID_BUT_USED;
            }
        else
            glDeleteBuffers(1, &uiColorArray);
            
        if(pNormals != (M3DVector3f *)NOT_VALID_BUT_USED && pNormals != NULL) {
            glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
            glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * nVertsBuilding, pNormals);
            delete [] pNormals; pNormals = (M3DVector3f*)NOT_VALID_BUT_USED;
            }
        else
            glDeleteBuffers(1, &uiNormalArray);
            
        if(pTexCoords != (M3DVector2f *)NOT_VALID_BUT_USED && pTexCoords != NULL) {
            glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0);
            glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * nVertsBuilding, pTexCoords);
            delete [] pTexCoords; pTexCoords = (M3DVector2f*)NOT_VALID_BUT_USED;
            }
        else
            glDeleteBuffers(1, &uiTextureCoordArray);
        }
        
	bBatchDone = true;
    glBindVertexArray(0);
}

// *******************************************************************************************
// Make random access to data possible. This maps the buffer object to user accessable memory.
void GLBatch::MapForUpdate(void)
    {
    // Vertexes always exist
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    pVerts = (M3DVector3f*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * nVertsBuilding, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

    // If we have no colors, this is nullptr, otherwise look for sential value 0xbadf00d
    if(pColors != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        pColors = (M3DVector4f*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(M3DVector4f) * nVertsBuilding, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
        }

    // Repeat for normals
    if(pNormals != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        pNormals = (M3DVector3f*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * nVertsBuilding, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
        }

    // Repeat for texture coordinates
    if(pTexCoords != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        pTexCoords = (M3DVector2f*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(M3DVector2f) * nVertsBuilding, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
        }


    }

void GLBatch::UnmapForUpdate(void)
    {
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    pVerts = (M3DVector3f*)NOT_VALID_BUT_USED;

    if(pColors != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pColors = (M3DVector4f*)NOT_VALID_BUT_USED;
        }


    if(pNormals != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pNormals = (M3DVector3f*)NOT_VALID_BUT_USED;
        }

    if(pTexCoords != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pTexCoords = (M3DVector2f*)NOT_VALID_BUT_USED;
        }
    }

// *************************************************************
// Update an individual attribute. Be careful, for perfromance
// sake, we are not validating the indexes in release mode
void GLBatch::UpdateVert(uint index, M3DVector3f vVertex)
    {
    assert(index < nVertsBuilding);
    memcpy(pVerts[index], vVertex, sizeof(M3DVector3f));
    }

void GLBatch::UpdateColor(uint index, M3DVector4f vColor)
    {
    assert(index < nVertsBuilding);
    memcpy(pColors[index], vColor, sizeof(M3DVector4f));
    }

void GLBatch::UpdateNormal(uint index, M3DVector3f vNormal)
    {
    assert(index < nVertsBuilding);
    memcpy(pNormals[index], vNormal, sizeof(M3DVector3f));
    }

void GLBatch::UpdateTexCoord(uint index, M3DVector2f vTexCoord)
    {
    assert(index < nVertsBuilding);
    memcpy(pTexCoords[index], vTexCoord, sizeof(M3DVector2f));
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
    if(pTexCoords == NULL) {	// Nope, we need to create it
        pTexCoords = new M3DVector2f[nNumVerts];
    }

	// Ignore if we go past the end, keeps things from blowing up
	if(nVertsBuilding >= nNumVerts)
		return;
	
	// Copy it in...
    memcpy(pTexCoords[nVertsBuilding], vTexCoord, sizeof(M3DVector2f));
	}


void GLBatch::Draw(void)
	{
	if(!bBatchDone)
		return;
    
    glBindVertexArray(uiVertexArrayObject);
    if(nVertsBuilding != 0)
        glDrawArrays(primitiveType, 0, nVertsBuilding);
    }

#endif
