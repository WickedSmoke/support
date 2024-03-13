/*
  GLTF GLB Writer Functions
  Copyright 2024 Karl Robillard
  SPDX-License-Identifier: MIT

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const uint8_t glbHeader[20] = {
    0x67, 0x6c, 0x54, 0x46, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x4a, 0x53, 0x4f, 0x4e
};

/*
 * Write header and begin JSON chunk.
 *
 * \param generator     The asset/generator string.
 *
 * Return error message or NULL if successful.
 */
const char* glb_writeHeader(FILE* fp, const char* generator)
{
    size_t n = fwrite(glbHeader, 1, sizeof(glbHeader), fp);
    if (n != sizeof(glbHeader))
        return "Cannot write to .glb file";

    fprintf(fp, "{\"asset\": {\"version\": \"2.0\", \"generator\": \"%s\"},\n",
            generator);
    return NULL;
}

/*
 * Write the JSON buffers & bufferViews elements for a basic mesh with
 * indices & vertex attributes.  bufferView 0 is the ELEMENT_ARRAY_BUFFER,
 * 1 is the ARRAY_BUFFER.
 *
 * \param indicesLen     Byte length of bufferView 0
 * \param vtxLen         Byte length of bufferView 1
 * \param vtxByteStride  Byte size for each set of attributes in bufferView 1.
 *
 * Return error message or NULL if successful.
 */
const char* glb_jsonBuffersIV(FILE* fp, size_t indicesLen, size_t vtxLen,
                              int vtxByteStride)
{
    static const char _jsonBuffersIV[] =
        "\"buffers\": [{\"byteLength\": %ld}],\n"
        "\"bufferViews\": [{\n"
        "  \"buffer\": 0, \"byteOffset\": 0, \"byteLength\": %ld,\n"
        "  \"target\": 34963\n"
        "  },{\n"
        "  \"buffer\": 0, \"byteOffset\": %ld, \"byteLength\": %ld,\n"
        "  \"byteStride\": %d,\n"
        "  \"target\": 34962\n"
        "}]\n";

    if (fprintf(fp, _jsonBuffersIV, indicesLen + vtxLen,
                indicesLen, indicesLen, vtxLen, vtxByteStride) < 0)
        return "Cannot write to .glb file";
    return NULL;
}

static long glb_finishChunk(FILE* fp, uint32_t padding)
{
    long pos = ftell(fp);
    if (pos > 0) {
        long mod4 = pos & 3;
        if (mod4) {
            mod4 = 4 - mod4;
            fwrite(&padding, 1, mod4, fp);
            pos += mod4;
        }
    }
    return pos;
}

/*
 * Pad JSON chunk, write the binary chunk, and update the header lengths.
 *
 * The caller is responsible for closing the file.
 *
 * \param data      Pointers of buffers to write.
 * \param len       Byte sizes of buffers to write.
 * \param count     Number of data & len entries.
 *
 * Return error message or NULL if successful.
 */
const char* glb_writeBin(FILE* fp, void** data, size_t* len, int count)
{
    static const uint8_t glbBin_[4] = { 0x42, 0x49, 0x4E, 0x00 };
    uint32_t lenPair[2];
    uint32_t total;
    long pos;
    size_t n;

    // Finish JSON chunk.
    fputs("}\n", fp);
    pos = glb_finishChunk(fp, 0x20202020);
    if (pos < 0) {
ftell_err:
        return "Cannot get .glb file position";
    }

    total = 0;
    for (int i = 0; i < count; ++i)
        total += len[i];

    // Write the binary chunk.
    lenPair[0] = total;
    lenPair[1] = *((uint32_t*) glbBin_);
    n = fwrite(lenPair, sizeof(uint32_t), 2, fp);
    if (n != 2) {
write_err:
        return "Cannot write to .glb file";
    }

    for (int i = 0; i < count; ++i) {
        n = fwrite(data[i], 1, len[i], fp);
        if (n != len[i])
            goto write_err;
    }

    // Write header lengths.
    lenPair[1] = pos - sizeof(glbHeader);
    pos = glb_finishChunk(fp, 0);
    if (pos < 0)
        goto ftell_err;
    lenPair[0] = pos;
    fseek(fp, 8, SEEK_SET);
    n = fwrite(lenPair, sizeof(uint32_t), 2, fp);
    if (n != 2)
        return "Cannot write .glb lengths";

    return NULL;
}

