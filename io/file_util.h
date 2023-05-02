#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#ifdef __cplusplus
extern "C" {
#endif

size_t file_size(const char* path);
void*  file_readBinary(const char* path, size_t size);

#ifdef __cplusplus
}
#endif
#endif
