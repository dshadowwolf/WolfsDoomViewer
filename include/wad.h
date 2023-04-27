#ifndef __WAD_H__
#define __WAD_H__
extern "C" {
#include <inttypes.h>

typedef struct wadheader_s {
    uint32_t magic;    // [IP] << 24 | 'W' << 16 | 'A' << 8' | 'D'
    uint32_t numlumps; // number of "lumps" in the 'WAD'
    uint32_t offs_dir; // "infotableofs"    
} wadheader_t;

typedef struct wad_dirent_s {
    uint32_t filepos;     // offset to the data
    uint32_t lumpsz;      // size of lump
    char     lumpname[8]; // name of lump, null padded
} wad_dirent_t;

typedef struct wad_s {
    wadheader_t header;
    uint8_t *lump_data;
    wad_dirent_t *directory;
} wad_t;

};

#endif