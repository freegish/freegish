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

#include "../game/editor.h"
#include "../game/debug.h"
#include "../game/block.h"
#include "../game/english.h"
#include "../game/game.h"
#include "../game/level.h"
#include "../game/lighting.h"
#include "../game/mainmenu.h"
#include "../game/objedit.h"
#include "../game/prerender.h"
#include "../game/render.h"
#include "../game/ropeedit.h"
#include "../game/gameobject.h"
#include "../game/setup.h"
#include "../game/gametexture.h"
#include "../physics/particle.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../math/vector.h"
#include "../menu/menu.h"
#include "../sdl/endian.h"
#include "../sdl/event.h"
#include "../video/glfunc.h"
#include "../video/text.h"
#include "../video/texture.h"
#include "../sdl/video.h"

_editor editor;
char currentTextureFilename[256];
int menu_last_loadlevel = -1;
int editor_mode = EDITOR_MODE_TILES;

void process_events_tiles(){
    float vec[3], vec2[3];
    int x,y;
    int count, count2;
    if (!menuinputkeyboard && !menuinputmouse)
      {
      get_mouse_coords(&vec[0], &vec[1]);
      x = (int)vec[0];
      y = (int)vec[1];

      if (!editor.paste)
        {
        if (!keyboard[SCAN_SHIFT])
          {
          if (mouse.lmb){
            setblock(x,y,editor.blocknum);
            menu_last_loadlevel = -1;
          }
          if (mouse.rmb && (editor.editstart[0]==0 && editor.editstart[1]==0)){
            setblock(x,y,0);
            menu_last_loadlevel = -1;
          }
          }
        else
          {
          if (mouse.lmb && !prevmouse.lmb)
            {
            editor.editstart[0]=x;
            editor.editstart[1]=y;
            }
          if (mouse.lmb && (editor.editstart[0]!=0 || editor.editstart[1]!=0))
            {
            if (editor.editstart[0]<x)
              {
              editor.editarea[0][0]=editor.editstart[0];
              editor.editarea[1][0]=x;
              }
            else
              {
              editor.editarea[0][0]=x;
              editor.editarea[1][0]=editor.editstart[0];
              }
            if (editor.editstart[1]<y)
              {
              editor.editarea[0][1]=editor.editstart[1];
              editor.editarea[1][1]=y;
              }
            else
              {
              editor.editarea[0][1]=y;
              editor.editarea[1][1]=editor.editstart[1];
              }
            }
          }
        if (editor.editstart[0]!=0 || editor.editstart[1]!=0)
          {
          count=-1;
          // level.area reference is at game/level.h
          // basically, 0 is win, 1 is warpzone, the rest is for tutorial text or vs gamemodes
          if (keyboard[SCAN_5])
            count=0;
          if (keyboard[SCAN_6])
            count=1;
          if (keyboard[SCAN_7])
            count=2;
          if (keyboard[SCAN_8])
            count=3;
          if (count!=-1)
            {
            if (keyboard[SCAN_SHIFT])
              count+=4;

            level.area[count][0]=editor.editarea[0][0];
            level.area[count][1]=editor.editarea[0][1];
            level.area[count][2]=editor.editarea[1][0]+1.0f;
            level.area[count][3]=editor.editarea[1][1]+1.0f;
            }
          }
        if (keyboard[SCAN_ESC] || (!mouse.rmb && prevmouse.rmb))
          {
          editor.editstart[0]=0;
          editor.editstart[1]=0;
          editor.editarea[0][0]=0;
          editor.editarea[0][1]=0;
          editor.editarea[1][0]=0;
          editor.editarea[1][1]=0;
          }
        if (keyboard[SCAN_C] && !prevkeyboard[SCAN_C])
          {
          editor.copysize[0]=editor.editarea[1][0]-editor.editarea[0][0];
          editor.copysize[1]=editor.editarea[1][1]-editor.editarea[0][1];
          for (count=0;count<=editor.copysize[1];count++)
          for (count2=0;count2<=editor.copysize[0];count2++)
            editor.copybuffer[count][count2]=getblock(editor.editarea[0][0]+count2,editor.editarea[0][1]+count);

          editor.paste=1;
          editor.editstart[0]=0;
          editor.editstart[1]=0;
          editor.editarea[0][0]=0;
          editor.editarea[0][1]=0;
          editor.editarea[1][0]=0;
          editor.editarea[1][1]=0;
          }
        if (keyboard[SCAN_DELETE] && !prevkeyboard[SCAN_DELETE])
          {
          editor.copysize[0]=editor.editarea[1][0]-editor.editarea[0][0];
          editor.copysize[1]=editor.editarea[1][1]-editor.editarea[0][1];
          for (count=0;count<=editor.copysize[1];count++)
          for (count2=0;count2<=editor.copysize[0];count2++)
            setblock(editor.editarea[0][0]+count2,editor.editarea[0][1]+count,0);
          }
        if (keyboard[SCAN_B] && !prevkeyboard[SCAN_B])
          {
          editor.copysize[0]=editor.editarea[1][0]-editor.editarea[0][0];
          editor.copysize[1]=editor.editarea[1][1]-editor.editarea[0][1];
          for (count=0;count<=editor.copysize[1];count++)
          for (count2=0;count2<=editor.copysize[0];count2++)
            setblock(editor.editarea[0][0]+count2,editor.editarea[0][1]+count,editor.blocknum);
          }
        if (keyboard[SCAN_V] && !prevkeyboard[SCAN_V])
          editor.paste=1;
        if (keyboard[SCAN_E] && !prevkeyboard[SCAN_E])
          editor.blocknum = getblock(x, y);
        }
      else
        {
        if (mouse.lmb && !prevmouse.lmb)
          {
          for (count=0;count<=editor.copysize[1];count++)
          for (count2=0;count2<=editor.copysize[0];count2++)
            setblock(x+count2,y+count,editor.copybuffer[count][count2]);
          }
        if (keyboard[SCAN_ESC] || (!mouse.rmb && prevmouse.rmb))
          editor.paste=0;
        }
      if (keyboard[SCAN_G])
        editor.blocknum=getblock(x,y);
      // set first player position to mouse coords
      if (keyboard[SCAN_P] && !prevkeyboard[SCAN_P]){
        if (object[0].type == OBJ_TYPE_GISH){
          get_mouse_coords(&vec[0], &vec[1]);
          vec[2] = 0.0f;
          subtractvectors(vec2, vec, object[0].position);
          for (count = 0; count < 16; count++){
            particle[object[0].particle[count]].position[0] += vec2[0];
            particle[object[0].particle[count]].position[1] += vec2[1];
          }
          object[0].position[0] = vec[0];
          object[0].position[1] = vec[1];
        }
      }
      if (keyboard[SCAN_Q] && !prevkeyboard[SCAN_Q])
        {
        if (!keyboard[SCAN_SHIFT])
          editor.blocknum++;
        else
          editor.blocknum+=10;
        if (editor.blocknum>255)
          editor.blocknum=255;
        }
      if (keyboard[SCAN_Z] && !prevkeyboard[SCAN_Z])
        {
        if (!keyboard[SCAN_SHIFT])
          editor.blocknum--;
        else
          editor.blocknum-=10;
        if (editor.blocknum<0)
          editor.blocknum=0;
        }

      if (keyboard[SCAN_LFT_BRACKET] && !prevkeyboard[SCAN_LFT_BRACKET])
      if (level.tileset>0)
        level.tileset--;
      if (keyboard[SCAN_RGT_BRACKET] && !prevkeyboard[SCAN_RGT_BRACKET])
      if (level.tileset<7)
        level.tileset++;
      }
}

