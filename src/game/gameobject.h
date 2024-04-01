#ifndef GISH_GAME_GAMEOBJECT_H
#define GISH_GAME_GAMEOBJECT_H
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

void createbox(float position[3],float sizex,float sizey,float mass,float friction);
void createtarboy(float position[3]);
void createwheel(float position[3],float sizex,float sizey,float mass,float friction,int anchor);
void setupobjecttypes(void);
void createrope(int type,int particlenum,int particlenum2,int objectnum,int objectnum2,int texturenum);
void createanchor(float position[3]);
void createbutton(float position[3],float mass);
void createswitch(float position[3],float mass,int rotate);
void createbeast(int beasttype,float position[3],float sizex,float sizey,float mass,float friction);
void createbobble(int beasttype,float position[3],float sizex,float sizey,float mass,float friction);
void createhead(float position[3],float sizex,float sizey,float mass,float friction);
void createamber(float position[3]);
void createareaswitch(float position[3],float sizex,float sizey);
void creategenerator(float position[3],float mass);
void deleteobject(int objectnum);
void deleterope(int ropenum);
void objecttimetolive(void);
void createcar(float position[3],float sizex,float sizey,float mass,float friction);

// what more to dehardcode:
// bond types
// physicstemp.bond types
// particle types
// menuitem types
// beast types


typedef enum
{
    LVL_OBJ_TYPE_GISH = 1,
    LVL_OBJ_TYPE_BOX = 2,
    LVL_OBJ_TYPE_MIDDLE_FIXED_BOX = 3,
    LVL_OBJ_TYPE_LEFT_FIXED_BOX_OR_CAR = 4,
    LVL_OBJ_TYPE_RIGHT_FIXED_BOX = 5,
    LVL_OBJ_TYPE_WHEEL = 6,
    LVL_OBJ_TYPE_ANCHORED_WHEEL = 7,
    LVL_OBJ_TYPE_LIGHT_OR_ANCHOR = 8,
    LVL_OBJ_TYPE_BUTTON = 9,
    LVL_OBJ_TYPE_ONE_TIME_BUTTON = 10,
    LVL_OBJ_TYPE_SWITCH_UP = 11,
    LVL_OBJ_TYPE_SWITCH_RIGHT = 12,
    LVL_OBJ_TYPE_SWITCH_DOWN = 13,
    LVL_OBJ_TYPE_SWITCH_LEFT = 14,
    LVL_OBJ_TYPE_AREASWITCH = 15,
    LVL_OBJ_TYPE_ONE_TIME_AREASWITCH = 16,
    LVL_OBJ_TYPE_GENERATOR = 17,
    LVL_OBJ_TYPE_SECRET_AREASWITCH = 18,
    LVL_OBJ_TYPE_MONSTER_BEGIN = 20,
    LVL_OBJ_TYPE_MONSTER_END = 36,

} level_object_types;

extern const char* LVL_OBJ_NAMES[]; // don't forget to rename things if changed enum

typedef enum
{
    OBJ_TYPE_GISH = 1,
    OBJ_TYPE_BOX = 2,
    OBJ_TYPE_WHEEL = 3,
    OBJ_TYPE_BEAST_OR_BOBBLE = 4,
    OBJ_TYPE_HEAD = 5,
    OBJ_TYPE_AMBER = 6,
    OBJ_TYPE_ANCHOR = 8,
    OBJ_TYPE_BUTTON = 9,
    OBJ_TYPE_SWITCH = 10,
    OBJ_TYPE_GENERATOR = 15,
    OBJ_TYPE_AREASWITCH = 16,
    OBJ_TYPE_CAR = 20,

} gameobject_types;

typedef enum
{
    WEAK_ROPE = 1,
    STRONG_ROPE = 2,
    WEAK_CHAIN = 3,
    STRONG_CHAIN = 4,
    PUSHING_PISTON = 5,
    HALF_PUSHED_PUSHING_PISTON = 6,
    PULLING_PISTON = 7,
    HALF_PULLED_PULLING_PISTON = 8,
    BAR = 9,
    SPRING = 10,

} rope_types;
extern const char* ROPE_TYPE_NAMES[]; // don't forget to rename things if changed enum

typedef enum
{
    LIGHT_DEFAULT_ON = 1,
    LIGHT_DEFAULT_OFF = 2, // idk, maybe something else
    FLICKERING_LIGHT = 3,
} light_types;

typedef struct
  {
  int type;
  int texturenum;
  int timetolive;
  int prevhitpoints;
  int hitpoints;
  int damagedelay;
  int animationnum;
  int animationtype;
  int frame;
  float framedelay;
  int direction;
  int link;
  float mass;
  float axis[8];
  int button;
  float data[8];
  int idata[8];
  int soundnum[4];
  float size[2];
  float position[3];
  float prevposition[3];
  int numoforientations;
  float orientation[3][3];
  float angle;
  float velocity[3];
  float prevvelocity[3];
  float radius;
  float friction;
  float frictionapplied;
  int numofparticles;
  int particle[32];
  int particlestick[32];
  float particlestickposition[32][3];
  float particlesticknormal[32][3];
  int numofcdlines;
  int cdline[32][2];
  float texcoord[4][2];
  int lighttype;
  int lighton;
  float lightcolor[3];
  float lightintensity;
  float lightintensitymax;
  int rotate;
  int ai;
  int beasttype;
  } _object;

typedef struct
  {
  int collide[256];
  int numofpoints;
  struct
    {
    int type;
    float position[3];
    float mass;
    } point[64];
  int numoflinks;
  struct
    {
    int type;
    int part1;
    int part2;
    } link[256];
  } _objecttype;

typedef struct
  {
  int type;
  int part1;
  int part2;
  int bondnum;
  int link;
  int texturenum;
  float angle;
  float cycle;
  float cyclelength;
  float cyclecount;
  float range;
  } _rope;

extern int numofobjects;
extern _object object[512];
extern _objecttype objecttype[128];

extern int numofropes;
extern _rope rope[1024];

#endif /* GISH_GAME_GAMEOBJECT_H */
