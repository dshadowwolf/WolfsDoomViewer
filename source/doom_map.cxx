#include "doom_map.hpp"
#include "wad.hpp"
#include "map_types.h"
#include "line.h"
#include "constants.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <algorithm>
#include <functional>
#include <cmath>

DoomMap::DoomMap(std::string map_name, WadFile *wadFile)
: mapName(map_name), theFile(std::make_shared<WadFile*>(wadFile)) 
{
    int s_index = (*theFile)->getDirIndex(mapName);
    for( int i = 0; i < 8; i++ ) {
        wad_dirent_t entry = (*theFile)->getDirectoryEntry(s_index + i + 1);
        char nn[9] = {0,0,0,0,0,0,0,0,0};
        strncpy(nn,entry.lumpname,8);
        std::string m(nn);
        if (m == "THINGS") {
            things_offs = entry.filepos;
            numthings = ITEM_COUNT(entry.lumpsz, THING_SIZE);
        } else if (m == "LINEDEFS") {
            linedef_offs = entry.filepos;
            numlinedefs = ITEM_COUNT(entry.lumpsz, LINEDEF_SIZE);
        } else if (m == "SIDEDEFS") {
            sidedef_offs = entry.filepos;
            numsidedefs = ITEM_COUNT(entry.lumpsz, SIDEDEF_SIZE);
        } else if (m == "VERTEXES") {
            vertex_offs = entry.filepos;
            numverts = ITEM_COUNT(entry.lumpsz, VERTEX_SIZE);
        } else if (m == "SEGS") {
            segs_offs = entry.filepos;
            numsegs = ITEM_COUNT(entry.lumpsz, SEG_SIZE);
        } else if (m == "SSECTORS") {
            ssector_offs = entry.filepos;
            numsubsectors = ITEM_COUNT(entry.lumpsz, SSECTOR_SIZE);
        } else if (m == "NODES") {
            node_offs = entry.filepos;
            numnodes = ITEM_COUNT(entry.lumpsz, NODE_SIZE);
        } else if (m == "SECTORS") {
            sector_offs = entry.filepos;
            numsectors = ITEM_COUNT(entry.lumpsz, SECTOR_SIZE);
        } else {
            std::cout << "Don't know how to handle lump named " << nn << " so I'm ignoring it." << std::endl;
        }
    }
}

bsp_node_t const* DoomMap::getRootNode() {
    return getBSPNode(numnodes - 1);
}

vertex_t const* DoomMap::getVertex(int16_t index) { 
    if (index > (numverts)) return nullptr;
    size_t offset = ((0x04) * (index)) + (vertex_offs);

    return (vertex_t const*)(*theFile)->gotoWadOffset(offset);
};

bsp_node_t const* DoomMap::getNode(int16_t index) {
    if (index > (numnodes)) return nullptr;
    size_t offset = ((0x1C) * (index)) + (node_offs);

    return (bsp_node_t const*)(*theFile)->gotoWadOffset(offset);
};

subsector_t const* DoomMap::getSubsector(int16_t index) {
    if (index > (numsubsectors)) return nullptr;
    size_t offset = ((0x04) * (index)) + (ssector_offs); 

    return (subsector_t const*)(*theFile)->gotoWadOffset(offset);
};

linedef_t const* DoomMap::getLinedef(int16_t index) {
    if (index > (numlinedefs)) return nullptr;
    size_t offset = ((0x0E) * (index)) + (linedef_offs); 

    return (linedef_t const*)(*theFile)->gotoWadOffset(offset);
};

sector_t const* DoomMap::getSector(int16_t index) {
    if (index > (numsectors)) return nullptr;
    size_t offset = ((0x1A) * (index)) + (sector_offs);

    return (sector_t const*)(*theFile)->gotoWadOffset(offset);
};

sidedef_t const* DoomMap::getSidedef(int16_t index) {
    if (index > (numsidedefs)) return nullptr;
    size_t offset = ((0x1E) * (index)) + (sidedef_offs); 

    return (sidedef_t const*)(*theFile)->gotoWadOffset(offset);
};

