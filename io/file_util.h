#ifndef FILE_UTIL_H
#define FILE_UTIL_H
/*
 * File Utilities (version 1.1)
 * Written and dedicated to the public domain in 2023 by Karl Robillard.
 */

#ifdef __cplusplus
extern "C" {
#endif

size_t file_size(const char* path);
void*  file_readBinary(const char* path, size_t size);
const char* file_stem(const char* path, size_t* len);

#ifdef __cplusplus
}
#endif
#endif
