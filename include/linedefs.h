#ifndef __LINEDEF_H__
#define __LINEDEF_H__
extern "C" {
#include <inttypes.h>
    typedef struct linedef_s {
        int16_t start_vertex;
        int16_t end_vertex;
        int16_t flags;
        int16_t special_type;
        int16_t sector_tag;
        int16_t front_sidedef;
        int16_t back_sidedef;
    } linedef_t;
}

#endif