seg_t const* DoomMap::getSegment(int16_t index) {
    if (index > (numsegs)) return nullptr;
    size_t offset = ((0x0C) * (index)) + (segs_offs);

    return (seg_t const*)(*theFile)->gotoWadOffset(offset);
};

thing_t const* DoomMap::getThing(int16_t index) {
    if (index > (numthings)) return nullptr;
    size_t offset = ((0x0A) * (index)) + (things_offs); 

    return (thing_t const*)(*theFile)->gotoWadOffset(offset);
};


#define MAX(a,b) (a)>(b)?(a):(b)
#define MIN(a,b) (a)<(b)?(a):(b)

// z\sub{i} = \frac{x\sub{i} - min(x)}{max(x) - min(x)}
// above scaled to the 0 - 1 range
// if we wish to scale to 0 - R_MAX, multiply by R_MAX
// if we have an R_MIN, multiple by (R_MAX - (R_MIN * 2)) then add R_MIN
uint32_t scale_x(int xpos, int MAP_X_MIN, int MAP_X_MAX, int scale_min = 30) {
    uint32_t r_min = scale_min/2, r_max = SW - scale_min;
    uint32_t b_sub_a = r_max - r_min; // scale max
    int32_t incoming_sub_minimum = xpos - MAP_X_MIN;
    int32_t max_sub_min = MAP_X_MAX - MAP_X_MIN;
    double scale_factor = (double)incoming_sub_minimum/(double)max_sub_min;
    double base_scaled = scale_factor * r_max;
    uint32_t result = round(base_scaled + r_min);

    return result;
}

// z\sub{i} = \frac{x\sub{i} - min(x)}{max(x) - min(x)}
// above scaled to the 0 - 1 range
// if we wish to scale to 0 - R_MAX, multiply by R_MAX
// if we have an R_MIN, multiple by (R_MAX - (R_MIN * 2)) then add R_MIN

uint32_t scale_y(int ypos, int MAP_Y_MIN, int MAP_Y_MAX, int scale_min = 30) {
    uint32_t r_min = scale_min/2, r_max = SH - scale_min;
    uint32_t b_sub_a = r_max - r_min; // scale max
    int32_t incoming_sub_minimum = ypos - MAP_Y_MIN;
    int32_t max_sub_min = MAP_Y_MAX - MAP_Y_MIN;
    double scale_factor = (double)incoming_sub_minimum/(double)max_sub_min;
    double base_scaled = scale_factor * r_max;
    int32_t result = round(base_scaled + r_min);

    return SH - result;
}

uint32_t scale_y_noinvert(int ypos, int MAP_Y_MIN, int MAP_Y_MAX, int scale_min = 30) {
    uint32_t r_min = scale_min/2, r_max = SH - scale_min;
    uint32_t b_sub_a = r_max - r_min; // scale max
    int32_t incoming_sub_minimum = ypos - MAP_Y_MIN;
    int32_t max_sub_min = MAP_Y_MAX - MAP_Y_MIN;
    double scale_factor = (double)incoming_sub_minimum/(double)max_sub_min;
    double base_scaled = scale_factor * r_max;
    int32_t result = round(base_scaled + r_min);

    return result;
}

bounds_t* scale_verts(vertex_t const* start, vertex_t const* end, bounds_t map_bounds) {
    int sxi = start->x_pos, sxy = start->y_pos;
    int exi = end->x_pos, exy = end->y_pos;

    int scaled_x_start = scale_x(sxi, map_bounds.start.x_pos, map_bounds.end.x_pos);
    int scaled_y_start = scale_y(sxy, map_bounds.start.y_pos, map_bounds.end.y_pos);
    int scaled_x_end = scale_x(exi, map_bounds.start.x_pos, map_bounds.end.x_pos);
    int scaled_y_end = scale_y(exy, map_bounds.start.y_pos, map_bounds.end.y_pos);

    bounds_t *rv = new bounds_t();
    rv->start.x_pos = scaled_x_start;
    rv->start.y_pos = scaled_y_start;
    rv->end.x_pos = scaled_x_end;
    rv->end.y_pos = scaled_y_end;

    return rv;
}

