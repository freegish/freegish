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

#if defined(LINUX) || defined(MAC)
  #include <unistd.h>
#endif

#include <math.h>
#include <string.h>

#include "../game/animation.h"
#include "../game/gameaudio.h"
#include "../game/game.h"
#include "../game/gameobject.h"
#include "../game/random.h"
#include "../game/sprite.h"
#include "../math/vector.h"
#include "../physics/object.h"
#include "../physics/particle.h"
#include "../video/texture.h"

int numofanimations;
_animation animation[128];

void objectanimation(void)
  {
  int count,count2;
  int particlelist[2];
  float vec[3];
  float intersectpoint[3];
  float normal[3];
  float scale;

  for (count=0;count<numofobjects;count++)
    {
    if (object[count].type==OBJ_TYPE_GISH)
      {
      /*
      for (count2=0;count2<object[count].numofparticles;count2++)
      if ((rand()&8191)==0)
        createparticle(5,particle[object[count].particle[count2]].position,particle[object[count].particle[count2]].velocity,0.125f,-1,100);
      */

      if (object[count].frame==12)
        {
        object[count].framedelay+=0.125f;
        if (object[count].framedelay>=1.0f)
          {
          object[count].frame=0;
          object[count].framedelay=0.0f;
          }
        }

      if (object[count].numoforientations==0)
        {
        if (object[count].frame>=5 && object[count].frame<=7)
          object[count].frame=1;
        if (object[count].frame<4)
          {
          object[count].framedelay+=1.0f;
          if (object[count].framedelay>=4.0f)
            {
            object[count].frame++;
            object[count].framedelay=0.0f;
            }
          }
        }
      else
        {
        if (object[count].frame>0 && object[count].frame<=4)
          {
          object[count].framedelay+=1.0f;
          if (object[count].framedelay>=4.0f)
            {
            object[count].frame--;
            object[count].framedelay=0.0f;
            }
          }
        if (object[count].frame==0)
          if ((rnd()&255)==0)
            object[count].frame=5;

        if (object[count].frame>=5 && object[count].frame<=7)
          {
          object[count].framedelay+=1.0f;
          if (object[count].framedelay>=4.0f)
            {
            object[count].frame++;
            if (object[count].frame==8)
              object[count].frame=0;
            object[count].framedelay=0.0f;
            }
          }
        }
      if ((object[count].button&8)==8)
      if (object[count].frame<8)
        {
        object[count].frame=8;
        object[count].framedelay=0.0f;
        }

      if (object[count].frame>=8 && object[count].frame<=11)
        {
        object[count].framedelay+=1.0f;
        if (object[count].framedelay>=4.0f)
          {
          if ((object[count].button&8)==8)
            {
            if (object[count].frame<11)
              object[count].frame++;
            }
          else
            {
            if (object[count].frame>7)
              object[count].frame--;
            if (object[count].frame==7)
              object[count].frame=0;
            }
          object[count].framedelay=0.0f;
          }
        }
      }
    if (object[count].type==OBJ_TYPE_BEAST_OR_BOBBLE)
      {
      float mindistance = INFINITY;
      float distance;
      int closest_player = 0;

      // find closest player
      for (int i = 0; i < game.numofplayers; i++){
          subtractvectors(vec, object[count].position, object[i].position);
          distance = vectorlength(vec);
          if (distance < mindistance){
              closest_player = i;
              mindistance = distance;
          }
      }

      if (object[count].animationtype==0 || object[count].animationtype==1)
      if ((object[count].button&1)==1)
        {
        object[count].animationtype=2;
        object[count].frame=0;
        object[count].framedelay=0.0f;
        }
      if (object[count].beasttype==12)
      if (object[count].animationtype==0)
      if ((object[count].button&2)==2)
        {
        object[count].animationtype=1;
        object[count].frame=0;
        object[count].framedelay=0.0f;
        }
      if (object[count].animationtype==0)
        {
        if (object[count].link!=-1)
          {
          object[object[count].link].texturenum=animation[object[count].animationnum+32].stand[0];
          if (fabs(object[closest_player].position[0]-object[count].position[0])<1.5f)
          if (object[closest_player].position[1]>object[count].position[1])
            object[object[count].link].texturenum=animation[object[count].animationnum+32].stand[1];
          }

        object[count].framedelay+=0.1f;
        if (object[count].framedelay>=1.0f)
          {
          object[count].framedelay=0.0f;
          object[count].frame++;
          if (object[count].frame>=animation[object[count].animationnum].stand[1])
            object[count].frame=0;
          }
        object[count].texturenum=animation[object[count].animationnum].stand[0]+object[count].frame;
        if (vectorlength(object[count].velocity)>=0.01f)
          {
          object[count].animationtype=1;
          object[count].frame=0;
          object[count].framedelay=0.0f;
          }
        }
      if (object[count].animationtype==1)
        {
        if (object[count].link!=-1)
          {
          object[object[count].link].texturenum=animation[object[count].animationnum+32].walk[0];
          if (fabs(object[closest_player].position[0]-object[count].position[0])<1.5f)
          if (object[closest_player].position[1]>object[count].position[1])
            object[object[count].link].texturenum=animation[object[count].animationnum+32].stand[1];
          }
        if (object[count].beasttype!=12)
          object[count].framedelay+=vectorlength(object[count].velocity)*6.0f;
        else
          object[count].framedelay+=0.2f;
        if (object[count].framedelay>=1.0f)
          {
          object[count].framedelay=0.0f;
          object[count].frame++;
          if (object[count].frame>=animation[object[count].animationnum].walk[1])
            {
            object[count].frame=0;
            if (object[count].beasttype==12)
              object[count].animationtype=0;
            }
          }
        object[count].texturenum=animation[object[count].animationnum].walk[0]+object[count].frame;
        if (object[count].beasttype!=12)
        if (vectorlength(object[count].velocity)<0.01f)
          {
          object[count].animationtype=0;
          object[count].frame=0;
          object[count].framedelay=0.0f;
          }
        if (object[count].beasttype==12)
        if (object[count].frame==4)
        if (object[count].framedelay==0.0f)
          {
          addvectors(vec,object[count].position,yaxis);
          createbeast(11,vec,1.0f,1.0f,1.0f,0.2f);
          object[numofobjects-1].timetolive=512;
          object[numofobjects-1].link=-1;
          for (count2=0;count2<4;count2++)
            {
            particle[object[numofobjects-1].particle[count2]].velocity[1]=0.08f;
            if (object[count].direction==0)
              particle[object[numofobjects-1].particle[count2]].velocity[0]=-0.04f;
            else
              particle[object[numofobjects-1].particle[count2]].velocity[0]=0.04f;
            }
          }
        }
      if (object[count].animationtype==2)
        {
        if (object[count].link!=-1)
          object[object[count].link].texturenum=animation[object[count].animationnum+32].attack[0];

        object[count].framedelay+=0.2f;
        if (object[count].framedelay>=1.0f)
          {
          object[count].framedelay=0.0f;
          object[count].frame++;
          if (object[count].frame>=animation[object[count].animationnum].attack[1])
            {
            object[count].animationtype=0;
            object[count].frame=0;
            }
          }
        object[count].texturenum=animation[object[count].animationnum].attack[0]+object[count].frame;

        if (object[count].frame==1)
        if (object[count].framedelay==0.0f)
          {
          if (object[count].beasttype<3 || object[count].beasttype==8 || object[count].beasttype==11 || object[count].beasttype==16)
            playsound(11,object[count].position,NULL,0.25f,0,0.7f,-1,0);
          else if (object[count].beasttype==7 || object[count].beasttype==13)
            playsound(12,object[count].position,NULL,0.5f,0,1.0f,-1,0);
          else
            playsound(13,object[count].position,NULL,0.5f,0,1.0f,-1,0);
          }

        if (object[count].frame==5)
          {
          //if (object[count].beasttype!=7 && object[count].beasttype!=13)
            {
            scale=1.25f;
            if (object[count].beasttype<3 || object[count].beasttype==8 || object[count].beasttype==11 || object[count].beasttype==16)
              scale=0.75f;
            if (object[count].beasttype==15)
              scale=1.5f;
            if (object[count].beasttype==7 || object[count].beasttype==13)
              scale=3.0f;
  
            if (object[count].direction==0)
              scaleaddvectors(vec,object[count].position,object[count].orientation[0],-scale);
            if (object[count].direction==1)
              scaleaddvectors(vec,object[count].position,object[count].orientation[0],scale);
            if (lineintersectobject(intersectpoint,normal,&scale,object[count].position,vec,closest_player,particlelist))
              {
              scale=0.06f;
              if (object[count].beasttype<3)
                scale=0.03f;
              if (object[count].beasttype==15)
                scale=0.15f;
  
              if (object[count].direction==0)
                {
                scaleaddvectors(particle[particlelist[0]].velocity,particle[particlelist[0]].velocity,object[count].orientation[0],-scale);
                scaleaddvectors(particle[particlelist[1]].velocity,particle[particlelist[1]].velocity,object[count].orientation[0],-scale);
                }
              if (object[count].direction==1)
                {
                scaleaddvectors(particle[particlelist[0]].velocity,particle[particlelist[0]].velocity,object[count].orientation[0],scale);
                scaleaddvectors(particle[particlelist[1]].velocity,particle[particlelist[1]].velocity,object[count].orientation[0],scale);
                }
              if (object[count].beasttype<3 || object[count].beasttype==11)
                object[closest_player].hitpoints-=20;
              else
                object[closest_player].hitpoints-=40;
              if (object[count].beasttype==15)
                object[closest_player].hitpoints-=40;
              }
            }
          if (object[count].beasttype==7 || object[count].beasttype==13)
          if (object[count].framedelay==0.0f)
            {
            if (object[count].beasttype==7)
              createbeast(2,object[count].position,1.0f,1.0f,1.0f,0.2f);
            else
              createbeast(8,object[count].position,1.0f,1.0f,1.0f,0.2f);
            object[numofobjects-1].timetolive=512;
            object[numofobjects-1].link=-1;
            for (count2=0;count2<4;count2++)
              {
              if (object[count].direction==0)
                particle[object[numofobjects-1].particle[count2]].velocity[0]=-0.08f;
              else
                particle[object[numofobjects-1].particle[count2]].velocity[0]=0.08f;
              }
            }
          }
        }
      if (object[count].timetolive>=0 && object[count].timetolive<45)
        {
        if (object[count].timetolive==44)
          {
          if (object[count].beasttype<3 || object[count].beasttype==8 || object[count].beasttype==11 || object[count].beasttype==16)
            {
            game.score[0]+=100*game.combo;
            createsprite(100*game.combo,object[count].position);
            game.combo++;
            game.combodelay+=100;
            if (game.combodelay>250)
              game.combodelay=250;
            playsound(7,object[count].position,NULL,1.0f,0,1.0f,-1,0);
            }
          else
            {
            game.score[0]+=200*game.combo;
            createsprite(200*game.combo,object[count].position);
            game.combo++;
            game.combodelay+=100;
            if (game.combodelay>250)
              game.combodelay=250;
            playsound(19,object[count].position,NULL,1.0f,0,1.0f,-1,0);
            }

          for (count2=0;count2<10;count2++)
            {
            vec[0]=(float)((rnd()&255)-127)/1270.0f;
            vec[1]=(float)((rnd()&255)-127)/1270.0f;
            vec[2]=0.0f;
            addvectors(vec,vec,object[count].velocity);

            createparticle(5,object[count].position,vec,0.25f,-1,100+(rnd()&63));
            particle[numofparticles-1].rendersize=0.125+(float)(rnd()&127)/1000.0f;
            particle[numofparticles-1].texturenum=367;
            if (object[count].beasttype==11 || object[count].beasttype==12)
              particle[numofparticles-1].texturenum=364;
            if (object[count].beasttype==0 || object[count].beasttype==3 || object[count].beasttype==4)
              particle[numofparticles-1].texturenum=365;
            }
          }

        if (object[count].link!=-1)
          {
          object[object[count].link].texturenum=animation[object[count].animationnum+32].die[0];
          if (object[object[count].link].timetolive>150)
            object[object[count].link].timetolive=150;
          }
        object[count].animationtype=3;
        object[count].frame=8-object[count].timetolive/5;
        object[count].framedelay=0.0f;
        object[count].texturenum=animation[object[count].animationnum].die[0]+object[count].frame;
        }
      }
    }
  if (game.levelnum==34)
  if (object[1].type==OBJ_TYPE_BOX)
  if ((rnd()&255)==0)
    {
    object[1].texturenum=64+(rnd()&3);
    }
  }