void process_events_objects(){
  float vec[3], vec2[3];
  int x,y;
  int count, count2;
  if (!menuinputkeyboard && !menuinputmouse)
    {
      get_mouse_coords(&vec[0], &vec[1]);
      vec[2] = 0.0f;
      x = (int)vec[0];
      y = (int)vec[1];
      if (!keyboard[SCAN_K] && mouse.lmb && !prevmouse.lmb){
        menu_last_loadlevel = -1;
        if (!keyboard[SCAN_H]){
          vec[0]=(float)x+0.5f;
          vec[1]=(float)y+0.5f;
          vec[2]=0.0f;
        } else {
          vec[0]=(float)x;
          vec[1]=(float)y;
          vec[2]=0.0f;
        }

          memset(&level.object[level.numofobjects],0,sizeof(level.object[level.numofobjects]));
          level.object[level.numofobjects].type=editor.objecttype;
          level.object[level.numofobjects].link=-1;
          copyvector(level.object[level.numofobjects].position,vec);
          if (editor.objectnum==-1 || level.object[level.numofobjects].type!=level.object[editor.objectnum].type)
            {
            level.object[level.numofobjects].texturenum=0;
            level.object[level.numofobjects].size[0]=1.0f;
            level.object[level.numofobjects].size[1]=1.0f;
            level.object[level.numofobjects].mass=1.0f;
            level.object[level.numofobjects].friction=0.8f;
            level.object[level.numofobjects].lightcolor[0]=1.0f;
            level.object[level.numofobjects].lightcolor[1]=1.0f;
            level.object[level.numofobjects].lightcolor[2]=1.0f;
            level.object[level.numofobjects].lightintensity=16.0f;
            }
          else
            {
            level.object[level.numofobjects].texturenum=level.object[editor.objectnum].texturenum;
            level.object[level.numofobjects].size[0]=level.object[editor.objectnum].size[0];
            level.object[level.numofobjects].size[1]=level.object[editor.objectnum].size[1];
            level.object[level.numofobjects].mass=level.object[editor.objectnum].mass;
            level.object[level.numofobjects].friction=level.object[editor.objectnum].friction;
            level.object[level.numofobjects].lighttype=level.object[editor.objectnum].lighttype;
            level.object[level.numofobjects].lightcolor[0]=level.object[editor.objectnum].lightcolor[0];
            level.object[level.numofobjects].lightcolor[1]=level.object[editor.objectnum].lightcolor[1];
            level.object[level.numofobjects].lightcolor[2]=level.object[editor.objectnum].lightcolor[2];
            level.object[level.numofobjects].lightintensity=level.object[editor.objectnum].lightintensity;
            }

          editor.objectnum=level.numofobjects;
          level.numofobjects++;
        }
      if (keyboard[SCAN_K] && editor.objectnum!=-1){
        if (mouse.lmb && !prevmouse.lmb){
          for (count=0;count<level.numofobjects;count++){
            subtractvectors(vec2,vec,level.object[count].position);
            if (vectorlength(vec2)<0.5f)
              level.object[editor.objectnum].link=count;
          }
        }
        if (mouse.rmb && !prevmouse.rmb)
          level.object[editor.objectnum].link=-1;
      }
      if (mouse.rmb && !prevmouse.rmb)
        {
        menu_last_loadlevel = -1;
        editor.objectnum=-1;

        for (count=0;count<level.numofobjects;count++)
          {
          subtractvectors(vec2,vec,level.object[count].position);
          if (vectorlength(vec2)<0.5f)
            editor.objectnum=count;
          }
        }
      if (keyboard[SCAN_E] && !prevkeyboard[SCAN_E]){
        editor.objectnum=-1;

        for (count=0;count<level.numofobjects;count++){
          subtractvectors(vec2,vec,level.object[count].position);
          if (vectorlength(vec2)<0.5f){
            editor.objectnum=count;
            editor.objecttype=level.object[count].type;
            break;
          }
        }
      }
      if (editor.objectnum!=-1){
        if (keyboard[SCAN_P] && !prevkeyboard[SCAN_P]){
          if (!keyboard[SCAN_H]){
            vec[0]=(float)x+0.5f;
            vec[1]=(float)y+0.5f;
            vec[2]=0.0f;
          } else {
            vec[0]=(float)x;
            vec[1]=(float)y;
            vec[2]=0.0f;
          }
          copyvector(level.object[editor.objectnum].position,vec);
          }
        // paste settings from another object
        if (keyboard[SCAN_V] && !prevkeyboard[SCAN_V]){
          for (count=0;count<level.numofobjects;count++){
            if (level.object[count].type == level.object[editor.objectnum].type){
              subtractvectors(vec2,vec,level.object[count].position);
              if (vectorlength(vec2)<0.5f){
                level.object[count].texturenum=level.object[editor.objectnum].texturenum;
                level.object[count].size[0]=level.object[editor.objectnum].size[0];
                level.object[count].size[1]=level.object[editor.objectnum].size[1];
                level.object[count].mass=level.object[editor.objectnum].mass;
                level.object[count].friction=level.object[editor.objectnum].friction;
                level.object[count].lighttype=level.object[editor.objectnum].lighttype;
                level.object[count].lightcolor[0]=level.object[editor.objectnum].lightcolor[0];
                level.object[count].lightcolor[1]=level.object[editor.objectnum].lightcolor[1];
                level.object[count].lightcolor[2]=level.object[editor.objectnum].lightcolor[2];
                level.object[count].lightintensity=level.object[editor.objectnum].lightintensity;
                editor.objectnum=count;
                break;
              }
            }
          }
        }
        if (keyboard[SCAN_HOME] && !prevkeyboard[SCAN_HOME])
        if (level.numofobjects>1)
          {
          count2=0;
          if (keyboard[SCAN_SHIFT])
            count2=2;
          memcpy(&level.object[255],&level.object[count2],sizeof(level.object[0]));
          memcpy(&level.object[count2],&level.object[editor.objectnum],sizeof(level.object[0]));
          memcpy(&level.object[editor.objectnum],&level.object[255],sizeof(level.object[0]));

          for (count=0;count<level.numofobjects;count++)
            {
            if (level.object[count].link==count2)
              level.object[count].link=editor.objectnum;
            else if (level.object[count].link==editor.objectnum)
              level.object[count].link=count2;
            }

          for (count=0;count<level.numofropes;count++)
            {
            if (level.rope[count].obj1==count2)
              level.rope[count].obj1=editor.objectnum;
            else if (level.rope[count].obj1==editor.objectnum)
              level.rope[count].obj1=count2;
            if (level.rope[count].obj2==count2)
              level.rope[count].obj2=editor.objectnum;
            else if (level.rope[count].obj2==editor.objectnum)
              level.rope[count].obj2=count2;
            }
          editor.objectnum=count2;
          }
        if (keyboard[SCAN_END] && !prevkeyboard[SCAN_END])
        if (level.numofobjects>2)
          {
          count2=1;
          if (keyboard[SCAN_SHIFT])
            count2=3;
          memcpy(&level.object[255],&level.object[count2],sizeof(level.object[0]));
          memcpy(&level.object[count2],&level.object[editor.objectnum],sizeof(level.object[0]));
          memcpy(&level.object[editor.objectnum],&level.object[255],sizeof(level.object[0]));

          for (count=0;count<level.numofobjects;count++)
            {
            if (level.object[count].link==count2)
              level.object[count].link=editor.objectnum;
            else if (level.object[count].link==editor.objectnum)
              level.object[count].link=count2;
            }

          for (count=0;count<level.numofropes;count++)
            {
            if (level.rope[count].obj1==count2)
              level.rope[count].obj1=editor.objectnum;
            else if (level.rope[count].obj1==editor.objectnum)
              level.rope[count].obj1=count2;
            if (level.rope[count].obj2==count2)
              level.rope[count].obj2=editor.objectnum;
            else if (level.rope[count].obj2==editor.objectnum)
              level.rope[count].obj2=count2;
            }
          editor.objectnum=count2;
          }

        vec[0]=1.0f;
        if (level.object[editor.objectnum].type==LVL_OBJ_TYPE_WHEEL || level.object[editor.objectnum].type==LVL_OBJ_TYPE_ANCHORED_WHEEL || level.object[editor.objectnum].type==LVL_OBJ_TYPE_GENERATOR)
          vec[0]=0.2f;
        if (keyboard[SCAN_LEFT] && !prevkeyboard[SCAN_LEFT])
        if (level.object[editor.objectnum].size[0]>vec[0])
          level.object[editor.objectnum].size[0]-=vec[0];

        if (keyboard[SCAN_RIGHT] && !prevkeyboard[SCAN_RIGHT])
        if (level.object[editor.objectnum].size[0]<16.0f)
          level.object[editor.objectnum].size[0]+=vec[0];

        if (keyboard[SCAN_DOWN] && !prevkeyboard[SCAN_DOWN])
        if (level.object[editor.objectnum].size[1]>vec[0])
          level.object[editor.objectnum].size[1]-=vec[0];

        if (keyboard[SCAN_UP] && !prevkeyboard[SCAN_UP])
        if (level.object[editor.objectnum].size[1]<16.0f)
          level.object[editor.objectnum].size[1]+=vec[0];

        if (keyboard[SCAN_LFT_BRACKET] && !prevkeyboard[SCAN_LFT_BRACKET])
          {
          if (!keyboard[SCAN_SHIFT])
            level.object[editor.objectnum].texturenum--;
          else
            level.object[editor.objectnum].texturenum-=10;
          if (level.object[editor.objectnum].texturenum<0)
            level.object[editor.objectnum].texturenum=0;
          }
        if (keyboard[SCAN_RGT_BRACKET] && !prevkeyboard[SCAN_RGT_BRACKET])
          {
          if (!keyboard[SCAN_SHIFT])
            level.object[editor.objectnum].texturenum++;
          else
            level.object[editor.objectnum].texturenum+=10;
          if (level.object[editor.objectnum].texturenum>255)
            level.object[editor.objectnum].texturenum=255;
          }
        }

      if (keyboard[SCAN_Q] && !prevkeyboard[SCAN_Q])
        {
        if (!keyboard[SCAN_SHIFT])
          editor.objecttype++;
        else
          editor.objecttype+=10;
        if (editor.objecttype>255)
          editor.objecttype=255;
        }
      if (keyboard[SCAN_Z] && !prevkeyboard[SCAN_Z])
        {
        if (!keyboard[SCAN_SHIFT])
          editor.objecttype--;
        else
          editor.objecttype-=10;
        if (editor.objecttype<0)
          editor.objecttype=0;
        }
      if (keyboard[SCAN_DELETE] && !prevkeyboard[SCAN_DELETE])
        deletelevelobject(editor.objectnum);
  }
}

