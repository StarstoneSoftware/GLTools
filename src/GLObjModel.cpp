/*
 *  GLObjModel.cpp
 *  PointMe
 *
 *  Created by Richard Wright on 4/20/11.
 *  Copyright 2011 Starstone Software Systems, Inc. All rights reserved.
 *
 */

#include "GLObjModel.h"
#include <math3d.h>

#include <stdio.h>

#ifndef _WIN32
#define MAX_PATH    255
#endif



GLObjModel::GLObjModel(void) : nGroupCount(0), nMaterialCount(0), pMaterialTable(NULL)
    {
    vObjectTranslation[0] = 0.0f;
    vObjectTranslation[1] = 0.0f;
    vObjectTranslation[2] = 0.0f;
    
    vObjectRotation[0] = 0.0f;
    vObjectRotation[1] = 0.0f;
    vObjectRotation[2] = 1.0f;
    vObjectRotation[3] = 0.0f;
    
    vObjectScale[0] = 1.0f;
    vObjectScale[1] = 1.0f;
    vObjectScale[2] = 1.0f;
    
    UpdateTransformMatrix();
    }
    
GLObjModel::~GLObjModel(void)
    {
    // Delete all the meshes
    for(int i = 0; i < nGroupCount; i++)
        {
        OBJGroup *pGroup = groups[i];
        delete pGroup;
        }
    groups.clear();
    }
    
    
void GLObjModel::UpdateTransformMatrix(void)
    {
    M3DMatrix44f mTrans, mRot, mScale, mTemp;
    
    m3dTranslationMatrix44(mTrans, vObjectTranslation[0],vObjectTranslation[1],vObjectTranslation[2]);    
    m3dRotationMatrix44(mRot, m3dDegToRad(vObjectRotation[0]), vObjectRotation[1],vObjectRotation[2],vObjectRotation[3]);
    m3dScaleMatrix44(mScale, vObjectScale[0],vObjectScale[1],vObjectScale[2]);
    
    m3dMatrixMultiply44(mObjectTransform, mRot, mTrans);
    m3dMatrixMultiply44(mTemp, mObjectTransform, mScale);
    memcpy(mObjectTransform, mTemp, sizeof(M3DMatrix44f));
    }
    
    
////////////////////////////////////////////////////////////////////
// How many times does the given character occur in the string?
int GLObjModel::GetCharCount(const char *szString, char character)
    {
    int retVal = 0;
    
    for(unsigned int i = 0; i < strlen(szString); i++)
        if(szString[i] == character)
            retVal++;
    
    return retVal;
    }


