#ifndef CLSFILEDIRECTORY_H
#define CLSFILEDIRECTORY_H
#include <dirent.h>
#include <stdlib.h>
#include "clsCString.h"

//#android
#ifdef ANDROID
/* Old compatibility names for C types.  */
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#endif

//global
extern char *__progname;

class FileDirectory
{
public:
    FileDirectory();
    static bool isExistDirOrFile(const char *FilePath);
    static bool isDirectory(const char *path);
    static long GetFileSize(const char *FilePath);
    static CString GetFormatSize(long Size);
    static CString GetCurrentDir();
    static CString GetFullProgramPath();
    static CString GetProgramFileName();
    static bool SaveFile(const char *buff, const char* FilePath);
    static CString ReadFile(const char* FilePath);
    static ulong GetTimeLastChangeFile(const char *filePath);
    static char *GetFileNameExtension(const CString *FilePath);
    static char *GetFileNameExtension(const char *FilePath, int Length);
    static bool SetMaximumFileDiscriptor(int max);  //https://linux.die.net/man/2/setrlimit  https://nxmnpg.lemoda.net/2/setrlimit
    static int GetMaximumFileDiscriptor();

    static bool CopyFile(const char *Source, const char *Dest);
    static bool RemoveFile(const char *Dest);
    static bool SetExecutePermission(const char *PathDest);
};

#endif // CLSFILEDIRECTORY_H
