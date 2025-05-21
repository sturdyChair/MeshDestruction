/***************************************************************************
 *
 *  Copyright (C) 2024 CutDigital Enterprise Ltd
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  For your convenience, a copy of the License has been included in this
 *  repository.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.WE
 *
 **************************************************************************/

#ifndef __MIO_OFF_H__
#define __MIO_OFF_H__  1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*
    Funcion to read in an .off file that stores a single 3D mesh object (in ASCII
    format). The pointer parameters will be allocated inside this function and must
    be freed by caller.
*/
void mioReadOFF(
    // absolute path to file
    const char* fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
    double** pVertices,
    // pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
    unsigned int** pFaceVertexIndices,
    // pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
    unsigned int** pFaceSizes,
    // number of vertices in "pVertices"
    unsigned int* numVertices,
    // number of faces (number of elements in "pFaceSizes")
    unsigned int* numFaces);

/*
    Funcion to write out a .obj file that stores a single 3D mesh object (in ASCII
    format). 
    NOTE: To ignore edges when writing the output just pass pEdgeVertexIndices = NULL and set numEdges = 0
*/
void mioWriteOFF(
    // absolute path to file
    const char* fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
    double* pVertices,
    // pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
    unsigned int* pFaceVertexIndices,
    // pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
    unsigned int* pFaceSizes,
    // pointer to list of edge-vertex indices stored as [ij,ij,ij,ij,ij,...]
    unsigned int* pEdgeVertexIndices,
    // number of vertices in "pVertices"
    unsigned int numVertices,
    // number of faces (number of elements in "pFaceSizes")
    unsigned int numFaces,
    // number of edges (number of elements in "pEdgeVertexIndices" divided by 2)
    unsigned int numEdges);

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef __MIO_OFF_H__