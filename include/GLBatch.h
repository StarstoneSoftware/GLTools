/*
GLBatch.h
 
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

#ifndef __GL_BATCH__
#define __GL_BATCH__

#include <assert.h>

#include "../Math3D/math3d.h"
#include "GLBatchBase.h"
#include "M3DGeometryTransform.h"
#include "M3DFrame.h"
#include "M3DFrustum.h"
#include "GLShaderManager.h"

class GLBatch: public GLBatchBase
    {
    public:
        GLBatch(void);
        virtual ~GLBatch(void);
        
		// Start populating the array
        void Begin(GLenum primitive, GLuint nVerts);
        void Reset(GLenum primitive);
        inline int NumCurrentVerts(void) { return nVertsBuilding; }
        inline bool IsBatchDone(void) { return bBatchDone; }
        inline GLenum GetPrimitive(void) { return primitiveType; }
        
        
        // Tell the batch you are done
        void End(void);

        // Block Copy in vertex data
        void CopyVertexData3f(M3DVector3f *vVerts);
        void CopyNormalDataf(M3DVector3f *vNorms);
        void CopyColorData4f(M3DVector4f *vColors);
        void CopyTexCoordData2f(M3DVector2f *vTexCoords);

        // Just to make life easier...
        inline void CopyVertexData3f(GLfloat *vVerts) { CopyVertexData3f((M3DVector3f *)vVerts); }
        inline void CopyNormalDataf(GLfloat *vNorms) { CopyNormalDataf((M3DVector3f *)vNorms); }
        inline void CopyColorData4f(GLfloat *vColors) { CopyColorData4f((M3DVector4f *)vColors); }
        inline void CopyTexCoordData2f(GLfloat *vTex) { CopyTexCoordData2f((M3DVector2f *)vTex); }

        virtual void Draw(void);
 
        void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
        void Vertex3fv(M3DVector3f vVertex);
        
        void Normal3f(GLfloat x, GLfloat y, GLfloat z);
        void Normal3fv(M3DVector3f vNormal);
        
        void Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void Color4fv(M3DVector4f vColor);
        
        void TexCoord2f(GLclampf s, GLclampf t);
        void TexCoord2fv(M3DVector2f vTexCoord);

        void MapForUpdate(void);
        void UnmapForUpdate(void);
        inline M3DVector3f* GetVertex3f(int iIndex) { return &(pVerts[iIndex]);}
        void UpdateVert(uint index, M3DVector3f vVertex);

        inline M3DVector4f* GetColor4f(int iIndex) { return &(pColors[iIndex]); }
        void UpdateColor(uint index, M3DVector4f vColor);

        inline M3DVector3f* GetNormal3f(int iIndex) { return &(pNormals[iIndex]); }
        void UpdateNormal(uint index, M3DVector3f vNormal);

        inline M3DVector2f* GetTexCoord2f(int iIndex) { return &(pTexCoords[iIndex]); }
        void UpdateTexCoord(uint index, M3DVector2f vTexCoord);
        
    protected:
        GLenum		primitiveType;		// What am I drawing....
        GLuint      uiVertexArrayObject;
        GLuint		uiVertexArray;
        GLuint      uiNormalArray;
        GLuint		uiColorArray;
        GLuint		uiTextureCoordArray;
        
        GLuint nVertsBuilding;		// Building up vertexes counter (immediate mode emulator)
        GLuint nNumVerts;			// Number of verticies in this batch
		
        bool	bBatchDone = false;			// Batch has been built
        bool    bBuffersMade = false;       // Buffers have been allocated
	
        M3DVector3f *pVerts = nullptr;
        M3DVector3f *pNormals = nullptr;
        M3DVector4f *pColors = nullptr;
        M3DVector2f *pTexCoords = nullptr;
        };

#endif // __GL_BATCH__