void DoomMap::render_map_2d(SDL_Renderer *renderer) {
    for( int i = 0; i < getNumLinedefs(); i++ ) {
        linedef_t const *ld = getLinedef(i);
        vertex_t const *start = getVertex(ld->start_vertex);
        vertex_t const *end = getVertex(ld->end_vertex);
        bounds_t *v = scale_verts(start, end, getMapExtents());

        SDL_RenderDrawLine(renderer, v->start.x_pos, v->start.y_pos, v->end.x_pos, v->end.y_pos);
        delete v;
    }

    SDL_RenderPresent(renderer);
}

void DoomMap::render_player_position(SDL_Renderer *renderer) {
    thing_t const* player = getThing(0);
    bounds_t b = getMapExtents();
    int xpos = scale_x(player->x_pos, b.start.x_pos, b.end.x_pos);
    int ypos = scale_y(player->y_pos, b.start.y_pos, b.end.y_pos);
    aacircleRGBA(renderer, xpos, ypos, 2, 255, 95, 31, 255);
    SDL_RenderPresent(renderer);
}

double to_radians(int degrees) {
    return (double)degrees * M_PI / 180.0f;
}

void DoomMap::render_player_fov(SDL_Renderer *renderer) {
    thing_t const* player = getThing(0);
    bounds_t b = getMapExtents();
    int facing_angle = player->facing + 180;

    int sub_half = facing_angle - H_FOV;
    int add_half = facing_angle + H_FOV;
    float sh_rad = to_radians(sub_half);
    float ah_rad = to_radians(add_half);

    float side_1_sin = sin(sh_rad);
    float side_1_cos = cos(sh_rad);
    float side_2_sin = sin(ah_rad);
    float side_2_cos = cos(ah_rad);
    
    int sx = scale_x(player->x_pos, b.start.x_pos, b.end.x_pos), sy = scale_y(player->y_pos, b.start.y_pos, b.end.y_pos);
    int pxa = sx + side_1_cos * SH;
    int pya = sy + side_1_sin * SH;
    int pxb = sx + side_2_cos * SH;
    int pyb = sy + side_2_sin * SH;
    aalineRGBA(renderer, sx, sy, pxa, pya, 255, 95, 31, 255);
    aalineRGBA(renderer, sx, sy, pxb, pyb, 255, 95, 31, 255);
    SDL_RenderPresent(renderer);
}

int to_degrees(double radians) {
    return static_cast<int>(rintf32x((radians * 180) / M_PI));
}

int DoomMap::point_at(vertex_t p) {
    thing_t const* player = getThing(0);

    double delta_x = p.x_pos - player->x_pos;
    float delta_y = p.y_pos - player->y_pos;

    double base = atan2f(delta_y, delta_x);
    return -1 * to_degrees(base); // otherwise pointing "up" (negative) along the Y axis reads as -90 degrees instead of 90 degrees
}

int DoomMap::point_at_from(vertex_t a, vertex_t p) {
    double delta_x = p.x_pos - a.x_pos;
    float delta_y = p.y_pos - a.y_pos;

    double base = atan2f(delta_y, delta_x);
    return -1 * to_degrees(base); // otherwise pointing "up" (negative) along the Y axis reads as -90 degrees instead of 90 degrees
}

#define TAU (M_PI * 2)

