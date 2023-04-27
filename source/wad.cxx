#include "wad.hpp"
#include "generic_exception.hpp"
#include <string.h>

WadFile::WadFile(void const* fileData)
: data(fileData)
{
    wadheader_t head = getHeader();
    if (head.magic != IWAD_MAGIC && head.magic != PWAD_MAGIC) {
        throw generic_exception("Not an IWAD or PWAD");
    }
};

wadheader_t WadFile::getHeader() {
    return *((wadheader_t *)data);
};

wad_dirent_t WadFile::getDirectoryEntry(int index) {
    int offset = ((index*16)+(getHeader().offs_dir));
    return *((wad_dirent_t *)(data+offset));
};

void const* WadFile::getLump(char lumpName[8]) {
    for( int i = 0; i < getHeader().numlumps; i++ ) {
        if( getDirectoryEntry(i).lumpname == lumpName) {
            return (void const *)(data + getDirectoryEntry(i).filepos);
        }
    }
    return (void const *)0;
}

int WadFile::getDirIndex(std::string name) {
    char nn[9] = {0,0,0,0,0,0,0,0,0};
    for( int i = 0; i < getHeader().numlumps; i++ ) {
        wad_dirent_t entry = getDirectoryEntry(i);
        strncpy(nn, entry.lumpname, 8);
        if (name == nn) return i;
    }
    return -1;
}
