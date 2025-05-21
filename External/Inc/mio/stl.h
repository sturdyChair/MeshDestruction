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

#ifndef __MIO_STL_H__
#define __MIO_STL_H__  1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*
    Funcion to read in a [.stl|.stl-ascii] file that stores a single 3D mesh object (in ASCII
    format). The pointer parameters will be allocated inside this function and must
    be freed by caller.
*/
void mioReadSTL(
	// absolute path to file
	const char* fpath,
	// pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	double** pVertices,
	// pointer to list of normal coordinates associated with each face stored as [xyz,xyz,xyz,...]
	// NOTE: the number of normals is V * 3, where V is the number of vertices in "pVertices"
	double** pNormals,
	// number of vertices (which can be used to deduce the number of vertices)
	unsigned int* numVertices);

/*
    Funcion to write out a [.stl|.stl-ascii] file that stores a single 3D mesh object (in ASCII
    format).
*/
void mioWriteSTL(
    // absolute path to file
	const char* const fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	const double* const pVertices,
    // pointer to list of normal coordinates associated with each face stored as [xyz,xyz,xyz,...]
    // NOTE: the number of normals is V * 3, where V is the number of vertices in "pVertices"
	const double* const pNormals,
    // number of triangles (which can be used to deduce the number of vertices) 
    const unsigned int numVertices);

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef __MIO_STL_H__