#pragma once
#include <string>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <direct.h>
#include <windows.h>

inline static void makePath(std::string path)
{
    if (path == "")
        return;
    size_t start = 0, end = 0;
    do
    {
        end = path.find("\\", start);
        _mkdir(end != std::string::npos ? path.substr(0, end + 1).c_str() : path.c_str());
        start = end + 1;
    } while (end != std::string::npos);
}

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

inline static int fileWrite(const std::string& name, const char* bulk, int len, bool append = false, bool exclusive = false)
{
    size_t pos = name.rfind("\\");
    if(pos != std::string::npos)
        makePath(name.substr(0, pos));

    FILE* f = NULL;
    if(append)
        f = fopen(name.c_str(), "ab");
    else
    {
        int fd = _open(name.c_str(), (exclusive ? _O_EXCL : 0) | _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
        f = fd == -1 ? NULL : _fdopen(fd, "wb");
    }
    if(!f)
        return 0;

    int result = fwrite(bulk, 1, len, f);
    fclose(f);
    return result;
}

inline static int fileWriteStr(const std::string& name, std::string str, bool append = false, bool exclusive = false)
{
    return fileWrite(name, str.c_str(), str.length(), append, exclusive);
}

inline static int fileAppend(const std::string& name, std::string str)
{
    return fileWriteStr(name, str, true);
}

inline static void fileDelete(const std::string fileName)
{
#ifdef UNICODE
    std::wstring wFileName(fileName.begin(), fileName.end());
    DeleteFile(wFileName.c_str());
#else
    DeleteFile(fileName.c_str());
#endif
}

// tries to open file in "exclusive" mode: if the file does not exist, then the function creates it, otherwise it fails.
inline static int fileWriteExclusive(const std::string& name, std::string str)
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
inline static bool fileExists(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "r");
    if(!f)
        return false;
    fclose(f);
    return true;
}