/////////////////////////////////////////////////////////////////////////////////
// Load and preprocess the model. Returns false if the model cannot be found or
// if an error occurs. 
bool GLObjModel::LoadModelOBJ(const char *szModelFile)
    {
    char cLineRead[128];    // Line buffer, 128 bytes is more than enough
#ifndef SB_MOBILE_BUILD
    initializeOpenGLFunctions();
#endif

    // First things first, see if we can open the file
    FILE *pFile;
    pFile = fopen(szModelFile, "r");
    if(pFile == NULL)
        return false;
        
        
    int nTotalVerts = 0;
    int nTotalNormals = 0;
    int nTotalTexCoords = 0;
    int nTotalGroups = 0;
    int nLargestGroup = 0;
    int nCurrentGroup = 0;
    int nTotalTriangles = 0;
    
    while(!feof(pFile)) {
        fgets(cLineRead, 128, pFile);
            
        // Look for the material file
        if(strncmp(cLineRead, "mtllib ", 7) == 0) {
        
        
            // The name of the material file is embedded
            // Extract the path (if any) from the model file
            char cFileName[MAX_PATH];
            strcpy(cFileName, szModelFile);
            
            for(int i = strlen(cFileName); i >= 0; i--)
                if(cFileName[i] == '/')
                    break;
                else 
                    cFileName[i] = NULL;
    
            strncat(cFileName, &cLineRead[7], MAX_PATH);
            int l = strlen(cFileName);
            cFileName[l-1] = NULL;
            cFileName[l-2] = NULL;
            
            LoadMaterialTable(cFileName);        
           // printf("Material File: %s\r\n", cFileName);
            }
            
        // How many groups and how big are they
        if(cLineRead[0] == 'g') {
            if(nCurrentGroup > nLargestGroup)
                nLargestGroup = nCurrentGroup;
                
            nTotalGroups++;
            nCurrentGroup = 0;
            continue;
            }
        
        
        // How many verts
        if(cLineRead[0] == 'v' && cLineRead[1] == ' ') {
            nTotalVerts++;
            continue;
            }
            
        // How many normals
        if(cLineRead[0] == 'v' && cLineRead[1] == 'n') {
            nTotalNormals++;
            continue;
            }
        
        // How many texture coordinates
        if(cLineRead[0] == 'v' && cLineRead[1] == 't') {
            nTotalTexCoords++;
            continue;
            }
        
        // How many faces
        if(cLineRead[0] == 'f') {
            nTotalTriangles++;
            nCurrentGroup++;
            
            continue;
            }
        }

//    printf("Groups: %d\r\nLargest Group: %d\r\nTriangles: %d\r\nVertexes: %d\r\nNormals: %d\r\nTexture Coordinates: %d\r\n",
//                nTotalGroups, nLargestGroup, nTotalTriangles, nTotalVerts, nTotalNormals, nTotalTexCoords);


    fseek(pFile, 0, SEEK_SET);
    
    groups.reserve(nTotalGroups);
        
    // The +1's are just to keep things well behaved if they are zero
    M3DVector3f *vVertexCache = new M3DVector3f[nTotalVerts];
    M3DVector3f *vNormalCache = new M3DVector3f[nTotalNormals+1];
    M3DVector2f *vTexCoordCache = new M3DVector2f[nTotalTexCoords+1];
    
    int         vertexCount = 0;
    int         normalCount = 0;
    int         texCoordCount = 0;
    int         nFaceCount = 0;
    OBJGroup    *pObjGroup = NULL;

    ////////////////////////////////////////////////
    // An OBJ file conists of a series of meshes
    while(!feof(pFile)) {
    
        // Get the next line
        fgets(cLineRead, 128, pFile);
        
        // Skip comments or empty lines
        if(cLineRead[0] == '#' || cLineRead[0] == ' ')
            continue;
        
        // New Group?
        if(cLineRead[0] == 'g') {
            // Close last group
            if(pObjGroup != NULL) {
                pObjGroup->pBatch->End();
                //printf("Vertes: %d\r\n", pObjGroup->pBatch->GetVertexCount());
                }
        
            pObjGroup = new OBJGroup;
            pObjGroup->pBatch->BeginMesh(nLargestGroup * 3);
            groups.push_back(pObjGroup);
            nGroupCount++;
  //          printf("Group Count: %d\r\n", groupCount);
            continue;
            }
            
        // Oh, we are going to make a lot of assumptions here... better do lots of error checking against
        // invalid files.
        
        // Reading in verties
        if(cLineRead[0] == 'v' && cLineRead[1] == ' ') {
            sscanf(cLineRead, "v %f %f %f", &vVertexCache[vertexCount][0],
                                            &vVertexCache[vertexCount][1],
                                            &vVertexCache[vertexCount][2]);
        
            // What about w? I should see if it's there, and if so (and not 1.0), divide
            // the other verts by it.
            // TBD....

            vertexCount++;  
           // printf("vertexCount: %d\r\n", vertexCount);  
           continue;    
            }
        
        // Reading in Texture coordinates
        if(cLineRead[0] == 'v' && cLineRead[1] == 't') {
            sscanf(cLineRead, "vt %f %f", &vTexCoordCache[texCoordCount][0], &vTexCoordCache[texCoordCount][1]);
            
            // No w support here either
            
            texCoordCount++;
            continue;
            }
            
        
        // Reading in normals
        if(cLineRead[0] == 'v' && cLineRead[1] == 'n') {
            sscanf(cLineRead, "vn %f %f %f", &vNormalCache[normalCount][0],
                                             &vNormalCache[normalCount][1],
                                             &vNormalCache[normalCount][2]);
                                             
            normalCount++;
            continue;
            }


        // Set the material for this batch
        if(strncmp(cLineRead, "usemtl ", 7) == 0)
            {
            // Search. If none found, fall through and keep defaults
            for(int i = 0; i < nMaterialCount; i++)
                if(strncmp(&cLineRead[7], pMaterialTable[i].szMaterialName, strlen(pMaterialTable[i].szMaterialName)) == 0) { // Match
                    memcpy(pObjGroup->matAmbient, pMaterialTable[i].vAmbient, sizeof(M3DVector4f));
                    memcpy(pObjGroup->matDiffuse, pMaterialTable[i].vDiffuse, sizeof(M3DVector4f));
                    memcpy(pObjGroup->matSpecular, pMaterialTable[i].vSpecular, sizeof(M3DVector4f));
                    break;
                    }
            
            continue;
            }


        // Face processing. There may be three or more. If more than three, we must make our own
        // triangles. 
        // Oh, and do I really want to figure out this pattern every time???
        if(cLineRead[0] == 'f') {
            nFaceCount++;
                
            //int nVertCount = GetCharCount(cLineRead, '/');
            
            // Just verts
            // f v1 v2 v3 v4 ...
            //if(nVertCount == 0)     // We JUST HAVE verts...
                
            // Vertex Texture
            // f v1/vt1 v2/vt2 v3/vt3 ...
            
            // Vertex texture normal
            // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
    
            // Vertex Normal
            // f v1//vn1 v2//vn2 v3//vn3 ...
            
            
            //nVertCount /= 2;    // Two slashes per vertex
            int verts[3], norms[3];
            sscanf(cLineRead, "f %d//%d %d//%d %d//%d",
                                &verts[0], &norms[0],
                                &verts[1], &norms[1],
                                &verts[2], &norms[2]);
                                
            // Indexes are one based. My arrays are not
            verts[0]--;
            verts[1]--;
            verts[2]--;
            norms[0]--;
            norms[1]--;
            norms[2]--;
            
            // Assemble the triangles, drop it in the batch
            M3DVector3f vTriVerts[3], vTriNorms[3];
            
            for(int i = 0; i < 3; i++) {
                memcpy(vTriVerts[i], vVertexCache[verts[i]], sizeof(M3DVector3f));
                memcpy(vTriNorms[i], vNormalCache[norms[i]], sizeof(M3DVector3f));
                
                //printf("Values: %f %f %f \r\n", vTriVerts[i][0],vTriVerts[i][1],vTriVerts[i][2]);
                }
            
            pObjGroup->pBatch->AddTriangle(vTriVerts, vTriNorms, NULL);
            }
        }    
        
    pObjGroup->pBatch->End();
    
    fclose(pFile);
    delete [] vVertexCache;
    delete [] vNormalCache;
    delete [] vTexCoordCache;
    
    // Materials?
    delete [] pMaterialTable;

   
    return true;
    }
    
    
