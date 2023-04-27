#ifndef __LINE_H__

#include <utility>
#include <inttypes.h>

class Line {
    std::pair<int16_t, int16_t> start;
    std::pair<int16_t, int16_t> end;

    public:
    inline Line(int16_t sx, int16_t sy, int16_t ex, int16_t ey): start(sx,sy), end(ex, ey) {};
    inline Line(std::pair<int16_t, int16_t> s, std::pair<int16_t, int16_t> e): start(s), end(e) {};

    inline std::pair<int16_t, int16_t> getStart() { return start; };
    inline std::pair<int16_t, int16_t> getEnd() { return end; };

};

#define __LINE_H__
#endif