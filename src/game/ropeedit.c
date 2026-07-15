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

#include "../video/opengl.h"

#include "../sdl/sdl.h"

#include "../game/ropeedit.h"
#include "../game/english.h"
#include "../game/game.h"
#include "../game/gameobject.h"
#include "../game/level.h"
#include "../game/lighting.h"
#include "../game/mainmenu.h"
#include "../game/objedit.h"
#include "../game/render.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../math/vector.h"
#include "../menu/menu.h"
#include "../sdl/event.h"
#include "../video/glfunc.h"
#include "../video/text.h"
#include "../sdl/video.h"

_ropeedit ropeedit;

void renderropeedit(void)
  {
  int count;
  float vec[3];

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINES);

  glColor4f(0.0f,1.0f,0.0f,1.0f);

  for (count=0;count<ropeedit.numofpoints;count++)
    {
    if (count==ropeedit.pointnum)
      glColor4f(1.0f,1.0f,1.0f,1.0f);
    else if (count==ropeedit.pointhighlight)
      glColor4f(0.0f,1.0f,0.0f,1.0f);
    else
      glColor4f(0.0f,0.5f,0.0f,1.0f);

    vec[0]=ropeedit.point[count].position[0];
    vec[1]=ropeedit.point[count].position[1]+0.25f;
    vec[2]=0.0f;

    glVertex3fv(vec);

    vec[0]=ropeedit.point[count].position[0];
    vec[1]=ropeedit.point[count].position[1]-0.25f;
    vec[2]=0.0f;

    glVertex3fv(vec);

    vec[0]=ropeedit.point[count].position[0]-0.25f;
    vec[1]=ropeedit.point[count].position[1];
    vec[2]=0.0f;

    glVertex3fv(vec);

    vec[0]=ropeedit.point[count].position[0]+0.25f;
    vec[1]=ropeedit.point[count].position[1];
    vec[2]=0.0f;

    glVertex3fv(vec);
    }

  glEnd();

  glEnable(GL_TEXTURE_2D);
  }