void setup_rope_edit(){
        float vec[3], vec2[3];
        int x,y;
        int count, count2;

        ropeedit.numofpoints=0;
        for (count=0;count<level.numofobjects;count++)
          {
          if (level.object[count].type>=LVL_OBJ_TYPE_BOX && level.object[count].type<LVL_OBJ_TYPE_WHEEL)
            {
            for (count2=0;count2<4;count2++)
              {
              if (count2==0 || count2==3)
                vec[0]=level.object[count].position[0]-level.object[count].size[0]*0.5f;
              else
                vec[0]=level.object[count].position[0]+level.object[count].size[0]*0.5f;
              if (count2<2)
                vec[1]=level.object[count].position[1]+level.object[count].size[1]*0.5f;
              else
                vec[1]=level.object[count].position[1]-level.object[count].size[1]*0.5f;
              vec[2]=0.0f;
              copyvector(ropeedit.point[ropeedit.numofpoints].position,vec);
              ropeedit.point[ropeedit.numofpoints].objectnum=count;
              ropeedit.point[ropeedit.numofpoints].particlenum=count2;
              ropeedit.numofpoints++;

              copyvector(level.object[count].vertex[count2],vec);
              }
            }
          if (level.object[count].type>=LVL_OBJ_TYPE_WHEEL && level.object[count].type<=LVL_OBJ_TYPE_ANCHORED_WHEEL)
            {
            for (count2=0;count2<4;count2++)
              {
              vec[0]=level.object[count].position[0];
              vec[1]=level.object[count].position[1];
              if (count2==0)
                vec[0]=level.object[count].position[0]+level.object[count].size[0]*0.5f;
              if (count2==1)
                vec[1]=level.object[count].position[1]-level.object[count].size[1]*0.5f;
              if (count2==2)
                vec[0]=level.object[count].position[0]-level.object[count].size[0]*0.5f;
              if (count2==3)
                vec[1]=level.object[count].position[1]+level.object[count].size[1]*0.5f;
              vec[2]=0.0f;
              copyvector(ropeedit.point[ropeedit.numofpoints].position,vec);
              ropeedit.point[ropeedit.numofpoints].objectnum=count;
              ropeedit.point[ropeedit.numofpoints].particlenum=count2*4;
              ropeedit.numofpoints++;

              copyvector(level.object[count].vertex[count2],vec);
              }

            count2=4;
            vec[0]=level.object[count].position[0];
            vec[1]=level.object[count].position[1];
            vec[2]=0.0f;

            copyvector(ropeedit.point[ropeedit.numofpoints].position,vec);
            ropeedit.point[ropeedit.numofpoints].objectnum=count;
            ropeedit.point[ropeedit.numofpoints].particlenum=16;
            ropeedit.numofpoints++;

            copyvector(level.object[count].vertex[count2],vec);
            }
          if (level.object[count].type==LVL_OBJ_TYPE_LIGHT_OR_ANCHOR)
            {
            vec[0]=level.object[count].position[0];
            vec[1]=level.object[count].position[1];
            vec[2]=0.0f;
            copyvector(ropeedit.point[ropeedit.numofpoints].position,vec);
            ropeedit.point[ropeedit.numofpoints].objectnum=count;
            ropeedit.point[ropeedit.numofpoints].particlenum=0;
            ropeedit.numofpoints++;
            copyvector(level.object[count].vertex[0],vec);
            }
          }
        ropeedit.pointnum=-1;
}

void rope_edit_button_function(){
  setup_rope_edit();
  menuset();
}

void process_events_ropes(){
    float vec[3], vec2[3];
    int count;
    get_mouse_coords(&vec[0], &vec[1]);
    vec[2]=0.0f;

    ropeedit.pointhighlight=-1;
    for (count=0;count<ropeedit.numofpoints;count++)
      {
      subtractvectors(vec2,ropeedit.point[count].position,vec);
      if (vectorlength(vec2)<0.25f)
        ropeedit.pointhighlight=count;
      }

    if (mouse.lmb && !prevmouse.lmb){
      if (ropeedit.pointnum==-1)
        {
        if (ropeedit.pointhighlight!=-1)
          ropeedit.pointnum=ropeedit.pointhighlight;
        }
      else
        {

          {
          if (ropeedit.pointhighlight!=-1)
          if (ropeedit.point[ropeedit.pointnum].objectnum!=ropeedit.point[ropeedit.pointhighlight].objectnum)
            {
            level.rope[level.numofropes].type=ropeedit.ropetype;
            level.rope[level.numofropes].obj1=ropeedit.point[ropeedit.pointnum].objectnum;
            level.rope[level.numofropes].obj1part=ropeedit.point[ropeedit.pointnum].particlenum;

            level.rope[level.numofropes].obj2=ropeedit.point[ropeedit.pointhighlight].objectnum;
            level.rope[level.numofropes].obj2part=ropeedit.point[ropeedit.pointhighlight].particlenum;
            level.rope[level.numofropes].texturenum=ropeedit.texturenum;

            level.numofropes++;
            }
          ropeedit.pointnum=-1;
          }
        }
    }

    if (mouse.rmb && !prevmouse.rmb)
      {
      for (count=0;count<level.numofropes;count++)
        {
        if (ropeedit.point[ropeedit.pointhighlight].objectnum==level.rope[count].obj1 && ropeedit.point[ropeedit.pointhighlight].particlenum==level.rope[count].obj1part)
          deletelevelrope(count);
        }
      for (count=0;count<level.numofropes;count++)
        {
        if (ropeedit.point[ropeedit.pointhighlight].objectnum==level.rope[count].obj2 && ropeedit.point[ropeedit.pointhighlight].particlenum==level.rope[count].obj2part)
          deletelevelrope(count);
        }
      /*
      for (count=0;count<level.numofropes;count++)
        {
        if (pointintersectline(vec,level.object[level.rope[count].obj1].vertex[level.rope[count].obj1part],level.object[level.rope[count].obj2].vertex[level.rope[count].obj2part],0.25f))
          deletelevelrope(count);
        }
      */
      }
    if (keyboard[SCAN_Q] && !prevkeyboard[SCAN_Q])
    if (ropeedit.texturenum<3)
      ropeedit.texturenum++;

    if (keyboard[SCAN_Z] && !prevkeyboard[SCAN_Z])
    if (ropeedit.texturenum>0)
      ropeedit.texturenum--;
}

