/*
    processDir v1.1 - A cross-platform directory read funtion.

    Written and dedicated to the public domain by Karl Robillard.
*/

enum ProcessDirResult
{
    PDIR_STOP,
    PDIR_CONTINUE
};

enum ProcessDirType
{
    PDIR_DIR  = 4,
    PDIR_FILE = 8,
    PDIR_LINK = 10
};

typedef int (*ProcessDirFunc)(const char* name, int type, void* user);

#ifdef __cplusplus
extern "C"
#endif
int processDir( const char* path, ProcessDirFunc func, void* user );
