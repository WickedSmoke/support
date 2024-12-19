#include <stdio.h>
#include <string.h>

#include "file_util.c"

int main(int argc, char** argv)
{
    const char* paths[6] = {
        "base",
        "base.ext",
        "base-1.0.2.ext",
        "",
        "/path/to/base.ext",
        "C:\\path\\to\\base.ext"
    };
    char buf[40];
    const char* base;
    size_t len;
    (void) argc;
    (void) argv;

    for (int i = 0; i < 6; ++i) {
        base = file_stem(paths[i], &len);
        memcpy(buf, base, len);
        buf[len] = '\0';
        printf("file_stem \"%s\" -> \"%s\"\n", paths[i], buf);
    }
    return 0;
}
