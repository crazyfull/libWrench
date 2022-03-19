
#include "clsFileDirectory.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>

/*
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDirectory _getcwd
#else
#include <unistd.h>
#define GetCurrentDirectory getcwd
#endif
*/

#define  BUFFSIZE 255

#define kb (1000)
#define mb (1000000)
#define gb (1000000000)
#define tb (1000000000000)


FileDirectory::FileDirectory()
{

}

char *FileDirectory::GetFileNameExtension(const CString * FilePath){
    return  GetFileNameExtension(FilePath->Data(), FilePath->length());
}

char *FileDirectory::GetFileNameExtension(const char *FilePath, int Length)
{
    char *str = (char*)FilePath;
    char *ret = nullptr;
    for(int i = Length; i > 0; i--){
        if(str[i] == '/')
            break;

        if(str[i] == '.'){
            ret = str+i;
            break;
        }
    }

    ret = CString::toLower(ret);
    return ret;
}

bool FileDirectory::SetMaximumFileDiscriptor(int max)
{
    struct rlimit limit;
    limit.rlim_cur = max;
    limit.rlim_max = max+10;

    if ( setrlimit( RLIMIT_NOFILE, &limit ) )
    {
        return false;
    }else{
        return true;
    }
}

int FileDirectory::GetMaximumFileDiscriptor()
{
    rlimit limit = {0};
    getrlimit(RLIMIT_NOFILE, &limit);
    //printf("getrlimit %d \n",limit.rlim_cur);
    return limit.rlim_cur;
}

CString FileDirectory::GetFormatSize(long Size)
{
    CString ret;
    //https://en.wikipedia.org/wiki/Mebibyte

    double outSize = 0;
    uint32_t vahed = 0;
    int formtCount = 0;

    if(Size >= tb ){
        outSize = static_cast<double>(Size) / tb;
        memcpy((char*)&vahed, "TB", 2);
        formtCount = 3+1;
    }else if(Size >= gb ){
        outSize = static_cast<double>(Size) / gb;
        memcpy((char*)&vahed, "GB", 2);
        formtCount = 3+1;
    }else if(Size >= mb ){
        outSize = static_cast<double>(Size) / mb;
        memcpy((char*)&vahed, "MB", 2);
        formtCount = 2+1;
    }else if(Size >= kb){
        outSize = static_cast<double>(Size) / kb;
        memcpy((char*)&vahed, "KB", 2);
        formtCount = 1+1;
    }else{
        outSize = Size;
        memcpy((char*)&vahed, "byte", sizeof(vahed));

    }


    // gcvt(f,10,)       // tabe mohem tabdil duble be string


    char strOut[35];
    int strOutLen = 0;
    CString::NumberToString(outSize, strOut);
    for(int i = 0; i < sizeof(strOut);i++)
    {
        if(strOut[i] == '.'){
            strOutLen = i+formtCount;
            if(sizeof(strOut) > strOutLen)
                strOut[strOutLen] = 0;
            break;
        }
    }


    strOut[strOutLen] = ' ';
    ret.append(strOut, strOutLen+1);
    ret.append((char*)&vahed, sizeof(vahed));
    return ret;
}

ulong FileDirectory::GetTimeLastChangeFile(const char *FilePath){
    struct stat attrib;
    ulong ret = 0;
    int isOK = stat(FilePath, &attrib);
    if(isOK==0){
        //strftime(date, 10, "%d-%m-%y", gmtime(&(attrib.st_ctime)));
        //strftime(date, sizeof(date), "%d-%m-%y", localtime(&(attrib.st_ctime)));
        ret = attrib.st_ctim.tv_sec;   //st_mtim
    }
    return ret;
}

bool FileDirectory::isExistDirOrFile(const char *FilePath)
{
    //struct stat   buffer;
    //return (stat (FilePath, &buffer) == 0);
    if( access( FilePath, R_OK ) != -1 ) { //F_OK
        // file exists
        return true;
    } else {
        // file doesn't exist
    }
    return false;
}

bool FileDirectory::isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return false;

    if(S_ISDIR(statbuf.st_mode) != 0){
        return true;
    }
    return false;
}

long FileDirectory::GetFileSize(const char *FilePath){

    struct stat attrib;
    long ret = 0;
    if(stat(FilePath, &attrib)==0){
        //strftime(date, 10, "%d-%m-%y", gmtime(&(attrib.st_ctime)));
        //strftime(date, sizeof(date), "%d-%m-%y", localtime(&(attrib.st_ctime)));
        ret = attrib.st_size;   //st_mtim
    }
    return ret;

    /*
    long fSize = 0;

    FILE *fp = fopen(FilePath, "r");
    if(fp){
        //get size
        fseek(fp, 0, SEEK_END);
        fSize = ftell(fp);
        rewind(fp);
        fclose(fp);
    }

    return fSize;
    */
}

CString FileDirectory::GetCurrentDir()
{
    CString CurrDirectory;// = (FileDirectory::GetFullProgramPath().Data() - 5);
    CString ProgramPath = FileDirectory::GetFullProgramPath();
    if(ProgramPath.isEmpty() == false){
        CurrDirectory.append(ProgramPath.Data(), ProgramPath.length() - strlen(__progname));
    }
    return CurrDirectory;
}

CString FileDirectory::GetFullProgramPath()
{
    //https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
    CString processPatgh;
    char result[ PATH_MAX ];
    memset(result, 0, sizeof(result));
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX );
    if(count > 0){
        processPatgh = result;
    }

    return processPatgh;
}

CString FileDirectory::GetProgramFileName()
{
    CString Path = __progname;
    return  Path;
}

bool FileDirectory::CopyFile(const char* Source, const char* Dest)
{
    if(!Source || !Dest )
        return false;

    bool ret = false;
    std::ifstream  from(Source, std::ios::binary);
    if(from.is_open()){
        std::ofstream  to(Dest, std::ios::binary);
        if(to.is_open()){
            to << from.rdbuf();
            ret = true;
        }
        to.close();
    }

    from.close();
    return ret;
}

bool FileDirectory::RemoveFile(const char* Dest)
{
    if(!Dest)
        return false;

    if(remove(Dest) == 0){
        return true;
    }else{
        return false;
    }
}

bool FileDirectory::SetExecutePermission(const char* PathDest)
{
    if(!PathDest)
        return false;

    //int chmod(const char *path, mode_t mode);
    if(chmod(PathDest, S_IRWXU|S_IRWXG|S_IROTH|S_IWOTH|S_IXOTH) == 0){
        return true;
    }else{
        return false;
    }
}

bool FileDirectory::SaveFile(const char* buff, const char *FilePath){
    FILE *fptr;
    fptr = fopen(FilePath, "wb");
    //a aapend
    //b binary
    //w write
    //a+
    //https://www.programiz.com/c-programming/c-file-input-output

    if(fptr == nullptr)
    {
        //cannot open file
        return false;
    }

    fwrite(buff, strlen(buff), 1, fptr);
    fclose(fptr);
    return true;
}

CString FileDirectory::ReadFile(const char *FilePath)
{
    CString ouput;
    FILE *fp;
    char buffer[512];
    size_t fsize;

    fp = fopen(FilePath, "rb");
    if(fp != nullptr){
        while(1)
        {
            fsize = fread(&buffer, sizeof(char), sizeof(buffer), fp);
            if(fsize == 0)
                break;
            //printf("fsize %zu \n", fsize);
            //printf("buffer %s \n", buffer);
            ouput.append(buffer, fsize);
        }
        fclose(fp);
    }

    return ouput;
}