void loadanimationpart(char* animationpartfilename, int animationpart[2], int animationlength){
    char filename_with_folder[256];
    char filename[32] = "bibatk01.png";
    animationpart[0]=numofanimations;
    animationpart[1]=animationlength;
    strcpy(filename,animationpartfilename);
    for (int count=1;count<=animationpart[1];count++){
        filename[6]=48+(count/10)%10;
        filename[7]=48+count%10;
        sprintf(filename_with_folder, "../animation/%s", filename); // MAYBE: add ability to list several datapacks and try loading them one by one
	    filename_with_folder[255] = 0; // safety first
        loadtexture(numofanimations,filename_with_folder,0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
        numofanimations++;
    }
}

void loadheadpart(int* numberinanimationpart, char* filename, int framenum){
    filename[7] = 48 + framenum; // "mumhed00.png" -> "mumhed01.png"
    *numberinanimationpart=numofanimations;

    char filename_with_folder[256];
	sprintf(filename_with_folder, "../animation/%s", filename); // MAYBE: add ability to list several datapacks and try loading them one by one
	filename_with_folder[255] = 0; // safety first

    loadtexture(numofanimations,filename_with_folder,0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    numofanimations++;
}

void loadanimation(int animationnum, char* standname, int standlength, char* walkname, int walklength, char* attackname, int attacklength, char* diename, int dielength, int head_needed, char* headname){
    animation[animationnum].loaded=1;

    loadanimationpart(standname, animation[animationnum].stand, standlength);
    loadanimationpart(walkname, animation[animationnum].walk, walklength);
    loadanimationpart(attackname, animation[animationnum].attack, attacklength);
    loadanimationpart(diename, animation[animationnum].die, dielength);

    if (head_needed){
        animationnum+=32;

        char filename[32]="bibatk01.png";
        strcpy(filename, headname);

        loadheadpart(&animation[animationnum].stand[0], filename, 1);
        loadheadpart(&animation[animationnum].walk[0], filename, 2);
        loadheadpart(&animation[animationnum].attack[0], filename, 3);
        loadheadpart(&animation[animationnum].stand[1], filename, 5);
        loadheadpart(&animation[animationnum].die[0], filename, 6);
    }
}

void loadanimations(void)
  {
  numofanimations=ANIMATIONS_START_TEXTURE;
  loadanimation(0,  "bibsta", 6, "bibwlk", 9, "bibatk", 9, "nibdie", 9, 0, "");
  loadanimation(1,  "nibsta", 6, "nibwlk", 9, "nibatk", 9, "nibdie", 9, 0, "");
  loadanimation(2,  "ribsta", 6, "ribwlk", 9, "ribatk", 9, "ribdie", 9, 0, "");
  loadanimation(3,  "mumsta", 6, "mumwlk", 9, "mumatk", 9, "mumdie", 9, 1, "mumhed01.png");
  loadanimation(4,  "sklsta", 6, "sklwlk", 9, "sklatk", 9, "skldie", 9, 1, "sklhed01.png");
  loadanimation(5,  "zomsta", 6, "zomwlk", 9, "zomatk", 9, "zomdie", 9, 1, "zomhed01.png");
  loadanimation(6,  "gimsta", 6, "gimwlk", 9, "gimatk", 9, "gimdie", 9, 1, "gimhed01.png");
  loadanimation(7,  "vissta", 6, "viswlk", 9, "visatk", 9, "visdie", 9, 0, "");
  loadanimation(8,  "aibsta", 6, "aibwlk", 9, "aibatk", 9, "aibdie", 9, 0, "");
  loadanimation(9,  "altsta", 6, "altwlk", 9, "altatk", 9, "altdie", 9, 1, "gimhed01.png");
  loadanimation(10, "frksta", 6, "frkwlk", 9, "frkatk", 9, "frkdie", 9, 1, "frkhed01.png");
  loadanimation(11, "pibsta", 6, "pibwlk", 9, "pibatk", 9, "pibdie", 9, 0, "");
  loadanimation(12, "sepsta", 6, "sepspw", 9, "sepatk", 8, "sepdie", 9, 0, "");
  loadanimation(13, "sissta", 6, "siswlk", 9, "sisatk", 9, "sisdie", 9, 0, "");
  loadanimation(14, "ghosta", 5, "ghowlk", 5, "ghoatk", 8, "ghodie", 9, 0, "");
  loadanimation(15, "bassta", 6, "baswlk", 9, "basatk", 9, "basdie", 9, 0, "");
  loadanimation(16, "satsta", 6, "satwlk", 9, "satatk", 9, "ribdie", 9, 0, "");
  }
