#ifndef __SEG_H__
#define __SEG_H__
extern "C" {
#include <inttypes.h>
typedef struct seg_s {
    int16_t start_vertex;
    int16_t end_vertex;
    int16_t angle_bam16;
    int16_t linedef;
    int16_t direction; // 0 == same as linedef, 1 == opposite
    int16_t offs_dist;
} seg_t;
}

#endif