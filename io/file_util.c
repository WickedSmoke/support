#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

size_t file_size(const char* path)
{
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA fad;
    LARGE_INTEGER size;
    if (! GetFileAttributesExA(path, GetFileExInfoStandard, &fad))
        return -1;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart  = fad.nFileSizeLow;
    return size.QuadPart;
#else
    struct stat ss;
    if (stat(path, &ss) == -1)
        return -1;
    return ss.st_size;
#endif
}

void* file_readBinary(const char* path, size_t size)
{
    void* buf = malloc(size);
    if (buf) {
        FILE* fp = fopen(path, "rb");
        if (fp) {
            size_t n = fread(buf, 1, size, fp);
            fclose(fp);
            if (n != size) {
                free(buf);
                buf = NULL;
            }
        }
    }
    return buf;
}
