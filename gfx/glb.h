/*
  GLTF GLB Writer Functions (v1.0)
  Copyright 2024 Karl Robillard
  SPDX-License-Identifier: MIT
*/
#ifndef GLB_H
#define GLB_H

#include <stdio.h>

/*
accessors/componentType

#define GL_UNSIGNED_SHORT   0x1403  // 5123
#define GL_INT              0x1404  // 5124
#define GL_UNSIGNED_INT     0x1405  // 5125
#define GL_FLOAT            0x1406  // 5126

bufferView/target

#define GL_ARRAY_BUFFER         0x8892  // 34962
#define GL_ELEMENT_ARRAY_BUFFER 0x8893  // 34963

meshes/primitives/mode

#define GL_POINTS           0x0000
#define GL_LINES            0x0001
#define GL_LINE_LOOP        0x0002
#define GL_LINE_STRIP       0x0003
#define GL_TRIANGLES        0x0004
#define GL_TRIANGLE_STRIP   0x0005
#define GL_TRIANGLE_FAN     0x0006
*/

#ifdef __cplusplus
extern "C" {
#endif

const char* glb_writeHeader(FILE* fp, const char* generator);
const char* glb_jsonBuffersIV(FILE* fp, size_t indicesLen, size_t vtxLen,
                              int vtxByteStride);
const char* glb_writeBin(FILE* fp, void** data, size_t* len, int count);
void        glb_vec3Bounds(const float* xyz, int count, int stride,
                           float* min, float* max);

#ifdef __cplusplus
}
#endif
#endif
