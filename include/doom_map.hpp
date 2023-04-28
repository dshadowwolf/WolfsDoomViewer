#ifndef __DOOM_MAP_HPP__

#include <vector>
#include "wad.hpp"
#include "map_types.h"
#include "line.h"
#include <inttypes.h>
#include <string>
#include <memory>
#include <string.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <list>

typedef struct bounds_s {
    vertex_t start;
    vertex_t end;
} bounds_t;

#define THING_SIZE   0x0A
#define LINEDEF_SIZE 0x0E
#define SIDEDEF_SIZE 0x1E
#define VERTEX_SIZE  0x04
#define SEG_SIZE     0x0C
#define SSECTOR_SIZE 0x04
#define NODE_SIZE    0x1C
#define SECTOR_SIZE  0x1A

#define ITEM_COUNT(lsize, item_size) (uint16_t)((lsize)/(item_size))

class DoomMap {
    size_t vertex_offs;
    uint16_t numverts;
    size_t things_offs;
    uint16_t numthings;
    size_t segs_offs;
    uint16_t numsegs;
    size_t linedef_offs;
    uint16_t numlinedefs;
    size_t sidedef_offs;
    uint16_t numsidedefs;
    size_t ssector_offs;
    uint16_t numsubsectors;
    size_t node_offs;
    uint16_t numnodes;
    size_t sector_offs;
    uint16_t numsectors;
    TTF_Font *dFont;

    std::string mapName;
    std::shared_ptr<WadFile *> theFile;

    protected:
    double point_at(vertex_t p);
    double point_at_from(vertex_t a, vertex_t p);
    bool isBoxInFOV(bounds_t box);
    //std::vector<Line> walk_next(thing_t const* player, SDL_Renderer *renderer, bsp_node_t *node, bounds_t *extents);
    //std::vector<Line> getSubSector(uint16_t subsector_number);
    std::list<Line> walk_next(bsp_node_t *current, SDL_Renderer *r);
    std::list<Line> getSubSector(uint16_t subsector_number);
    bsp_node_t* getBSPNode(uint16_t node_id);
    bool isVisible(Line &);

    public:
    DoomMap(std::string map_name, WadFile *wadFile);
    bsp_node_t const* getRootNode();

    vertex_t const * getVertex(int16_t index);
    bsp_node_t const * getNode(int16_t index);
    subsector_t const * getSubsector(int16_t index);
    linedef_t const * getLinedef(int16_t index);
    sidedef_t const * getSidedef(int16_t index);
    seg_t const * getSegment(int16_t index);
    thing_t const * getThing(int16_t index);
    sector_t const * getSector(int16_t index);

    inline uint16_t getNumLinedefs() { return numlinedefs; };
    inline bounds_t getMapExtents() {
        std::list<int16_t> x_vals, y_vals;
        for (int i = 0; i < numverts; i++) {
            vertex_t const* curr = getVertex(i);
            x_vals.push_back(curr->x_pos);
            y_vals.push_back(curr->y_pos);
        }

        x_vals.sort();
        y_vals.sort();

        bounds_t rv({{ x_vals.front(), y_vals.front() }, { x_vals.back(), y_vals.back() }});
        return rv;
    };

    void render_map_2d(SDL_Renderer *renderer);
    void render_player_position(SDL_Renderer *renderer);
    void render_player_fov(SDL_Renderer *renderer);
    void render_fov_subsectors(SDL_Renderer *renderer);
};

#endif