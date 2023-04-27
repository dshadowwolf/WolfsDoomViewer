#ifndef __SIDEDEF_H__
#define __SIDEDEF_H__
extern "C" {
#include <inttypes.h>
    typedef struct sidedef_s {
        int16_t x_offs;
        int16_t y_offs;
        char    upper_tex[8];
        char    middle_tex[8];
        int16_t faces_sector;
    } sidedef_t;
}
#endif