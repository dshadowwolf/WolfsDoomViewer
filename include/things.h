#ifndef __THINGS_H__
#define __THINGS_H__
extern "C" {
#include <inttypes.h>

#define THING_SKILL_ONE_TWO 1
#define THING_SKILL_THREE 2
#define THING_SKILL_FOUR_FIVE 4
#define THING_AMBUSH 8
#define THING_MULTIPLAYER_ONLY 16

typedef struct thing_s {
    int16_t x_pos;
    int16_t y_pos;
    int16_t facing;
    int16_t type;
    int16_t flags;
} thing_t;

}

#endif