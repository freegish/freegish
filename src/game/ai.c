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

#include <math.h>

#include "../game/ai.h"
#include "../game/game.h"
#include "../game/level.h"
#include "../game/gameobject.h"
#include "../game/random.h"
#include "../math/vector.h"

void enemygishai(int objectnum){
    float vec[3];
    /*
    object[objectnum].axis[0]=-object[0].axis[0];
    object[objectnum].axis[1]=object[0].axis[0];
    object[objectnum].button=object[0].button;
    */
    subtractvectors(vec,object[objectnum-game.numofplayers].position,object[objectnum].position);
    if (vec[0]<-0.5f)
      object[objectnum].axis[0]=-1.0f;
    if (vec[0]>0.5f)
      object[objectnum].axis[0]=1.0f;
    if (vectorlength(vec)<2.0f)
      object[objectnum].button|=8;
    /*
    subtractvectors(vec,object[2].position,object[objectnum].position);
    if (vectorlength(vec)>3.0f)
      object[objectnum].button=1;
    */
}

void beastai(int objectnum){
    float vec[3];
    float intersectpoint[3];
    float normal[3];
    float scale;

    float mindistance = INFINITY;
    float distance;
    int closest_player = 0;

    // find closest player
    for (int i = 0; i < game.numofplayers; i++){
        subtractvectors(vec, object[objectnum].position, object[i].position);
        distance = vectorlength(vec);
        if (distance < mindistance){
            closest_player = i;
            mindistance = distance;
        }
    }

    // attack closest player
    if (object[closest_player].idata[0]==0)
    if (fabs(object[closest_player].position[0]-object[objectnum].position[0])<12.0f)
    if (fabs(object[closest_player].position[1]-object[objectnum].position[1])<8.0f)
      {
      if (object[objectnum].beasttype!=7 && object[objectnum].beasttype!=13)
        {
        subtractvectors(vec,object[closest_player].position,object[objectnum].position);
        if ((vec[0]>-2.5f && vec[0]<0.0f && object[objectnum].direction==0) || (vec[0]<2.5f && vec[0]>0.0f && object[objectnum].direction==1))
        if (fabs(vec[1])<1.0f)
          object[objectnum].button|=1;

        //if (object[objectnum].beasttype==12)
        //if ((game.framenum&511)==0)
        //  object[objectnum].button|=2;

        if ((rnd()&7)!=7)
          {
          if (object[closest_player].position[0]<object[objectnum].position[0])
            object[objectnum].axis[0]-=1.0f;
          else
            object[objectnum].axis[0]+=1.0f;
  
          if (object[objectnum].beasttype!=12)
          if ((rnd()&255)==0)
            object[objectnum].button|=2;

          if (object[objectnum].beasttype==12)
          if ((rnd()&255)==0 && (rnd()&1)==0)
            object[objectnum].button|=2;
          }
        }
      else
        {
        subtractvectors(vec,object[closest_player].position,object[objectnum].position);
        if (vec[0]>-8.0f && vec[0]<8.0f)
        if (fabs(vec[1])<6.0f)
        if ((game.framenum&127)==0)
          object[objectnum].button|=1;
        /*
          if (object[0].position[0]<object[objectnum].position[0]-5.0f)
            object[objectnum].axis[0]-=1.0f;
          if (object[0].position[0]>object[objectnum].position[0]+5.0f)
            object[objectnum].axis[0]+=1.0f;
        */
        //if (object[0].position[0]<object[objectnum].position[0])
        if (object[closest_player].position[0]<object[objectnum].position[0]-5.0f)
          {
          scaleaddvectors(vec,object[objectnum].position,object[objectnum].orientation[0],-3.0f);
          scaleaddvectors(vec,vec,object[objectnum].orientation[1],-2.0);
          if (lineintersectlevel(intersectpoint,normal,&scale,object[objectnum].position,vec))
            object[objectnum].axis[0]-=1.0f;
          else
            {
            if (object[objectnum].velocity[0]<-0.01f)
              object[objectnum].axis[0]+=1.0f;
            }
          }
        if (object[closest_player].position[0]>object[objectnum].position[0]+5.0f)
          {
          scaleaddvectors(vec,object[objectnum].position,object[objectnum].orientation[0],3.0f);
          scaleaddvectors(vec,vec,object[objectnum].orientation[1],-2.0f);
          if (lineintersectlevel(intersectpoint,normal,&scale,object[objectnum].position,vec))
            object[objectnum].axis[0]+=1.0f;
          else
            {
            if (object[objectnum].velocity[0]>0.01f)
              object[objectnum].axis[0]-=1.0f;
            }
          }
        }
      }
}

void objectai(int objectnum)
  {
  object[objectnum].axis[0]=0.0f;
  object[objectnum].axis[1]=0.0f;
  object[objectnum].button=0;

  if (object[objectnum].type==OBJ_TYPE_GISH)
  if (game.levelnum==34)
      enemygishai(objectnum);
  if (object[objectnum].type==OBJ_TYPE_BEAST_OR_BOBBLE)
      beastai(objectnum);

  if (object[objectnum].axis[0]<-1.0f)
    object[objectnum].axis[0]=-1.0f;
  if (object[objectnum].axis[0]>1.0f)
    object[objectnum].axis[0]=1.0f;
  if (object[objectnum].axis[1]<-1.0f)
    object[objectnum].axis[1]=-1.0f;
  if (object[objectnum].axis[1]>1.0f)
    object[objectnum].axis[1]=1.0f;
  }
