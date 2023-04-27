#ifndef __SECTOR_H__
#define __SECTOR_H__
extern "C" {
#include <inttypes.h>
 
typedef struct sector_s {
    int16_t floor_height;
    int16_t ceiling_height;
    char floor_tex[8];
    char ceiling_tex[8];
    int16_t light_level;
    int16_t type;
    int16_t tag_number;
} sector_t;
}
#endif