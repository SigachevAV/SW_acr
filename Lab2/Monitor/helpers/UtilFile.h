#pragma once
#include <string>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <dirent.h>

inline static char* fileRead(const std::string& path, unsigned long long* fileSize = NULL, bool text = false)
{
    FILE* file = fopen( path.c_str(), "rb" );
    if(!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    size_t bufLen = ftell(file);
    if(fileSize)
        *fileSize = bufLen;
    rewind( file );

    char* buffer = new char[bufLen + (text ? 1 : 0)];
    size_t last = fread(buffer, 1, bufLen, file);
    if(text)
        buffer[last] = 0;

    fclose(file);
    return buffer;
}

inline static char* fileReadStr(const std::string& path)
{
    return fileRead(path, 0, true);
}

inline static size_t fileWrite(const std::string& name, const char* bulk, size_t len, bool append = false, bool exclusive = false)
{
    FILE* f = NULL;
    if(append)
        f = fopen(name.c_str(), "ab");
    else
        f = _fdopen(_open(name.c_str(), (exclusive ? _O_EXCL : 0) | _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE), "wb");
    if(!f)
        return 0;

    size_t result = fwrite(bulk, 1, len, f);
    fclose(f);
    return result;
}

inline static size_t fileWriteStr(const std::string& name, std::string str, bool append = false, bool exclusive = false)
{
    return fileWrite(name, str.c_str(), str.length(), append, exclusive);
}

inline static size_t fileAppend(const std::string& name, std::string str)
{
    return fileWriteStr(name, str, true);
}

// tries to open file in "exclusive" mode: if the file does not exist, then the function creates it, otherwise it fails.
inline static size_t fileWriteExclusive(const std::string& name, std::string str)
{
    return fileWriteStr(name, str, false, true);
}

// number of files in directory
/*inline static int fileCount(const char* dir)
{
    int result = 0;
    DIR* d = opendir(dir);
    struct dirent* ent;
    while(ent = readdir(d))
    {
        std::string name = ent->d_name;
        if(name != "." && name != "..")
            result++;
    }
    closedir(d);
    return result;
}
*/
bool fileExists(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "r");
    if(!f)
        return false;
    fclose(f);
    return true;
}