//////////////////////////////////////////////////////////////////////////////////////////
// Parses the seperate material file that accompanies the OBJ file.
bool GLObjModel::LoadMaterialTable(const char *szMaterialFile)
    {
    char cLineRead[128];
    
    // First things first, see if we can open the file
    FILE *pFile;
    pFile = fopen(szMaterialFile, "r");
    if(pFile == NULL)
        return false;

    // Count how many materials there are
    nMaterialCount = 0;
    while(!feof(pFile)) {
        
        // Get the next line
        fgets(cLineRead, 128, pFile);
            
        if(strncmp(cLineRead, "newmtl ", 7) == 0)
                nMaterialCount++;
        }

    // Back to the beginning of the file
    fseek(pFile, 0, SEEK_SET);

    pMaterialTable = new CMatTable[nMaterialCount];

    // Read them in
    int nCurrMaterial = -1;
    while(!feof(pFile)) {
        
        // Get the next line
        fgets(cLineRead, 128, pFile);
        
        // Skip blanks and comments
        if(cLineRead[0] == '#' || cLineRead[0] == ' ')
            continue;
            
        // New Material?
        if(strncmp(cLineRead, "newmtl ", 7) == 0) {
            nCurrMaterial++;
            pMaterialTable[nCurrMaterial].szMaterialName = strdup(&cLineRead[7]);

            if(nCurrMaterial > nMaterialCount)
                break;              // BADNESS, how could this happen underneath us?

            continue;
            }
            
        // Ambient?
        if(strncmp(cLineRead, "Ka", 2) == 0) {
            sscanf(cLineRead, "Ka %f %f %f", &pMaterialTable[nCurrMaterial].vAmbient[0],
                                             &pMaterialTable[nCurrMaterial].vAmbient[1],
                                             &pMaterialTable[nCurrMaterial].vAmbient[2]);
                                             pMaterialTable[nCurrMaterial].vAmbient[3] = 1.0f;
            continue;
            }
            
        // Diffuse?
        if(strncmp(cLineRead, "Kd", 2) == 0) {
            sscanf(cLineRead, "Kd %f %f %f", &pMaterialTable[nCurrMaterial].vDiffuse[0],
                                             &pMaterialTable[nCurrMaterial].vDiffuse[1],
                                             &pMaterialTable[nCurrMaterial].vDiffuse[2]);
                                             pMaterialTable[nCurrMaterial].vDiffuse[3] = 1.0f;
            continue;
            }
        
        // Specular?
        if(strncmp(cLineRead, "Ks", 2) == 0) {
            sscanf(cLineRead, "Ks %f %f %f", &pMaterialTable[nCurrMaterial].vSpecular[0],
                                             &pMaterialTable[nCurrMaterial].vSpecular[1],
                                             &pMaterialTable[nCurrMaterial].vSpecular[2]);
                                             pMaterialTable[nCurrMaterial].vSpecular[3] = 1.0f;
            continue;
            }
        
        // illum... 0 - 10 mean different things
        // Ns....
        // d ...
        // Tr ...
        // map_....
        // bump ....
        }
        

    fclose(pFile);
    
    return true;
    }
    
    