// many thanks to `señor chillynips` on "The Lair" Discord for this...
bool DoomMap::isVisible(Line &l) {
    thing_t const* player = getThing(0);
    double pf_rad = to_radians(player->facing);
    double start = fmod((atan2(l.getStart().second - player->y_pos, l.getStart().first - player->x_pos) - pf_rad), TAU);
    double end = fmod((atan2(l.getEnd().second - player->y_pos, l.getEnd().first - player->x_pos) - pf_rad), TAU);
    double lower = MIN(start, end);
    double upper = MAX(start, end);
    double hfov = to_radians(H_FOV);

    if ((lower > M_PI_2 || lower < -M_PI_2) && (upper > M_PI_2 || upper < -M_PI_2)) return false;

    if ((lower >= -hfov && lower <= hfov) || (upper >= -hfov && upper <= hfov)) return true;
    
    return false;
}

// the logic that is here is based on work done by JGTrike on Discord. Any issues with the math and logic are probably mine --
// he has a degree in mathematics and is a university professor of the same.
bool DoomMap::isBoxInFOV(bounds_t box) {
    // set up some basic bits...
    thing_t const* player = getThing(0);
    int px = player->x_pos, py = player->y_pos, facing = player->facing;
    int16_t left = box.start.x_pos, top = box.start.y_pos, right = box.end.x_pos, bottom = box.end.y_pos;
    std::list<bounds_t> sides = { { {left, top}, {right, top} }, { {right, top}, {right, bottom} },
                                  { {left, bottom}, {right, bottom} }, { {left, top}, {left, bottom} } };

    // if we offset the camera (player) position positive-x to point O and compute an arc-tan from O to the end-points of the view angle
    // we can then compute the angles from there to Y and Z (corners of a side) -- if the angle ranges overlap, the side is in camera -- eg: the box is in FOV
    vertex_t O = { px + 400, py };
    vertex_t B = { px + sin(to_radians(facing+H_FOV)) * 300, py + cos(to_radians(facing+H_FOV)) * 300 };
    vertex_t C = { px + sin(to_radians(facing-H_FOV)) * 300, py + cos(to_radians(facing-H_FOV)) * 300 };
    vertex_t A = { px, py };

    for (auto side = sides.begin(); side != sides.end(); side++) {
        bounds_t s = *side;
        vertex_t Z = { s.start.x_pos, s.start.y_pos };
        vertex_t Y = { s.end.x_pos, s.end.y_pos };
        Line l( {s.start.x_pos, s.start.y_pos}, {s.end.x_pos, s.end.y_pos} );
        bool isv = isVisible(l);

        int OAZ = point_at(Z);
        OAZ += OAZ<0?360:0;
        //OAZ += facing; // add in the facing value
        OAZ %= 360;
        int OAY = point_at(Y);
        OAY += OAY<0?360:0;
        //OAY += facing; // ditto
        OAY %= 360;

        int OAB = point_at_from(O, B) + 180;
        int OAC = point_at_from(O, C) + 180;
        
        if (OAC < OAY && OAY < OAB) return true;
        if (OAC < OAZ && OAZ < OAB) return true;            
        if (OAC > OAY && OAB < OAZ) return true;
        if (isv) return true; // for those rare cases the above misses entirely...
    }

    return false;
}

// return true if point is on the back, false if point is on the front
// front/back being left/right node depends on the direction of the
// splitter.
bool point_on_side(float x, float y, bsp_node_t *node) {
    // ZDoom/GZDoom and derivatives seem to rely on a quick and dirty calc
    // to handle this all -- while potentially useful, I think it could be
    // a perf bottleneck... However, it is a much better solution, with
    // the calc returning -1 for 'back' and 1 for 'front' we can just double
    // invert to get a pure boolean... So, cribbed from GZDoom...

    return !!(((int32_t)((int64_t)((int64_t)y - node->part_y_start * node->part_x_diff + (int64_t)node->part_x_start-x * node->part_y_diff) >> 32)) > 0);
}

