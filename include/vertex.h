#ifndef __VERTEX_H__
#define __VERTEX_H__

extern "C" {
    #include <inttypes.h>

    typedef struct vertex_s {
        int16_t x_pos;
        int16_t y_pos;
    } vertex_t;
}

#endif