//////////////////////////////////////////////////////////////////////////////////////////////
// Loading and optimizing from ASCII is extremely slow. Save the binary representation, pre-optimized
bool GLObjModel::SaveModelBinary(const char *szModelFile)
    {
    FILE *pFile;
    pFile = fopen(szModelFile, "wb");
    if(pFile == NULL)
        return false;
        
    // Scope specific transformation data
    fwrite(vObjectTranslation, 1, sizeof(M3DVector3f), pFile);
    fwrite(vObjectRotation, 1, sizeof(M3DVector4f), pFile);
    fwrite(vObjectScale, 1, sizeof(M3DVector3f), pFile);
        
        
    // How many groups are there?
    fwrite(&nGroupCount, 1, sizeof(int), pFile);
        
    for(int i = 0; i < nGroupCount; i++) {
        OBJGroup *pGrouping = groups[i];
        
        fwrite(pGrouping->matAmbient, sizeof(M3DVector4f), 1, pFile);
        fwrite(pGrouping->matDiffuse, sizeof(M3DVector4f), 1, pFile);
        fwrite(pGrouping->matSpecular, sizeof(M3DVector4f), 1, pFile);
        fwrite(pGrouping->cTextureName0, sizeof(char) * 32, 1, pFile);
        fwrite(pGrouping->cTextureName1, sizeof(char) * 32, 1, pFile);
        fwrite(pGrouping->cTextureName2, sizeof(char) * 32, 1, pFile);
        pGrouping->pBatch->SaveMesh(pFile);
        }
    
    fclose(pFile);
    
    return true;
    }
    
//////////////////////////////////////////////////////////////////////////////////////////////
// Loading and optimizing from ASCII is extremely slow. Load the binary representation directly, 
// pre-optimized
bool GLObjModel::LoadModelBinary(const char *szModelFile)
    {
    FILE *pFile;
    pFile = fopen(szModelFile, "rb");
    if(pFile == NULL)
        return false;

    // Scope specific transformation data
    fread(vObjectTranslation, 1, sizeof(M3DVector3f), pFile);
    fread(vObjectRotation, 1, sizeof(M3DVector4f), pFile);
    fread(vObjectScale, 1, sizeof(M3DVector3f), pFile);
    UpdateTransformMatrix();
 
    // How many groups are there?
    fread(&nGroupCount, 1, sizeof(int), pFile);
    groups.reserve(nGroupCount);

    for(int i = 0; i < nGroupCount; i++) {
        OBJGroup *pGrouping = new OBJGroup;
        groups.push_back(pGrouping);

        fread(pGrouping->matAmbient, sizeof(M3DVector4f), 1, pFile);
        fread(pGrouping->matDiffuse, sizeof(M3DVector4f), 1, pFile);
        fread(pGrouping->matSpecular, sizeof(M3DVector4f), 1, pFile);
        fread(pGrouping->cTextureName0, sizeof(char) * 32, 1, pFile);
        fread(pGrouping->cTextureName1, sizeof(char) * 32, 1, pFile);
        fread(pGrouping->cTextureName2, sizeof(char) * 32, 1, pFile);
        
        if((pGrouping->cTextureName0[0] == 0) && (pGrouping->cTextureName1[0] == 0) && (pGrouping->cTextureName2[0] == 0))
            pGrouping->pBatch->LoadMesh(pFile, false);
        else
            pGrouping->pBatch->LoadMesh(pFile, true);
        }

    fclose(pFile);
    
    return true;
    }


    
/////////////////////////////////////////////////////////////////////////////////////////////
// Just traverses all the batches and sets the uniforms for the ambient, diffuse and 
// specular material properties.
void GLObjModel::DrawAllBatches(GLint hAmb, GLint hDiff, GLint hSpec)
    {
    for(int i = 0; i < nGroupCount; i++)
        {
        if(hAmb >= 0)
            glUniform4fv(hAmb, 1, groups[i]->matAmbient);
            
        if(hDiff >= 0)
            glUniform4fv(hDiff, 1, groups[i]->matDiffuse);
            
        if(hDiff >= 0)
            glUniform4fv(hSpec, 1, groups[i]->matSpecular);
        
        groups[i]->pBatch->Draw();
        }
    }
