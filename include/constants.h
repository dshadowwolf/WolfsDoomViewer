#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define DOOM_W 320
#define DOOM_H 200
#define DOOM_RES (DOOM_W, DOOM_H)
#define SCALE 5.0
#define S_WIDTH (int)(DOOM_W * SCALE)
#define S_HEIGHT (DOOM_H * SCALE)
#define WIN_RES (S_WIDTH, S_HEIGHT)
#define H_WIDTH ((int)(S_WIDTH / 2))
#define H_HEIGHT ((int)(S_HEIGHT / 2))
#define SCREENRECT { 0, 0, WIDTH, HEIGHT }
#define SW S_WIDTH
#define SH S_HEIGHT
#define FOV 90
#define H_FOV ((int)(FOV/2))
#endif
