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

#include <math.h>

#include "../game/debug.h"
#include "../game/objedit.h"
#include "../game/animation.h"
#include "../game/editor.h"
#include "../game/gameobject.h"
#include "../game/english.h"
#include "../game/game.h"
#include "../game/level.h"
#include "../game/lighting.h"
#include "../game/mainmenu.h"
#include "../game/render.h"
#include "../game/setup.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../math/vector.h"
#include "../menu/menu.h"
#include "../sdl/event.h"
#include "../video/glfunc.h"
#include "../video/text.h"
#include "../video/texture.h"
#include "../sdl/video.h"

void renderlevelobjects(void)
  {
  int count,count2;
  int objectnum;
  float vec[3];
  float angle;

  for (count=0;count<level.numofobjects;count++)
    {
    if (level.object[count].type==LVL_OBJ_TYPE_GISH)
      {
      glDisable(GL_TEXTURE_2D);

      for (count2=0;count2<16;count2++)
        {
        glBegin(GL_TRIANGLES);

        glColor4f(0.0f,0.0f,0.0f,1.0f);

        angle=(float)count2*pi/8.0f;
        vec[0]=level.object[count].position[0]+cos(angle)*0.9f;
        vec[1]=level.object[count].position[1]-sin(angle)*0.9f;
        vec[2]=0.0f;
        glVertex3fv(vec);

        angle=(float)(count2+1)*pi/8.0f;
        vec[0]=level.object[count].position[0]+cos(angle)*0.9f;
        vec[1]=level.object[count].position[1]-sin(angle)*0.9f;
        vec[2]=0.0f;
        glVertex3fv(vec);

        glVertex3fv(level.object[count].position);

        glEnd();
        }

      glEnable(GL_TEXTURE_2D);
      }
    if (level.object[count].type>=LVL_OBJ_TYPE_BOX && level.object[count].type<LVL_OBJ_TYPE_WHEEL)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);

      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_WHEEL || level.object[count].type==LVL_OBJ_TYPE_ANCHORED_WHEEL || level.object[count].type==LVL_OBJ_TYPE_GENERATOR)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);

      for (count2=0;count2<16;count2++)
        {
        glBegin(GL_TRIANGLES);

        glColor4f(1.0f,1.0f,1.0f,1.0f);

        angle=(float)count2*pi/8.0f;
        vec[0]=level.object[count].position[0]+cos(angle)*level.object[count].size[0]*0.5f;
        vec[1]=level.object[count].position[1]-sin(angle)*level.object[count].size[1]*0.5f;
        vec[2]=0.0f;
        glTexCoord2f(0.5f+cos(angle)*0.5f,0.5f+sin(angle)*0.5f);
        glVertex3fv(vec);

        angle=(float)(count2+1)*pi/8.0f;
        vec[0]=level.object[count].position[0]+cos(angle)*level.object[count].size[0]*0.5f;
        vec[1]=level.object[count].position[1]-sin(angle)*level.object[count].size[1]*0.5f;
        vec[2]=0.0f;
        glTexCoord2f(0.5f+cos(angle)*0.5f,0.5f+sin(angle)*0.5f);
        glVertex3fv(vec);

        glTexCoord2f(0.5f,0.5f);
        glVertex3fv(level.object[count].position);

        glEnd();
        }
      }
    if (level.object[count].type==LVL_OBJ_TYPE_LIGHT_OR_ANCHOR)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-0.25f,level.object[count].position[1]+0.25f,0.0f);

      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+0.25f,level.object[count].position[1]+0.25f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+0.25f,level.object[count].position[1]-0.25f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-0.25f,level.object[count].position[1]-0.25f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_BUTTON || level.object[count].type==LVL_OBJ_TYPE_ONE_TIME_BUTTON)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-0.5f,level.object[count].position[1]+0.5f,0.0f);

      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+0.5f,level.object[count].position[1]+0.5f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+0.5f,level.object[count].position[1],0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-0.5f,level.object[count].position[1],0.0f);

      glEnd();
      }
    if (level.object[count].type>=LVL_OBJ_TYPE_MONSTER_BEGIN && level.object[count].type<40) // why 40 when monsters end at 36?
      {
      glBindTexture(GL_TEXTURE_2D,texture[animation[level.object[count].type-20].stand[0]].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);

      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_SWITCH_UP)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-0.125f,level.object[count].position[1]+0.5f,0.0f);
                                                   
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+0.125f,level.object[count].position[1]+0.5f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+0.25f,level.object[count].position[1]-0.5f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-0.25f,level.object[count].position[1]-0.5f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_SWITCH_RIGHT)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-0.5f,level.object[count].position[1]+0.25f,0.0f);
                                                   
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+0.5f,level.object[count].position[1]+0.125f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+0.5f,level.object[count].position[1]-0.125f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-0.5f,level.object[count].position[1]-0.25f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_SWITCH_DOWN)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-0.25f,level.object[count].position[1]+0.5f,0.0f);
                                                   
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+0.25f,level.object[count].position[1]+0.5f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+0.125f,level.object[count].position[1]-0.5f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-0.125f,level.object[count].position[1]-0.5f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_SWITCH_LEFT)
      {
      glBindTexture(GL_TEXTURE_2D,texture[level.object[count].texturenum+256].glname);
  
      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glTexCoord2f(0.0f,0.0f);
      glVertex3f(level.object[count].position[0]-0.5f,level.object[count].position[1]+0.125f,0.0f);
                                                   
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(level.object[count].position[0]+0.5f,level.object[count].position[1]+0.25f,0.0f);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(level.object[count].position[0]+0.5f,level.object[count].position[1]-0.25f,0.0f);

      glTexCoord2f(0.0f,1.0f);
      glVertex3f(level.object[count].position[0]-0.5f,level.object[count].position[1]-0.125f,0.0f);

      glEnd();
      }
    if (level.object[count].type==LVL_OBJ_TYPE_AREASWITCH || level.object[count].type==LVL_OBJ_TYPE_ONE_TIME_AREASWITCH || level.object[count].type==LVL_OBJ_TYPE_SECRET_AREASWITCH)
      {
      glDisable(GL_TEXTURE_2D);
  
      glBegin(GL_LINES);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);
      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);

      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);
      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);

      glVertex3f(level.object[count].position[0]+level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);
      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);

      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]-level.object[count].size[1]*0.5f,0.0f);
      glVertex3f(level.object[count].position[0]-level.object[count].size[0]*0.5f,level.object[count].position[1]+level.object[count].size[1]*0.5f,0.0f);

      glEnd();

      glEnable(GL_TEXTURE_2D);
      }
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);

    glColor4f(0.5f,0.5f,0.5f,1.0f);
    if (count==editor.objectnum)
      glColor4f(0.0f,1.0f,0.0f,1.0f);
    else if (editor.objectnum!=-1)
      {
      if (count==level.object[editor.objectnum].link)
        glColor4f(1.0f,0.0f,0.0f,1.0f);
      }

    vec[0]=level.object[count].position[0]-0.5f;
    vec[1]=level.object[count].position[1]+0.5f;
    vec[2]=0.0f;
    glVertex3fv(vec);

    vec[0]=level.object[count].position[0]+0.5f;
    vec[1]=level.object[count].position[1]-0.5f;
    vec[2]=0.0f;
    glVertex3fv(vec);

    vec[0]=level.object[count].position[0]+0.5f;
    vec[1]=level.object[count].position[1]+0.5f;
    vec[2]=0.0f;
    glVertex3fv(vec);

    vec[0]=level.object[count].position[0]-0.5f;
    vec[1]=level.object[count].position[1]-0.5f;
    vec[2]=0.0f;
    glVertex3fv(vec);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    }

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINES);

  for (count=0;count<level.numofropes;count++)
  if (level.rope[count].obj1!=-1 && level.rope[count].obj2!=-1)
    {
    if (level.rope[count].type==WEAK_ROPE)
      glColor4f(0.75f,0.75f,0.0f,1.0f);
    if (level.rope[count].type==STRONG_ROPE)
      glColor4f(1.0f,1.0f,0.0f,1.0f);
    if (level.rope[count].type==WEAK_CHAIN)
      glColor4f(0.5f,0.5f,0.5f,1.0f);
    if (level.rope[count].type==STRONG_CHAIN)
      glColor4f(0.75f,0.75f,0.75f,1.0f);
    if (level.rope[count].type>=PUSHING_PISTON && level.rope[count].type<10)
      glColor4f(0.75f,0.0f,0.75f,1.0f);
    if (level.rope[count].type==SPRING)
      glColor4f(0.0f,0.75f,0.75f,1.0f);

    objectnum=level.rope[count].obj1;
    copyvector(vec,level.object[objectnum].position);
    if (level.object[objectnum].type>=LVL_OBJ_TYPE_BOX && level.object[objectnum].type<LVL_OBJ_TYPE_WHEEL)
      {
      if (level.rope[count].obj1part==0)
        {
        vec[0]-=level.object[objectnum].size[0]*0.5f;
        vec[1]+=level.object[objectnum].size[1]*0.5f;
        }
      if (level.rope[count].obj1part==1)
        {
        vec[0]+=level.object[objectnum].size[0]*0.5f;
        vec[1]+=level.object[objectnum].size[1]*0.5f;
        }
      if (level.rope[count].obj1part==2)
        {
        vec[0]+=level.object[objectnum].size[0]*0.5f;
        vec[1]-=level.object[objectnum].size[1]*0.5f;
        }
      if (level.rope[count].obj1part==3)
        {
        vec[0]-=level.object[objectnum].size[0]*0.5f;
        vec[1]-=level.object[objectnum].size[1]*0.5f;
        }
      }
    if (level.object[objectnum].type>=LVL_OBJ_TYPE_WHEEL && level.object[objectnum].type<=LVL_OBJ_TYPE_ANCHORED_WHEEL)
      {
      if (level.rope[count].obj1part==0)
        vec[0]+=level.object[objectnum].size[0]*0.5f;
      if (level.rope[count].obj1part==4)
        vec[1]-=level.object[objectnum].size[1]*0.5f;
      if (level.rope[count].obj1part==8)
        vec[0]-=level.object[objectnum].size[0]*0.5f;
      if (level.rope[count].obj1part==12)
        vec[1]+=level.object[objectnum].size[1]*0.5f;
      }
    glVertex3fv(vec);

    objectnum=level.rope[count].obj2;
    copyvector(vec,level.object[objectnum].position);
    if (level.object[objectnum].type>=LVL_OBJ_TYPE_BOX && level.object[objectnum].type<LVL_OBJ_TYPE_WHEEL)
      {
      if (level.rope[count].obj2part==0)
        {
        vec[0]-=level.object[objectnum].size[0]*0.5f;
        vec[1]+=level.object[objectnum].size[1]*0.5f;
        }
      if (level.rope[count].obj2part==1)
        {
        vec[0]+=level.object[objectnum].size[0]*0.5f;
        vec[1]+=level.object[objectnum].size[1]*0.5f;
        }
      if (level.rope[count].obj2part==2)
        {
        vec[0]+=level.object[objectnum].size[0]*0.5f;
        vec[1]-=level.object[objectnum].size[1]*0.5f;
        }
      if (level.rope[count].obj2part==3)
        {
        vec[0]-=level.object[objectnum].size[0]*0.5f;
        vec[1]-=level.object[objectnum].size[1]*0.5f;
        }
      }
    if (level.object[objectnum].type>=LVL_OBJ_TYPE_WHEEL && level.object[objectnum].type<=LVL_OBJ_TYPE_ANCHORED_WHEEL)
      {
      if (level.rope[count].obj2part==0)
        vec[0]+=level.object[objectnum].size[0]*0.5f;
      if (level.rope[count].obj2part==4)
        vec[1]-=level.object[objectnum].size[1]*0.5f;
      if (level.rope[count].obj2part==8)
        vec[0]-=level.object[objectnum].size[0]*0.5f;
      if (level.rope[count].obj2part==12)
        vec[1]+=level.object[objectnum].size[1]*0.5f;
      }
    glVertex3fv(vec);
    }

  glEnd();

  glEnable(GL_TEXTURE_2D);
  }