std::list<Line> DoomMap::getSubSector(uint16_t subsector_number) {
    if (subsector_number > numsubsectors) return std::list<Line>();
    std::list<Line> segments;
    size_t ssoffs = (SSECTOR_SIZE * subsector_number)+ssector_offs;
    subsector_t *subsector = (subsector_t *)(*theFile)->gotoWadOffset(ssoffs);
    for( int i = 0; i < subsector->segment_count; i++) {
        size_t seg_offs = (SEG_SIZE * (subsector->first_segment + i)) + segs_offs;
        seg_t *segment = (seg_t *)(*theFile)->gotoWadOffset(seg_offs);
        vertex_t *start = (vertex_t *)(*theFile)->gotoWadOffset((VERTEX_SIZE * segment->start_vertex)+vertex_offs);
        vertex_t *end = (vertex_t *)(*theFile)->gotoWadOffset((VERTEX_SIZE * segment->end_vertex)+vertex_offs);
        segments.push_back(Line(start->x_pos, start->y_pos, end->x_pos, end->y_pos));
    }
    return segments;
}

bsp_node_t* DoomMap::getBSPNode(uint16_t node_id) {
    size_t offs = (NODE_SIZE * node_id) + node_offs;
    return (bsp_node_t *)(*theFile)->gotoWadOffset(offs);
}

std::list<Line> DoomMap::walk_next(bsp_node_t *current, SDL_Renderer *r) {
    std::list<Line> segments;
    std::list<Line> temp;
    std::list<bool> subs;
    bool isLeft;
    uint16_t ns = current->left_child, os = current->right_child;
    bounds_t boxLeft  = { { current->left_bbox[2],  current->left_bbox[0]  }, { current->left_bbox[3],  current->left_bbox[1]  } },
             boxRight = { { current->right_bbox[2], current->right_bbox[0] }, { current->right_bbox[3], current->right_bbox[1] } };
    bsp_node_t *next, *other;

    thing_t const* player = getThing(0);
    bounds_t b = getMapExtents();

    isLeft = point_on_side(player->x_pos, player->y_pos, current);
    other = NULL;
    next = NULL;
    subs.clear();
    int lsx = scale_x(boxLeft.start.x_pos, b.start.x_pos, b.end.x_pos), lsy = scale_y(boxLeft.start.y_pos, b.start.y_pos, b.end.y_pos);
    int width_left = scale_x(boxLeft.end.x_pos, b.start.x_pos, b.end.x_pos) - lsx, height_left = scale_y(boxLeft.end.y_pos, b.start.y_pos, b.end.y_pos) - lsy;
    int rsx = scale_x(boxRight.start.x_pos, b.start.x_pos, b.end.x_pos), rsy = scale_y(boxRight.start.y_pos, b.start.y_pos, b.end.y_pos);
    int width_right = scale_x(boxRight.end.x_pos, b.start.x_pos, b.end.x_pos) - rsx, height_right = scale_y(boxRight.end.y_pos, b.start.y_pos, b.end.y_pos) - rsy;
    SDL_Rect b_left = { x: lsx, y: lsy, w: width_left, h: height_left }; 
    SDL_Rect b_right = { x: rsx, y: rsy, w: width_right, h: height_right }; 

    // std::function<int16_t*(int16_t)> getColor = [](int16_t in) {
    //     std::srand(in);
    //     int16_t r = static_cast<int16_t>((double)std::rand()/RAND_MAX * 255.0f),
    //             g = static_cast<int16_t>((double)std::rand()/RAND_MAX * 255.0f),
    //             b = static_cast<int16_t>((double)std::rand()/RAND_MAX * 255.0f);
    //     int16_t *rv = new int16_t[4] { r, g, b, 255 };
    //     return rv;
    // };

    if (isLeft) {
        if (isBoxInFOV(boxLeft)) {
            if (ns >= 32768) {
                subs.push_back(true);
                ns -= 32768;
            } else {
                subs.push_back(false);  // traversing if NEXT != NULL, not a subsector
                next = getBSPNode(ns);
            }
            // int16_t *c = getColor(ns);
            // SDL_SetRenderDrawColor(r, c[0], c[1], c[2], c[3]);
            // SDL_RenderDrawRect(r, &b_left);
        } else {
            subs.push_back(false); // not a subsector
        }

        if (isBoxInFOV(boxRight)) {
            if (os >= 32768) {
                subs.push_back(true);
                os -= 32768;
            } else {
                subs.push_back(false);
                other = getBSPNode(os);
            }
        } else {
            subs.push_back(false);
        }
    } else {
        // swap next-step/other-step
        int t = ns;
        ns = os;
        os = t;
        if (isBoxInFOV(boxRight)) {
            if (ns >= 32768) {
                subs.push_back(true);
                ns -= 32768;
            } else {
                subs.push_back(false);
                next = getBSPNode(ns);
            }
            // int16_t *c = getColor(ns);
            // SDL_SetRenderDrawColor(r, c[0], c[1], c[2], c[3]);
            // SDL_RenderDrawRect(r,&b_right);
        } else {
            subs.push_back(false);
        }

        if (isBoxInFOV(boxLeft)) {
            if (os >= 32768) {
                subs.push_back(true);
                os -= 32768;
            } else {
                subs.push_back(false);  // traversing if NEXT != NULL, not a subsector
                other = getBSPNode(os);
            }
        } else {
            subs.push_back(false); // not a subsector
        }

    }

    if (next != NULL) {
        temp = walk_next(next, r);
        std::copy(temp.begin(), temp.end(), std::back_inserter(segments));
    }

    if (other != NULL) {
        temp = walk_next(other, r);
        std::copy(temp.begin(), temp.end(), std::back_inserter(segments));
    }

    if (subs.front() && next == NULL) { 
        temp = getSubSector(ns);
        std::copy(temp.begin(), temp.end(), std::back_inserter(segments));
    }

    if (subs.back() && other == NULL) { 
        temp = getSubSector(os);
        std::copy(temp.begin(), temp.end(), std::back_inserter(segments));
    }

    return segments;
}

