
#ifndef __BSP_NODE_H__
#define __BSP_NODE_H__
extern "C" {
#include <inttypes.h>

typedef struct bsp_node_s {
    int16_t part_x_start;
    int16_t part_y_start;
    int16_t part_x_diff;
    int16_t part_y_diff;
    int16_t right_bbox[4];
    int16_t left_bbox[4];
    uint16_t right_child;
    uint16_t left_child;
} bsp_node_t;
}

#endif