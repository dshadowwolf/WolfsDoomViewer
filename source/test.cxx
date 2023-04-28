#include <boost/iostreams/device/mapped_file.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include "wad.hpp"
#include "doom_map.hpp"
#include <string>
#include "generic_exception.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "constants.h"

int main() {
    boost::iostreams::mapped_file file("DOOM.WAD", boost::iostreams::mapped_file::readonly);
    
    if (!file.is_open()) {
        std::cerr << "Input file not open!" << std::endl;
        return -1;
    }

    WadFile theWad(file.const_data());
    DoomMap map("E1M1", &theWad);

    int err = SDL_Init(SDL_INIT_VIDEO);
    
    if (err < 0) {
        std::cerr << "Could not initialize SDL2: " << SDL_GetError() << std::endl;
        return -2;
    }

    TTF_Init();
    if (!TTF_WasInit()) {
        std::cerr << "Could not initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return -4;
    }

    SDL_Window *window = SDL_CreateWindow("WolfsDoom", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SW, SH, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        std::cerr << "Unable to initialize Window: " << SDL_GetError() << std::endl;
        return -3;
    }

    SDL_Surface *screen = SDL_GetWindowSurface(window);
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderDrawColor(rend, 0,0,0,255);
    SDL_RenderClear(rend);
    SDL_SetRenderDrawColor(rend, 127, 127, 127, 255);
    map.render_map_2d(rend);
    map.render_player_position(rend);
    map.render_player_fov(rend);
    SDL_SetRenderDrawColor(rend, 50, 205, 50, 255);
    map.render_fov_subsectors(rend);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(10000);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();    
    return 0;
}