void deletelevelobject(int objectnum)
  {
  int count;

  if (objectnum<0)
    return;
  if (objectnum>=level.numofobjects)
    return;

  if (editor.objectnum==objectnum)
    editor.objectnum=-1;

  for (count=0;count<level.numofropes;count++)
    {
    while (count<level.numofropes && (level.rope[count].obj1==objectnum || level.rope[count].obj2==objectnum))
      deletelevelrope(count);
    }
  for (count=0;count<level.numofobjects;count++)
    if (level.object[count].link==objectnum)
      level.object[count].link=-1;

  level.numofobjects--;

  if (objectnum==level.numofobjects)
    return;

  memcpy(&level.object[objectnum],&level.object[level.numofobjects],sizeof(level.object[objectnum]));

  if (editor.objectnum==level.numofobjects)
    editor.objectnum=objectnum;

  for (count=0;count<level.numofropes;count++)
    {
    if (level.rope[count].obj1==level.numofobjects)
      level.rope[count].obj1=objectnum;
    if (level.rope[count].obj2==level.numofobjects)
      level.rope[count].obj2=objectnum;
    }
  for (count=0;count<level.numofobjects;count++)
    if (level.object[count].link==level.numofobjects)
      level.object[count].link=objectnum;
  }

void deletelevelrope(int ropenum)
  {
  if (ropenum<0)
    return;
  if (ropenum>=level.numofropes)
    return;

  level.numofropes--;

  if (ropenum==level.numofropes)
    return;

  memcpy(&level.rope[ropenum],&level.rope[level.numofropes],sizeof(level.rope[ropenum]));
  }
