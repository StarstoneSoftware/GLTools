/*
 *  GLObjModel.h
 *  A .OBJ model loader
 *
 *  Created by Richard Wright on 4/20/11.
 *  Copyright 2011 Software Bisque, Inc. All rights reserved.
 *
 */


#include <GLTools.h>
#include <GLTriangleBatch.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef SB_MOBILE_BUILD
#include <QOpenGLFunctions>
#endif
#include <vector>
using namespace std;

///////////////////////////////////////////////////////////
// Temporary, used for parsing and assigning material properties
class CMatTable {
    public:
        CMatTable(void)
            {
            szMaterialName = NULL;
            }
        
        ~CMatTable(void)
            {
            free(szMaterialName);
            }
        
    char *szMaterialName;
    M3DVector4f vAmbient;
    M3DVector4f vDiffuse;
    M3DVector4f vSpecular;
    };


/////////////////////////////////////////////////////////////////////
// Each group has its own material properties.
class  OBJGroup {
    public:
        OBJGroup(void) 
            {
            pBatch = new GLTriangleBatch();

            // Default color data
            M3DVector4f vAmbient = { 0.2f, 0.2f, 0.2f, 1.0f };
            M3DVector4f vDiff = { 1.0f, 1.0f, 1.0f, 1.0f };
            M3DVector4f vSpec = { 0.0f, 0.0f, 0.0f, 1.0f };
            
            memcpy(matAmbient, vAmbient, sizeof(M3DVector4f));
            memcpy(matDiffuse, vDiff, sizeof(M3DVector4f));
            memcpy(matSpecular, vSpec, sizeof(M3DVector4f));
            memset(cTextureName0, 0, 32);
            memset(cTextureName1, 0, 32);
            memset(cTextureName2, 0, 32);
            }
            
        ~OBJGroup(void)
            {
            delete pBatch;
            }

    GLTriangleBatch *pBatch;
    
    
    //////////////////////////////////////////////////////////
    // Anything beyond these, we'll just have to regenerate
    // the models from the original .obj files.
    M3DVector4f     matAmbient;
    M3DVector4f     matDiffuse;
    M3DVector4f     matSpecular;
    char            cTextureName0[32];  // Color Map
    char            cTextureName1[32];  // Normal Map
    char            cTextureName2[32];  // Specular Map
    };
    
    
//////////////////////////////////////////////////////////////////////////////////////
// The main class. Contains a number of meshes, each of which may have it's own
// material properties. No transforms may be embedded however.
#ifndef SB_MOBILE_BUILD
class GLObjModel : public QOpenGLFunctions
#else
class GLObjModel
#endif
    {
    public:
        GLObjModel(void);
        ~GLObjModel(void);
    
    
        bool LoadModelOBJ(const char *szModelFile);
        
        bool SaveModelBinary(const char *szModelFile);
        bool LoadModelBinary(const char *szModelFile);
                    
        int  GetMeshCount(void) { return nGroupCount; }    
        void DrawAllBatches(GLint hAmb = -1, GLint hDiff = -1, GLint hSpec = -1);
        
        void GetObjectTransformMatrix(M3DMatrix44f m) {
            memcpy(m, mObjectTransform, sizeof(M3DMatrix44f));
            }
            
        void GetObjectTranslation(M3DVector3f m) {
            memcpy(m, vObjectTranslation, sizeof(M3DVector3f));
            }
            
        void GetObjectRotation(M3DVector4f m) {
            memcpy(m, vObjectRotation, sizeof(M3DVector4f));
            }
            
        void GetObjectScale(M3DVector3f m) {
            memcpy(m, vObjectScale, sizeof(M3DVector3f));
            }
            
        void SetObjectTranslation(float x, float y, float z) {
            vObjectTranslation[0] = x;
            vObjectTranslation[1] = y;
            vObjectTranslation[2] = z;
            UpdateTransformMatrix();
            }
            
        void SetObjectRotation(float rot, float x, float y, float z) {
            vObjectRotation[0] = rot;
            vObjectRotation[1] = x;
            vObjectRotation[2] = y;
            vObjectRotation[3] = z;
            UpdateTransformMatrix();
            }
            
        void SetObjectScale(float x, float y, float z) {
            vObjectScale[0] = x;
            vObjectScale[1] = y;
            vObjectScale[2] = z;
            UpdateTransformMatrix();
            }
    
    
    protected:
        bool LoadMaterialTable(const char *szMaterialFile);
        int  GetCharCount(const char *szString, char character);
        void UpdateTransformMatrix(void);

        int  nGroupCount;
        vector <OBJGroup*>  groups;    
        
        int        nMaterialCount;
        CMatTable* pMaterialTable;
        
        M3DMatrix44f    mObjectTransform;
        M3DVector3f     vObjectTranslation;
        M3DVector4f     vObjectRotation;
        M3DVector3f     vObjectScale;
        
        
    };