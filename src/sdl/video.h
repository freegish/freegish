#ifndef GISH_SDL_VIDEO_H
#define GISH_SDL_VIDEO_H
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

#include "../sdl/sdl.h"
#include "../sdl/video.h"

void listvideomodes(void);

typedef struct
  {
  int resolutionx;
  int resolutiony;
  int bitsperpixel;
  } _sdlvideomode;

extern int numofsdlvideomodes;
extern _sdlvideomode sdlvideomode[4096];

extern SDL_Window *globalwindow;
extern SDL_GLContext *glcontext;
extern SDL_Surface *windowicon;

#endif /* GISH_SDL_VIDEO_H */