std::list<Line> filter_list(std::list<Line> input, std::function<bool(Line &)> filter) {
    std::list<Line> res;
    std::for_each(input.begin(), input.end(), [&res, filter](Line &line) {
        if (filter(line)) res.emplace_front(line);
    });
    return res;
}

void DoomMap::render_fov_subsectors(SDL_Renderer *renderer) {
    bsp_node_t const* root = getRootNode();
    thing_t const* player = getThing(0);
    bounds_t ext = getMapExtents();
    std::list<Line> segments = walk_next((bsp_node_t *)root, renderer);

    //... this needs finished
    
    // okay, we do the iteration slightly different depending on if we are on the "front" or "back"
    // of the splitter. 
    
    // now we can render the returned lines, yay!
    std::for_each(segments.begin(), segments.end(), [&renderer, &ext](Line l) {
        int start_x = scale_x(l.getStart().first, ext.start.x_pos, ext.end.x_pos);
        int start_y = scale_y(l.getStart().second, ext.start.y_pos, ext.end.y_pos);
        int end_x = scale_x(l.getEnd().first, ext.start.x_pos, ext.end.x_pos);
        int end_y = scale_y(l.getEnd().second, ext.start.y_pos, ext.end.y_pos);
        aalineRGBA(renderer, start_x, start_y, end_x, end_y, 50, 205, 50, 255);
    });

    std::list<Line> actuallyVisible = filter_list(segments, std::bind(&DoomMap::isVisible, this, std::placeholders::_1));
    std::for_each(actuallyVisible.begin(), actuallyVisible.end(), [renderer,ext](Line &l) {
        int start_x = scale_x(l.getStart().first, ext.start.x_pos, ext.end.x_pos);
        int start_y = scale_y(l.getStart().second, ext.start.y_pos, ext.end.y_pos);
        int end_x = scale_x(l.getEnd().first, ext.start.x_pos, ext.end.x_pos);
        int end_y = scale_y(l.getEnd().second, ext.start.y_pos, ext.end.y_pos);
        aalineRGBA(renderer, start_x, start_y, end_x, end_y, 255, 255, 255, 255);
    });

    SDL_RenderPresent(renderer);
 
}

#undef TAU
#undef MAX
#undef MIN
