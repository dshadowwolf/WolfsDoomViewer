#ifndef __WAD_HPP__
#define __WAD_HPP__
#include "wad.h"
#include <string>
#include <iostream>

#define MAKE_ID(a,b,c,d) (((((d) << 24) | ((c) << 16)) | ((b) << 8)) | (a))
#define IWAD_MAGIC MAKE_ID('I','W','A','D')
#define PWAD_MAGIC MAKE_ID('P','W','A','D')

class WadFile {
    void const *data;

    public:
    WadFile(void const* fileData);

    wadheader_t getHeader();
    wad_dirent_t getDirectoryEntry(int index);
    void const* getLump(char lumpName[8]);

    int getDirIndex(std::string name);
    void const* gotoWadOffset(size_t offset) { return (void const *)(data + offset); };
    void const* operator + (size_t offset) { return (void const *)(data + offset); };
    void const* operator[] (size_t offset) { return gotoWadOffset(offset); };
};

#endif
