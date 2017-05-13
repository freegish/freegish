/*
Copyright (C) 2005, 2010 - Cryptic Sea

This file is part of Gish.

Gish is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "../config.h"

#include "../sdl/video.h"

int numofsdlvideomodes;
_sdlvideomode sdlvideomode[4096];

SDL_Window *globalwindow = NULL;
SDL_GLContext *glcontext = NULL;
SDL_Surface *windowicon = NULL;

void listvideomodes(void)
  {
    // https://wiki.libsdl.org/SDL_DisplayMode
    static int display_in_use = 0; /* Only using first display */

    int i, display_mode_count;
    SDL_DisplayMode mode;
    Uint32 f;
    numofsdlvideomodes=0;

    TO_DEBUG_LOG("SDL_GetNumVideoDisplays(): %i\n", SDL_GetNumVideoDisplays());

    display_mode_count = SDL_GetNumDisplayModes(display_in_use);
    if (display_mode_count < 1) {
        TO_DEBUG_LOG("SDL_GetNumDisplayModes failed: %s\n", SDL_GetError());
        sdlvideomode[numofsdlvideomodes].resolutionx=640;
        sdlvideomode[numofsdlvideomodes].resolutiony=480;
        sdlvideomode[numofsdlvideomodes].bitsperpixel=32;
        numofsdlvideomodes++;
        sdlvideomode[numofsdlvideomodes].resolutionx=800;
        sdlvideomode[numofsdlvideomodes].resolutiony=600;
        sdlvideomode[numofsdlvideomodes].bitsperpixel=32;
        numofsdlvideomodes++;
        sdlvideomode[numofsdlvideomodes].resolutionx=1024;
        sdlvideomode[numofsdlvideomodes].resolutiony=768;
        sdlvideomode[numofsdlvideomodes].bitsperpixel=32;
        numofsdlvideomodes++;
        return;
    }
    TO_DEBUG_LOG("SDL_GetNumDisplayModes: %i\n", display_mode_count);

    if (display_mode_count < 1) {

    }

    for (i = 0; i < display_mode_count && display_mode_count < 64; ++i) {
        if (SDL_GetDisplayMode(display_in_use, i, &mode) != 0) {
            TO_DEBUG_LOG("SDL_GetDisplayMode failed: %s\n", SDL_GetError());
            return;
        }
        f = mode.format;

        TO_DEBUG_LOG("Mode %i\tbpp %i\t%s\t%i x %i\n", i,
        SDL_BITSPERPIXEL(f), SDL_GetPixelFormatName(f), mode.w, mode.h);

        // if (SDL_VideoModeOK(sdlmode[count]->w,sdlmode[count]->h,32,SDL_OPENGL|SDL_FULLSCREEN)) {
            sdlvideomode[numofsdlvideomodes].resolutionx=mode.w;
            sdlvideomode[numofsdlvideomodes].resolutiony=mode.h;
            sdlvideomode[numofsdlvideomodes].bitsperpixel=32;
            numofsdlvideomodes++;
        // }
        // if (SDL_VideoModeOK(sdlmode[count]->w,sdlmode[count]->h,16,SDL_OPENGL|SDL_FULLSCREEN)) {
            sdlvideomode[numofsdlvideomodes].resolutionx=mode.w;
            sdlvideomode[numofsdlvideomodes].resolutiony=mode.h;
            sdlvideomode[numofsdlvideomodes].bitsperpixel=16;
            numofsdlvideomodes++;
        // }
    }
  }
