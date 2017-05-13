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

#include "config.h"

#include "sdl/sdl.h"

#include "video/opengl.h"

#include "audio/audio.h"
#include "game/config.h"
#include "game/high.h"
#include "game/mainmenu.h"
#include "game/player.h"
#include "game/options.h"
#include "input/joystick.h"
#include "menu/menu.h"
#include "sdl/endian.h"
#include "sdl/event.h"
#include "sdl/video.h"
#include "video/text.h"
#include "video/texture.h"

#ifdef DATAPATH
#include <unistd.h>
#endif

int main (int argc,char *argv[])
  {
  int count;
  int flags;
  const char *temp;

#ifdef DATAPATH
  chdir(DATAPATH);
#endif

  checkbigendian();

  loadconfig();
  loadscores();
  loadplayers();

  flags=SDL_INIT_VIDEO|SDL_INIT_TIMER;
  if (config.joystick)
    flags|=SDL_INIT_JOYSTICK;

  if (SDL_Init(flags) < 0)
    {
    fprintf(stderr, "Failed to initialize SDL:\n%s\n",SDL_GetError());
    return 1;
    }

  int display_count = 0, display_index = 0, mode_index = 0;
  SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

  if ((display_count = SDL_GetNumVideoDisplays()) < 1)
    {
    TO_DEBUG_LOG("SDL_GetNumVideoDisplays returned: %i\n", display_count);
    return 1;
    }

  if (SDL_GetDisplayMode(display_index, mode_index, &mode) != 0)
    {
    TO_DEBUG_LOG("SDL_GetDisplayMode failed: %s\n", SDL_GetError());
    return 1;
    }

  TO_DEBUG_LOG("SDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i\n", SDL_BITSPERPIXEL(mode.format), mode.w, mode.h);

  if (SDL_BITSPERPIXEL(mode.format)==16)
    config.bitsperpixel=16;

  for (count=1;count<argc;count++)
    {
    if (strcmp("-nosound",argv[count])==0)
      {
      config.sound=0;
      option.sound=0;
      option.music=0;
      }
    if (strcmp("-sound",argv[count])==0)
      config.sound=1;
    if (strcmp("-nomusic",argv[count])==0)
      option.music=0;
    }

  saveconfig();

  SDL_ShowCursor(SDL_DISABLE);

  listvideomodes();

  if (windowinfo.bitsperpixel==16)
    {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);
    }
  if (windowinfo.bitsperpixel==32)
    {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    }
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,windowinfo.depthbits);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,windowinfo.stencilbits);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

    globalwindow = SDL_CreateWindow("Gish", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    windowinfo.resolutionx, windowinfo.resolutiony,
                                    (windowinfo.fullscreen) ? SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN : SDL_WINDOW_OPENGL);
    SDL_SetWindowDisplayMode(globalwindow, &mode);

    if(globalwindow == NULL)
    {
    fprintf(stderr, "Failed to initialize video:\n%s\n",SDL_GetError());
    return 1;
    }

    windowicon = SDL_LoadBMP("freegish.bmp");
    SDL_SetColorKey(windowicon, SDL_TRUE, SDL_MapRGB(windowicon->format, 255, 255, 255));
    SDL_SetWindowIcon(globalwindow, windowicon);

    glcontext = SDL_GL_CreateContext(globalwindow);

  loadglextentions();

  for (count=0;count<2048;count++)
    glGenTextures(1,&texture[count].glname);

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  if (config.joystick)
    {
    numofjoysticks=SDL_NumJoysticks();
    for (count=0;count<numofjoysticks;count++)
      {
      joy[count]=SDL_JoystickOpen(count);
      temp=SDL_JoystickName(joy[count]);
      strcpy(joystick[count].name,temp);
      joystick[count].numofbuttons=SDL_JoystickNumButtons(joy[count]);
      joystick[count].numofhats=SDL_JoystickNumHats(joy[count]);
      }

    SDL_JoystickEventState(SDL_IGNORE);
    }

  font.texturenum=0;
  font.cursornum=0;
  font.sizex=640;
  font.sizey=480;

  loadtexture(1000,"font00.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);
  loadtexture(1001,"font01.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);

  loadtexture(768,"mouse00.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);
  loadtexture(769,"mouse00.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);

  setupmenuitems();

  if (!glext.multitexture)
    {
    notsupportedmenu();

    SDL_MinimizeWindow(globalwindow);
    SDL_Quit();
    return(0);
    }
    
  if (config.sound)
    setupaudio();

  mainmenu();

  saveconfig();
  savescores();
  saveplayers();

  if (config.sound)
    shutdownaudio();

  SDL_MinimizeWindow(globalwindow);
  SDL_Quit();

  return(0);
  }