void editlevel(int need_to_open_editor)
  {
  int count,count2;
  int x,y;
  int editing_level_gametype = 0;
  int editing_editor_layer = 0;
  int simtimer;
  int simcount;
  float vec[3], vec2[3];
  int menu_save = -1;
  int menu_load = -1;
  int menu_setup = -1;
  int menu_first_gametype_item = -1;
  int menu_gametype = -1;

  gametype gametypes[] = {
    GAMETYPE_CAMPAIGN,
    GAMETYPE_COLLECTION,
    GAMETYPE_2FOOTBALL,
    GAMETYPE_2SUMO,
    GAMETYPE_2GREED,
    GAMETYPE_2DUEL,
    GAMETYPE_2DRAGSTER,
    GAMETYPE_2COLLECTION,
    GAMETYPE_2RACING,
    GAMETYPE_4FOOTBALL,
    GAMETYPE_4SUMO,
  };

  if (need_to_open_editor && editor.filename[0]){
    loadlevel(editor.filename);
    setuplevel();
    setupgame();
  }

  simtimer=SDL_GetTicks();

  resetmenuitems();

  joystickmenu=0;

  editor.active=1;
  ropeedit.ropetype=PULLING_PISTON;
  view.zoom=10.0f;

  while (!menuitem[0].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glStencilMask(~0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
    glStencilMask(0);

    glColor3fv(level.ambient[3]);
    if (level.background[0]!=0)
      displaybackground(660);


    numofmenuitems=0;
    createmenuitem("",0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_F1);
    createmenuitem(TXT_LEVELNAME"     ",(640|TEXT_END),432,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_STRINGINPUT,editor.filename);
    setmenuitem(MO_HOTKEY,SCAN_ENTER);
    createmenuitem(TXT_BACKGROUND"     ",(640|TEXT_END),400,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_STRINGINPUT,&level.background);
    menu_save = createmenuitem(TXT_SAVE,(640|TEXT_END),464,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_F9);
    setmenuitem(MO_REPEAT);
    menu_load = createmenuitem(TXT_LOAD,(640-16*(strlen(TXT_SAVE)+2)|TEXT_END),464,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_F7);
    setmenuitem(MO_REPEAT);
    menu_setup = createmenuitem(TXT_SETUP,(640-16*(strlen(TXT_SAVE)+strlen(TXT_LOAD)+4)|TEXT_END),464,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_F5);
    setmenuitem(MO_REPEAT);
    createmenuitem(TXT_TILES,0,464,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_F6);
    setmenuitem(MO_SET, &editor_mode, EDITOR_MODE_TILES);
    createmenuitem(TXT_OBJECTS,(strlen(TXT_TILES)+1)*16,464,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_F3);
    setmenuitem(MO_SET, &editor_mode, EDITOR_MODE_OBJECTS);
    createmenuitem(TXT_ROPES,(strlen(TXT_TILES)+strlen(TXT_OBJECTS)+2)*16,464,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_F4);
    setmenuitem(MO_SET, &editor_mode, EDITOR_MODE_ROPES);
    setmenuitem(MO_FUNCTION, rope_edit_button_function); // MO_SET *must* be called before MO_FUNCTION
    createmenuitem("Show lines",0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_L);
    setmenuitem(MO_TOGGLE, &editor.showlines);
    createmenuitem("Focus layer",0,16,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY, SCAN_K);
    setmenuitem(MO_TOGGLE, &editor.showgrid);

    // bottom left corner
    if (editor_mode == EDITOR_MODE_TILES){
      if (editing_editor_layer){
        createmenuitem(TXT_BACK,0,368,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_ESC);
        setmenuitem(MO_TOGGLE, &editing_editor_layer);
        createmenuitem(TXT_BACKGROUND,0,368-16,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_1);
        setmenuitem(MO_SET, &editor.mode, 0);
        createmenuitem(TXT_MIDGROUND,0,368-32,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_2);
        setmenuitem(MO_SET, &editor.mode, 1);
        createmenuitem(TXT_FOREGROUND,0,368-48,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_3);
        setmenuitem(MO_SET, &editor.mode, 2);
        createmenuitem(TXT_WAYBACKGROUND,0,368-64,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_4);
        setmenuitem(MO_SET, &editor.mode, 3);
      }
      else{
        if (editor.mode==0){
          createmenuitem(TXT_BACKGROUND,0,368,16,1.0f,1.0f,1.0f,1.0f);
          setmenuitem(MO_TOGGLE, &editing_editor_layer);
        }
        else if (editor.mode==1){
          createmenuitem(TXT_MIDGROUND,0,368,16,1.0f,1.0f,1.0f,1.0f);
          setmenuitem(MO_TOGGLE, &editing_editor_layer);
        }
        else if (editor.mode==2){
          createmenuitem(TXT_FOREGROUND,0,368,16,1.0f,1.0f,1.0f,1.0f);
          setmenuitem(MO_TOGGLE, &editing_editor_layer);
        }
        else if (editor.mode==3){
          createmenuitem(TXT_WAYBACKGROUND,0,368,16,1.0f,1.0f,1.0f,1.0f);
          setmenuitem(MO_TOGGLE, &editing_editor_layer);
        }
        createmenuitem("",0,368,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_1);
        setmenuitem(MO_SET, &editor.mode, 0);
        createmenuitem("",0,368,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_2);
        setmenuitem(MO_SET, &editor.mode, 1);
        createmenuitem("",0,368,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_3);
        setmenuitem(MO_SET, &editor.mode, 2);
        createmenuitem("",0,368,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_HOTKEY, SCAN_4);
        setmenuitem(MO_SET, &editor.mode, 3);
      }
    }
    if (editor_mode == EDITOR_MODE_ROPES){
      // maybe: make a loop?
      count = 416;
      createmenuitem(ROPE_TYPE_NAMES[1],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_1);
      setmenuitem(MO_SET, &ropeedit.ropetype, WEAK_ROPE);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[2],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_2);
      setmenuitem(MO_SET, &ropeedit.ropetype, STRONG_ROPE);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[3],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_3);
      setmenuitem(MO_SET, &ropeedit.ropetype, WEAK_CHAIN);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[4],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_4);
      setmenuitem(MO_SET, &ropeedit.ropetype, STRONG_CHAIN);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[5],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_5);
      setmenuitem(MO_SET, &ropeedit.ropetype, PUSHING_PISTON);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[6],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_6);
      setmenuitem(MO_SET, &ropeedit.ropetype, HALF_PUSHED_PUSHING_PISTON);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[7],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_7);
      setmenuitem(MO_SET, &ropeedit.ropetype, PULLING_PISTON);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[8],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_8);
      setmenuitem(MO_SET, &ropeedit.ropetype, HALF_PULLED_PULLING_PISTON);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[9],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_9);
      setmenuitem(MO_SET, &ropeedit.ropetype, BAR);
      count -= 16;
      createmenuitem(ROPE_TYPE_NAMES[0],0,count,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY, SCAN_0);
      setmenuitem(MO_SET, &ropeedit.ropetype, SPRING);
      count -= 16;
    }

    // top right corner
    if (editor_mode == EDITOR_MODE_TILES){
      int y = 0;
      int offset = 16;
      menu_gametype = createmenuitem(TXT_GAMETYPE,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
      if (!editing_level_gametype){
        menu_first_gametype_item = createmenuitem(GAMETYPE_NAMES[level.gametype],(640|TEXT_END),y+offset,16,1.0f,1.0f,1.0f,1.0f);
        createmenuitem(TXT_GAMETIME,(640|TEXT_END),y+offset*2,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_INTINPUT,&level.time);
        createmenuitem(TXT_RED"  ",(640|TEXT_END),y+offset*4,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.ambient[editor.mode][0]);
        createmenuitem(TXT_GREEN,(640|TEXT_END),y+offset*6,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.ambient[editor.mode][1]);
        createmenuitem(TXT_BLUE" ",(640|TEXT_END),y+offset*8,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.ambient[editor.mode][2]);
      }
      else{
        menu_first_gametype_item = numofmenuitems;
        for (count = 0; count < GAMETYPE_COUNT; count++){
            createmenuitem(GAMETYPE_NAMES[gametypes[count]],(640|TEXT_END), offset + offset*count,16,1.0f,1.0f,1.0f,1.0f);
        }
      }
    }
    else if (editor_mode == EDITOR_MODE_OBJECTS){
      if (editor.objectnum!=-1)
        {
        createmenuitem("Mass   ",(640|TEXT_END),0,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.object[editor.objectnum].mass);
        setmenuitem(MO_HOTKEY,SCAN_M);
        createmenuitem("Friction",(640|TEXT_END),32,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.object[editor.objectnum].friction);
        setmenuitem(MO_HOTKEY,SCAN_F);
        createmenuitem("LType",(640|TEXT_END),64,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_INTINPUT,&level.object[editor.objectnum].lighttype);
        createmenuitem("Red  ",(640|TEXT_END),96,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.object[editor.objectnum].lightcolor[0]);
        createmenuitem("Green",(640|TEXT_END),128,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.object[editor.objectnum].lightcolor[1]);
        createmenuitem("Blue ",(640|TEXT_END),160,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.object[editor.objectnum].lightcolor[2]);
        createmenuitem("Inten",(640|TEXT_END),192,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_FLOATINPUT,&level.object[editor.objectnum].lightintensity);
        }
    }
    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkmenuitems();

    if ((menuitem[menu_gametype].active || menuitem[menu_first_gametype_item].active) && !editing_level_gametype){
        editing_level_gametype = 1;
        menuitem[menu_gametype].active = 0;
        menuitem[menu_first_gametype_item].active = 0;
    }
    if (editing_level_gametype){
        for (count = 0; count < GAMETYPE_COUNT; count++)
            if (menuitem[menu_first_gametype_item+count].active){
                editing_level_gametype = 0;
                menuitem[menu_first_gametype_item+count].active = 0;
                level.gametype = gametypes[count];
            }
    }
    if (menuitem[menu_load].active)
        {
        menu_last_loadlevel = loadlevel(editor.filename);
        setuplevel();
        setupgame();
        }
      if (menuitem[menu_setup].active)
        {
        setuplevel();
        setupgame();
      }
      if (menuitem[menu_save].active){
        savelevel(editor.filename);
      }

    zoom_view();

    view.zoomx=view.zoom+0.5f;
    view.zoomy=view.zoom*0.75f+0.5f;

    setuporthoviewport(0,0,640,480,view.zoom,view.zoom*0.75f,20.0f);
    setupviewpoint(view.position,view.orientation);

    setupframelighting();

    setuprenderobjects();

    scalevector(level.ambient[editor.mode],level.ambient[editor.mode],2.0f);

    if (!editor.showgrid)
      rendershadows();

    if (editor_mode == EDITOR_MODE_ROPES){
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER,0.0f);
      }

    if (!editor.showgrid || editor.mode==0)
      renderlevelback();
    if (!editor.showgrid || editor.mode==1)
      renderlevel();
    if (editor_mode == EDITOR_MODE_TILES)
      renderobjects();
    if (!editor.showgrid || editor.mode==2)
      renderlevelfore();
    if (editor_mode == EDITOR_MODE_OBJECTS || editor_mode == EDITOR_MODE_ROPES)
      renderlevelobjects();

    if (editor_mode == EDITOR_MODE_ROPES){
      glDisable(GL_ALPHA_TEST);
      renderropeedit();
    }
    scalevector(level.ambient[editor.mode],level.ambient[editor.mode],0.5f);

    if (editor.showlines)
      renderlevellines();

    rendereditblocks();

    setuptextdisplay();

    glDisable(GL_TEXTURE_2D);
    for (count=0;count<8;count++)
      {
      glBegin(GL_LINES);

      if (count==0)
        glColor4f(0.0f,1.0f,0.0f,1.0f);
      if (count==1)
        glColor4f(0.0f,1.0f,1.0f,1.0f);
      if (count==2)
        glColor4f(1.0f,1.0f,0.0f,1.0f);
      if (count==3)
        glColor4f(1.0f,0.0f,1.0f,1.0f);
      if (count==4)
        glColor4f(0.0f,0.5f,0.0f,1.0f);
      if (count==5)
        glColor4f(0.0f,0.5f,0.5f,1.0f);
      if (count==6)
        glColor4f(0.5f,0.5f,0.0f,1.0f);
      if (count==7)
        glColor4f(0.5f,0.0f,0.5f,1.0f);

      glVertex3f(level.area[count][0],level.area[count][1],0.0f);
      glVertex3f(level.area[count][2],level.area[count][1],0.0f);

      glVertex3f(level.area[count][2],level.area[count][1],0.0f);
      glVertex3f(level.area[count][2],level.area[count][3],0.0f);

      glVertex3f(level.area[count][2],level.area[count][3],0.0f);
      glVertex3f(level.area[count][0],level.area[count][3],0.0f);

      glVertex3f(level.area[count][0],level.area[count][3],0.0f);
      glVertex3f(level.area[count][0],level.area[count][1],0.0f);

      glEnd();
      }
    glEnable(GL_TEXTURE_2D);

    if (editor_mode == EDITOR_MODE_TILES){
      glBindTexture(GL_TEXTURE_2D,texture[editor.blocknum].glname);

      glBegin(GL_QUADS);

      glColor4f(1.0f,1.0f,1.0f,1.0f);

      vec[0]=0.0f;
      vec[1]=416.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glTexCoord2f(0.0f,0.0f);
      glVertex3f(vec[0],vec[1],-1.0f);

      vec[0]=64.0f;
      vec[1]=416.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(vec[0],vec[1],-1.0f);

      vec[0]=64.0f;
      vec[1]=480.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glTexCoord2f(1.0f,1.0f);
      glVertex3f(vec[0],vec[1],-1.0f);

      vec[0]=0.0f;
      vec[1]=480.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glTexCoord2f(0.0f,1.0f);
      glVertex3f(vec[0],vec[1],-1.0f);

      glEnd();

      drawtext(TXT_TILESET":/i",0,384,16,1.0f,1.0f,1.0f,1.0f,level.tileset);
      drawtext(TXT_TILE":/i",0,400,16,1.0f,1.0f,1.0f,1.0f,editor.blocknum);
    }
    if (editor_mode == EDITOR_MODE_OBJECTS){
      int INFO_Y = 352;
      int OFFSET_Y = 16;

      // draw level information
      drawtext(TXT_OBJECTS":/i",0,INFO_Y,16,1.0f,1.0f,1.0f,1.0f,level.numofobjects);
      drawtext(TXT_ROPES":/i",0,INFO_Y+OFFSET_Y,16,1.0f,1.0f,1.0f,1.0f,level.numofropes);

      // draw object type that will be created on lmb
      if (editor.objecttype <= 36)
          drawtext(TXT_OBJECTSET":/s",0,INFO_Y+OFFSET_Y*2,16,1.0f,1.0f,1.0f,1.0f,LVL_OBJ_NAMES[editor.objecttype]);
      else
          drawtext(TXT_OBJECTSET":NOTHING /i",0,INFO_Y+OFFSET_Y*2,16,1.0f,1.0f,1.0f,1.0f,editor.objecttype);

      // draw stuff related to the picked object
      if (editor.objectnum!=-1){
        drawtext(TXT_OBJECTNUM":/i",0,INFO_Y+OFFSET_Y*3,16,1.0f,1.0f,1.0f,1.0f,editor.objectnum);
        if (level.object[editor.objectnum].type <= 36)
            drawtext(TXT_OBJECTSET":/s",0,INFO_Y+OFFSET_Y*4,16,1.0f,1.0f,1.0f,1.0f,LVL_OBJ_NAMES[level.object[editor.objectnum].type]);
        else
            drawtext(TXT_OBJECTSET":NOTHING /i",0,INFO_Y+OFFSET_Y*4,16,1.0f,1.0f,1.0f,1.0f,level.object[editor.objectnum].type);
        drawtext(TXT_LINK":/i",0,INFO_Y+OFFSET_Y*5,16,1.0f,1.0f,1.0f,1.0f,level.object[editor.objectnum].link);
      }
    }
    if (editor_mode == EDITOR_MODE_ROPES){
      drawtext(TXT_NUMOFROPES":/i",0,432,16,1.0f,1.0f,1.0f,1.0f,level.numofropes);
      drawtext(TXT_TEXTURENUM":/i",0,448,16,1.0f,1.0f,1.0f,1.0f,ropeedit.texturenum);
    }

    if (menu_last_loadlevel >= 0){
      if (menu_last_loadlevel == LOADLEVELRESULT_OK){
        drawtext(TXT_LOAD_LEVEL_OK,(639|TEXT_END),480-16*6,16,0.0f,1.0f,0.0f,1.0f);
      } else if (menu_last_loadlevel == LOADLEVELRESULT_UNKNOWN){
        drawtext(TXT_LOAD_LEVEL_UNKNOWN,(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f);
      } else if (menu_last_loadlevel == LOADLEVELRESULT_FILEERROR){
        drawtext(TXT_LOAD_LEVEL_FILEERROR,(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f);
      } else if (menu_last_loadlevel == LOADLEVELRESULT_TOO_MANY_OBJECTS){
        drawtext(TXT_LOAD_LEVEL_TOO_MANY_OBJECTS,(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f);
      } else if (menu_last_loadlevel == LOADLEVELRESULT_TOO_MANY_ROPES){
        drawtext(TXT_LOAD_LEVEL_TOO_MANY_ROPES,(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f);
      } else if (menu_last_loadlevel == LOADLEVELRESULT_TEXTURE_SIZEX_TOO_BIG){
        drawtext(TXT_LOAD_LEVEL_TEXTURE_SIZEX_TOO_BIG,(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f);
      } else if (menu_last_loadlevel == LOADLEVELRESULT_TOO_MANY_BLOCK_LINES){
        drawtext(TXT_LOAD_LEVEL_TOO_MANY_BLOCK_LINES,(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f);
      } else {
        drawtext("LVL_VERSION = /i",(639|TEXT_END),480-16*6,16,1.0f,0.0f,0.0f,1.0f, menu_last_loadlevel);
      }
    }

    // draw mouse pos
    screen_to_world(mouse.x, mouse.y, &vec[0], &vec[1]);
    x = (int)vec[0];
    y = (int)vec[1];
    drawtext("/i /i", mouse.x, mouse.y, 10, 1.0f,1.0f,1.0f,1.0f, x, y);

    if (debug_grid_blocknums)
    for (count=view.position[1]-view.zoomy;count<=view.position[1]+view.zoomy;count++)
    if (count>=0 && count<256)
    for (count2=view.position[0]-view.zoomx;count2<=view.position[0]+view.zoomx;count2++)
    if (count2>=0 && count2<256){
        int blocknum=level.grid[count][count2];
        world_to_screen((float)count2+0.5f, (float)count-0.5f, &vec[0], &vec[1]);
        drawtext("/i",vec[0],vec[1],8,1.0f,1.0f,1.0f,1.0f, blocknum);
    }
    if (editor_mode == EDITOR_MODE_OBJECTS){
      if (debug_objectnums){
        for (count = 0; count < level.numofobjects; count++){
            world_to_screen(level.object[count].position[0], level.object[count].position[1], &vec[0], &vec[1]);
            x = (int)vec[0];
            y = (int)vec[1];
            drawtext("/i", x, y+10, 10, 1.0f,1.0f,1.0f,1.0f, count);
        }
        if (debug_objectlinks){
          for (count = 0; count < numofobjects; count++){
              world_to_screen(object[count].position[0], object[count].position[1], &vec[0], &vec[1]);
              drawtext("link: /i", (int)vec[0],(int)vec[1]+20, 10, 1.0f, 1.0f, 1.0f, 1.0f, object[count].link);
          }
        }
      }
    }
    if (editor_mode == EDITOR_MODE_ROPES){
      if (debug_ropeedit_points){
        drawtext("/i", 0, 0, 10, 1.0f,1.0f,1.0f,1.0f, ropeedit.numofpoints);
      }
    }

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    if (editor_mode == EDITOR_MODE_TILES){
      process_events_tiles();
    }
    if (editor_mode == EDITOR_MODE_OBJECTS){
      process_events_objects();
    }
    if (editor_mode == EDITOR_MODE_ROPES){
      process_events_ropes();
    }

    simcount=0;
    while (SDL_GetTicks()-simtimer>20 && simcount<5)
      {
      simcount++;
      count=SDL_GetTicks()-simtimer-20;
      simtimer=SDL_GetTicks()-count;

      if (!menuinputkeyboard)
          pan_view();
      }
    if (!menuinputkeyboard)
      {
      if (keyboard[SCAN_F2] && !prevkeyboard[SCAN_F2])
        {
        editblock();
        simtimer=SDL_GetTicks();
        }
      if (keyboard[SCAN_T] && !prevkeyboard[SCAN_T])
        {
        edittextures();
        simtimer=SDL_GetTicks();
        }
      }
    }

  editor.active=0;

  joystickmenu=1;

  resetmenuitems();
  }

void setblock(int x,int y,int blocknum)
  {
  if (x<0)
    return;
  if (x>=256)
    return;
  if (y<0)
    return;
  if (y>=256)
    return;

  if (editor.mode==0)
    level.backgrid[y][x]=blocknum;
  if (editor.mode==1)
    level.grid[y][x]=blocknum;
  if (editor.mode==2)
    level.foregrid[y][x]=blocknum;

  setuplevellines(x-1,y-1,x+1,y+1);
  }

int getblock(int x,int y)
  {
  int blocknum=0;

  if (x<0)
    return(0);
  if (x>=256)
    return(0);
  if (y<0)
    return(0);
  if (y>=256)
    return(0);

  if (editor.mode==0)
    blocknum=level.backgrid[y][x];
  if (editor.mode==1)
    blocknum=level.grid[y][x];
  if (editor.mode==2)
    blocknum=level.foregrid[y][x];

  return(blocknum);
  }

void rendereditblocks(void)
  {
  int count,count2;
  int x,y;
  //int blocknum;
  float vec[3];

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINES);

  glColor4f(1.0f,1.0f,1.0f,1.0f);

  if (!editor.paste)
    {
    for (count=editor.editarea[0][1];count<=editor.editarea[1][1];count++)
    for (count2=editor.editarea[0][0];count2<=editor.editarea[1][0];count2++)
      {
      glVertex3f((float)count2+0.0f,(float)count+1.0f,0.0f);
      glVertex3f((float)count2+1.0f,(float)count+1.0f,0.0f);

      glVertex3f((float)count2+1.0f,(float)count+1.0f,0.0f);
      glVertex3f((float)count2+1.0f,(float)count+0.0f,0.0f);

      glVertex3f((float)count2+1.0f,(float)count+0.0f,0.0f);
      glVertex3f((float)count2+0.0f,(float)count+0.0f,0.0f);

      glVertex3f((float)count2+0.0f,(float)count+0.0f,0.0f);
      glVertex3f((float)count2+0.0f,(float)count+1.0f,0.0f);
      }
    }
  else
    {
    get_mouse_coords(&vec[0], &vec[1]);
    x = (int)vec[0];
    y = (int)vec[1];

    for (count=0;count<=editor.copysize[1];count++)
    for (count2=0;count2<=editor.copysize[0];count2++)
      {
      glVertex3f((float)(x+count2)+0.0f,(float)(y+count)+1.0f,0.0f);
      glVertex3f((float)(x+count2)+1.0f,(float)(y+count)+1.0f,0.0f);

      glVertex3f((float)(x+count2)+1.0f,(float)(y+count)+1.0f,0.0f);
      glVertex3f((float)(x+count2)+1.0f,(float)(y+count)+0.0f,0.0f);

      glVertex3f((float)(x+count2)+1.0f,(float)(y+count)+0.0f,0.0f);
      glVertex3f((float)(x+count2)+0.0f,(float)(y+count)+0.0f,0.0f);

      glVertex3f((float)(x+count2)+0.0f,(float)(y+count)+0.0f,0.0f);
      glVertex3f((float)(x+count2)+0.0f,(float)(y+count)+1.0f,0.0f);
      }
    }
  glEnd();

  glEnable(GL_TEXTURE_2D);
  }

void editblock(void)
  {
  int count,count2/*,count3*/;
  int x,y;
  int simtimer;
  int simcount;
  float friction;
  float vec[3]/*,vec2[3]*/;
  int load_texture = -1;
  char filename[256];
  //float normal[3];
  //int changeddir;

  /*
  changeddir=changetilesetdir();

  filename[4]=48+(editor.blocknum/100)%10;
  filename[5]=48+(editor.blocknum/10)%10;
  filename[6]=48+editor.blocknum%10;
  loadtexturetga(EDITBLOCK_TEXTURE,filename,0,GL_CLAMP,GL_CLAMP,GL_NEAREST,GL_NEAREST);

  if (changeddir==0)
    chdir("..");
  */

  copytexture(EDITBLOCK_TEXTURE,editor.blocknum);
  texture[EDITBLOCK_TEXTURE].magfilter=GL_NEAREST;
  texture[EDITBLOCK_TEXTURE].minfilter=GL_NEAREST;
  setuptexture(EDITBLOCK_TEXTURE);
  memset(filename, 0, 256);
  strcpy(filename, texture[editor.blocknum].filename);

  simtimer=SDL_GetTicks();

  friction=1.0f;

  resetmenuitems();

  while (!menuitem[0].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(TXT_EXIT,0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    count=32;
    createmenuitem(TXT_FRICTION,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_FLOATINPUT,&block[editor.blocknum].friction);
    setmenuitem(MO_HOTKEY,SCAN_F);
    count+=32;
    createmenuitem(TXT_BREAKPOINT,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_FLOATINPUT,&block[editor.blocknum].breakpoint);
    setmenuitem(MO_HOTKEY,SCAN_B);
    count+=32;
    createmenuitem(TXT_MIDDAMAGE,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_INTINPUT,&block[editor.blocknum].middamage);
    setmenuitem(MO_HOTKEY,SCAN_M);
    count+=32;
    createmenuitem(TXT_FOREDAMAGE,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_INTINPUT,&block[editor.blocknum].foredamage);
    setmenuitem(MO_HOTKEY,SCAN_O);
    count+=32;
    createmenuitem(TXT_DENSITY,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_FLOATINPUT,&block[editor.blocknum].density);
    setmenuitem(MO_HOTKEY,SCAN_D);
    count+=32;
    createmenuitem(TXT_DRAG"   ",(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_FLOATINPUT,&block[editor.blocknum].drag);
    setmenuitem(MO_HOTKEY,SCAN_R);
    count+=32;
    createmenuitem(TXT_ANIMATION,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_INTINPUT,&block[editor.blocknum].animation);
    setmenuitem(MO_HOTKEY,SCAN_N);
    count+=32;
    createmenuitem(TXT_ANIMATESPD,(640|TEXT_END),count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_INTINPUT,&block[editor.blocknum].animationspeed);
    setmenuitem(MO_HOTKEY,SCAN_S);
    count+=32;
    createmenuitem(TXT_FILENAME"                           ",(640|TEXT_END),448,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_STRINGINPUT,filename);
    load_texture = createmenuitem(TXT_LOAD_TEXTURE,(640|TEXT_END),448-16,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_L);

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkmenuitems();

    if (menuitem[load_texture].active){
      loadtexture(editor.blocknum,filename,0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
      copytexture(EDITBLOCK_TEXTURE,editor.blocknum);
      texture[EDITBLOCK_TEXTURE].magfilter=GL_NEAREST;
      texture[EDITBLOCK_TEXTURE].minfilter=GL_NEAREST;
      setuptexture(EDITBLOCK_TEXTURE);
    }

    if (!menuinputkeyboard)
      {
      if (keyboard[SCAN_DELETE] && !prevkeyboard[SCAN_DELETE])
      if (block[editor.blocknum].numoflines>0)
        block[editor.blocknum].numoflines--;
      }
    if (mouse.x<464)
      {
      if (mouse.lmb && !prevmouse.lmb)
        {
        x=(mouse.x+6-32)/12;
        y=(mouse.y+6-32)/12;
        if (x<0)
          x=0;
        if (y<0)
          y=0;
        if (x>32)
          x=32;
        if (y>32)
          y=32;
        vec[0]=(float)x/32.0f;
        vec[1]=1.0f-(float)y/32.0f;

        block[editor.blocknum].line[block[editor.blocknum].numoflines][0]=vec[0];
        block[editor.blocknum].line[block[editor.blocknum].numoflines][1]=vec[1];
        }
      x=abs(mouse.x+6-32)/12;
      y=abs(mouse.y+6-32)/12;
      if (x<0)
        x=0;
      if (y<0)
        y=0;
      if (x>32)
        x=32;
      if (y>32)
        y=32;
      vec[0]=(float)x/32.0f;
      vec[1]=1.0f-(float)y/32.0f;

      block[editor.blocknum].line[block[editor.blocknum].numoflines][2]=vec[0];
      block[editor.blocknum].line[block[editor.blocknum].numoflines][3]=vec[1];
      block[editor.blocknum].line[block[editor.blocknum].numoflines][4]=friction;
      if (!mouse.lmb && prevmouse.lmb)
      if (block[editor.blocknum].line[block[editor.blocknum].numoflines][0]!=block[editor.blocknum].line[block[editor.blocknum].numoflines][2] || block[editor.blocknum].line[block[editor.blocknum].numoflines][1]!=block[editor.blocknum].line[block[editor.blocknum].numoflines][3])
        block[editor.blocknum].numoflines++;
      }

    setupblockflags(editor.blocknum);

    setuptextdisplay();

    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);


    glColor4f(0.0f,1.0f,0.0f,1.0f);

    for (count=0;count<=4;count++)
      {
      vec[0]=16.0f;
      vec[1]=32.0f+96.0f*(float)count;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glVertex3f(vec[0],vec[1],-1.0f);

      vec[0]=32.0f+400.0f;
      vec[1]=32.0f+96.0f*(float)count;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glVertex3f(vec[0],vec[1],-1.0f);

      vec[0]=32.0f+96.0f*(float)count;
      vec[1]=16.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glVertex3f(vec[0],vec[1],-1.0f);

      vec[0]=32.0f+96.0f*(float)count;
      vec[1]=32.0f+400.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glVertex3f(vec[0],vec[1],-1.0f);
      }
    glEnd();

    glEnable(GL_TEXTURE_2D);


    glBindTexture(GL_TEXTURE_2D,texture[EDITBLOCK_TEXTURE].glname);

    glBegin(GL_QUADS);

    glColor4f(1.0f,1.0f,1.0f,1.0f);

    vec[0]=32.0f;
    vec[1]=32.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(vec[0],vec[1],-1.0f);

    vec[0]=32.0f+384.0f;
    vec[1]=32.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(vec[0],vec[1],-1.0f);

    vec[0]=32.0f+384.0f;
    vec[1]=32.0f+384.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(vec[0],vec[1],-1.0f);

    vec[0]=32.0f;
    vec[1]=32.0f+384.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(vec[0],vec[1],-1.0f);

    glEnd();


    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);

    if (mouse.x<464)
    if (mouse.lmb)
      block[editor.blocknum].numoflines++;

    for (count=0;count<block[editor.blocknum].numoflines;count++)
      {
      glColor4f(1.0f,0.0f,0.0f,1.0f);

      vec[0]=32.0f+block[editor.blocknum].line[count][0]*384.0f;
      vec[1]=32.0f+384.0f-block[editor.blocknum].line[count][1]*384.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glVertex3f(vec[0],vec[1],-1.0f);

      glColor4f(0.0f,0.0f,1.0f,1.0f);

      vec[0]=32.0f+block[editor.blocknum].line[count][2]*384.0f;
      vec[1]=32.0f+384.0f-block[editor.blocknum].line[count][3]*384.0f;
      convertscreenvertex(vec,font.sizex,font.sizey);
      glVertex3f(vec[0],vec[1],-1.0f);
      }

    if (mouse.x<464)
    if (mouse.lmb)
      block[editor.blocknum].numoflines--;

    glEnd();

    glEnable(GL_TEXTURE_2D);

    drawtext(TXT_LINES":/i",0,432,12,1.0f,1.0f,1.0f,1.0f,block[editor.blocknum].numoflines);
    drawtext(TXT_ALPHA":/i",0,448,12,1.0f,1.0f,1.0f,1.0f,texture[editor.blocknum].isalpha);

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    if (keyboard[SCAN_A] && !prevkeyboard[SCAN_A])
      {
      for (count=0;count<texture[editor.blocknum].sizey;count++)
      for (count2=0;count2<texture[editor.blocknum].sizex;count2++)
      if (!bigendian)
        texture[editor.blocknum].rgba[0][count*texture[editor.blocknum].sizex+count2]&=0xFFFFFF;
      else
        texture[editor.blocknum].rgba[0][count*texture[editor.blocknum].sizex+count2]&=0xFFFFFF00;

      setupblockalpha(editor.blocknum);

      texture[editor.blocknum].isalpha=0;
      for (count=0;count<texture[editor.blocknum].sizey;count++)
      for (count2=0;count2<texture[editor.blocknum].sizex;count2++)
        {
        if (!bigendian)
        if ((texture[editor.blocknum].rgba[0][count*texture[editor.blocknum].sizex+count2]&0xFF000000)!=0xFF000000)
          texture[editor.blocknum].isalpha=1;

        if (bigendian)
        if ((texture[editor.blocknum].rgba[0][count*texture[editor.blocknum].sizex+count2]&0x000000FF)!=0x000000FF)
          texture[editor.blocknum].isalpha=1;
        }

      /*
      texture[editor.blocknum].isalpha=0;

      for (count=0;count<texture[editor.blocknum].sizey;count++)
      for (count2=0;count2<texture[editor.blocknum].sizex;count2++)
        {
        texture[editor.blocknum].rgba[0][count*texture[editor.blocknum].sizex+count2]|=0xFF000000;

        for (count3=0;count3<block[editor.blocknum].numoflines;count3++)
          {
          vec[0]=((float)count2+0.5f)/(float)texture[editor.blocknum].sizex;
          vec[1]=1.0f-((float)count+0.5f)/(float)texture[editor.blocknum].sizey;
          vec[2]=0.0f;
          vec[0]-=block[editor.blocknum].line[count3][0];
          vec[1]-=block[editor.blocknum].line[count3][1];
          vec2[0]=block[editor.blocknum].line[count3][2]-block[editor.blocknum].line[count3][0];
          vec2[1]=block[editor.blocknum].line[count3][3]-block[editor.blocknum].line[count3][1];
          vec2[2]=0.0f;
          crossproduct(normal,vec,vec2);
          if (normal[2]>0.0f)
            {
            texture[editor.blocknum].rgba[0][count*texture[editor.blocknum].sizex+count2]&=0xFFFFFF;
            texture[editor.blocknum].isalpha=1;
            }
          }
        }
      */

      setuptexture(editor.blocknum);

      memcpy(texture[EDITBLOCK_TEXTURE].rgba[0],texture[editor.blocknum].rgba[0],texture[editor.blocknum].sizex*texture[editor.blocknum].sizey*4);
      setuptexture(EDITBLOCK_TEXTURE);

      texture[editor.blocknum].filename[0] = 0; // detach this texture from its file because saving and loading will bring the whole texture back
      }

    if (keyboard[SCAN_Q] && !prevkeyboard[SCAN_Q])
      {
      if (!keyboard[SCAN_SHIFT])
        editor.blocknum++;
      else
        editor.blocknum+=10;
      if (editor.blocknum>255)
        editor.blocknum=255;

      copytexture(EDITBLOCK_TEXTURE,editor.blocknum);
      texture[EDITBLOCK_TEXTURE].magfilter=GL_NEAREST;
      texture[EDITBLOCK_TEXTURE].minfilter=GL_NEAREST;
      setuptexture(EDITBLOCK_TEXTURE);
      memset(filename, 0, 256);
      strcpy(filename, texture[editor.blocknum].filename);
      /*
      changeddir=changetilesetdir();

      filename[4]=48+(editor.blocknum/100)%10;
      filename[5]=48+(editor.blocknum/10)%10;
      filename[6]=48+editor.blocknum%10;
      loadtexturetga(EDITBLOCK_TEXTURE,filename,0,GL_CLAMP,GL_CLAMP,GL_NEAREST,GL_NEAREST);

      if (changeddir==0)
        chdir("..");
      */
      }
    if (keyboard[SCAN_Z] && !prevkeyboard[SCAN_Z])
      {
      if (!keyboard[SCAN_SHIFT])
        editor.blocknum--;
      else
        editor.blocknum-=10;
      if (editor.blocknum<0)
        editor.blocknum=0;

      copytexture(EDITBLOCK_TEXTURE,editor.blocknum);
      texture[EDITBLOCK_TEXTURE].magfilter=GL_NEAREST;
      texture[EDITBLOCK_TEXTURE].minfilter=GL_NEAREST;
      setuptexture(EDITBLOCK_TEXTURE);
      memset(filename, 0, 256);
      strcpy(filename, texture[editor.blocknum].filename);
      /*
      changeddir=changetilesetdir();

      filename[4]=48+(editor.blocknum/100)%10;
      filename[5]=48+(editor.blocknum/10)%10;
      filename[6]=48+editor.blocknum%10;
      loadtexturetga(EDITBLOCK_TEXTURE,filename,0,GL_CLAMP,GL_CLAMP,GL_NEAREST,GL_NEAREST);

      if (changeddir==0)
        chdir("..");
      */
      }
    /*
    if (keyboard[SCAN_F4] && !prevkeyboard[SCAN_F4])
      {
      changeddir=changetilesetdir();
      loadblock(editor.blocknum);

      if (changeddir==0)
        chdir("..");
      }
    if (keyboard[SCAN_F9] && !prevkeyboard[SCAN_F9])
      {
      changeddir=changetilesetdir();
      saveblock(editor.blocknum);

      if (changeddir==0)
        chdir("..");
      }
    */

    simcount=0;
    while (SDL_GetTicks()-simtimer>20 && simcount<5)
      {
      simcount++;
      count=SDL_GetTicks()-simtimer-20;
      simtimer=SDL_GetTicks()-count;
      }
    }

  resetmenuitems();
  }

void renderlevellines(void)
  {
  int count,count2,count3;
  int blocknum;
  float vec[3];

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINES);

  glColor4f(0.0f,0.0f,1.0f,1.0f);

  for (count=view.position[1]-32;count<view.position[1]+32;count++)
  if (count>=0 && count<256)
  for (count2=view.position[0]-32;count2<view.position[0]+32;count2++)
  if (count2>=0 && count2<256)
    {
    blocknum=level.grid[count][count2];
    for (count3=0;count3<block[blocknum].numoflines;count3++)
    if (((level.gridflags[count][count2]>>count3)&1)==0)
      {
      vec[0]=(float)count2+block[blocknum].line[count3][0];
      vec[1]=(float)count+block[blocknum].line[count3][1];
      glVertex3f(vec[0],vec[1],0.0f);

      vec[0]=(float)count2+block[blocknum].line[count3][2];
      vec[1]=(float)count+block[blocknum].line[count3][3];
      glVertex3f(vec[0],vec[1],0.0f);
      }
    }

  glEnd();

  glEnable(GL_TEXTURE_2D);
  }
