/*
 *  GLTriangleBatch.h
 *  OpenGL SuperBible
 *
Copyright (c) 2007-2023, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source forms, with or without modification, 
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

 *  This class allows you to simply add triangles as if this class were a 
 *  container. The AddTriangle() function searches the current list of triangles
 *  and determines if the vertex/normal/texcoord is a duplicate. If so, it addes
 *  an entry to the index array instead of the list of vertices.
 *  When finished, call EndMesh() to free up extra unneeded memory that is reserved
 *  as workspace when you call BeginMesh().
 *
 *  This class can easily be extended to contain other vertex attributes, and to 
 *  save itself and load itself from disk (thus forming the beginnings of a custom
 *  model file format).
 *
 */

#ifndef __GLT_TRIANGLE_BATCH
#define __GLT_TRIANGLE_BATCH

#include <stdio.h>
#include <limits.h>

#include "math3d.h"
#include "GLBatchBase.h"
#include "GLShaderManager.h"


#define VERTEX_DATA     0
#define NORMAL_DATA     1
#define TEXTURE_DATA    2
#define INDEX_DATA      3

#ifdef QT_IS_AVAILABLE
#include <qopenglextrafunctions.h>
class GLTriangleBatch : public GLBatchBase
#else
class GLTriangleBatch
#endif
    {
    public:
        GLTriangleBatch(void);
        virtual ~GLTriangleBatch(void);
        
        // Use these three functions to add triangles
        void BeginMesh(GLuint nMaxVerts);
        void AddTriangle(M3DVector3f verts[3], M3DVector3f vNorms[3], M3DVector2f vTexCoords[3], float epsilon = 0.00001f, int nCheckRange = INT_MAX);
        void End(void);

        // Useful for statistics
        inline GLuint GetIndexCount(void) { return nNumIndexes; }
        inline GLuint GetVertexCount(void) { return nNumVerts; }

		inline GLfloat GetBoundingSphere(void) { return boundingSphereRadius; }

		bool SaveMesh(const char *szFileName);
		bool LoadMesh(const char *szFileName, bool bNormals = true, bool bTexCoords = true);
        
        bool SaveMesh(FILE *pFile);
        bool LoadMesh(FILE *pFile, bool bNormals = true, bool bTexCoords = true);
        
        // Draw - make sure you call glEnableClientState for these arrays
        virtual void Draw(void);
        
    protected:
        GLushort  *pIndexes = nullptr;         // Array of indexes
        M3DVector3f *pVerts = nullptr;         // Array of vertices
        M3DVector3f *pNorms = nullptr;         // Array of normals
        M3DVector2f *pTexCoords = nullptr;     // Array of texture coordinates
        
        GLuint nMaxIndexes;         // Maximum workspace
        GLuint nNumIndexes;         // Number of indexes currently used
        GLuint nNumVerts;           // Number of vertices actually used
        
        bool   bMadeStuff;
        GLuint bufferObjects[4];
        GLuint vertexArrayBufferObject;
        GLfloat	boundingSphereRadius;
    };


#endif