/*
 * Find the minimum & maximum components in a vec3 array.
 *
 * \param xyz       Pointer to vec3 data.
 * \param count     Number float triplets in xyz array.
 * \param stride    Number of floats to advance to next vertex.
 * \param min       Minimum result of 3 floats.
 * \param max       Maximum result of 3 floats.
 */
void glb_vec3Bounds(const float* xyz, int count, int stride,
                    float* min, float* max)
{
    memcpy(min, xyz, sizeof(float) * 3);
    memcpy(max, xyz, sizeof(float) * 3);
    for (int i = 1; i < count; ++i) {
        for (int c = 0; c < 3; ++c) {
            if (xyz[c] < min[c])
                min[c] = xyz[c];
            if (xyz[c] > max[c])
                max[c] = xyz[c];
        }
        xyz += stride;
    }
}

#ifdef UNIT_TEST
// gcc -DUNIT_TEST glb.c
#include "glb.h"

uint16_t faceIndices[6] = {
    0, 1, 2, 2, 3, 0
};
float faceAttr[4 * 6] = {
    // position          normal
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f
};

const char accessors[] =
    "\"accessors\": [{\n"
    "  \"bufferView\": 0, \"componentType\": 5123, \"type\": \"SCALAR\",\n"
    "  \"count\": %ld\n"
    "  },{\n"
    "  \"bufferView\": 1, \"componentType\": 5126, \"type\": \"VEC3\",\n"
    "  \"count\": %ld,\n"
    "  \"min\": [%.10f, %.10f, %.10f],\n"
    "  \"max\": [%.10f, %.10f, %.10f]\n"
    "  },{\n"
    "  \"bufferView\": 1, \"componentType\": 5126, \"type\": \"VEC3\",\n"
    "  \"byteOffset\": 12,\n"
    "  \"count\": %ld\n"
    "}],\n";

int main(int argc, char** argv)
{
    FILE* fp = fopen("out.glb", "wb");
    if (fp) {
        const char* error = glb_writeHeader(fp, "example");
        if (! error) {
            fputs("\"scene\": 0,\n"
                  "\"scenes\": [{\"nodes\": [0]}],\n"
                  "\"nodes\": [{\"mesh\": 0}],\n"
                  "\"meshes\": [{\n"
                  "  \"primitives\": [{\n"
                  "    \"attributes\": {\"POSITION\": 1, \"NORMAL\": 2},\n"
                  "    \"indices\": 0\n"
                  "  }]\n"
                  "}],\n", fp);

            float posMin[3], posMax[3];
            glb_vec3Bounds(faceAttr, 4, 6, posMin, posMax);

            fprintf(fp, accessors,
                    sizeof(faceIndices) / sizeof(uint16_t),
                    (size_t) 4, posMin[0], posMin[1], posMin[2],
                                posMax[0], posMax[1], posMax[2],
                    (size_t) 4);

            void* binParts[2];
            size_t binPartsLen[2];
            binParts[0] = faceIndices;
            binParts[1] = faceAttr;
            binPartsLen[0] = sizeof(faceIndices);
            binPartsLen[1] = sizeof(faceAttr);

            glb_jsonBuffersIV(fp, binPartsLen[0], binPartsLen[1],
                              6 * sizeof(float));

            error = glb_writeBin(fp, binParts, binPartsLen, 2);
        }
        fclose(fp);

        if (error) {
            fprintf(stderr, "%s\n", error);
            return 1;
        }
    }
    return 0;
}
#endif
