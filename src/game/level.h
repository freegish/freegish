#ifndef GISH_GAME_LEVEL_H
#define GISH_GAME_LEVEL_H
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

int lineintersectlevel(float *intersectpoint,float *normal,float *scale,float *startpoint,float *endpoint);
void savelevel(char *filename);
int loadlevel(char *filename);
void createlevel(void);
void getlevellines(int objectnum);
void setuplevellines(int xstart,int ystart,int xend,int yend);
void loadleveltextures(void);
int lineintersectline3(float *intersectpoint,float *normal,float *scale,float *startpoint,float *endpoint,float *vertex1,float *vertex2);
int pointintersectlevel(float *intersectpoint,float *normal,float *scale,float *point);
int changetilesetdir(void);
void encryptdata(unsigned int code,unsigned int codepair,int cryptdatasize);
void decryptdata(unsigned int code,int cryptdatasize);
void byteswapdata(int cryptdatasize);

typedef enum {
        GAMETYPE_CAMPAIGN = 0,
        GAMETYPE_COLLECTION = 1,
        GAMETYPE_2FOOTBALL = 10,
        GAMETYPE_2SUMO = 11,
        GAMETYPE_2GREED = 12,
        GAMETYPE_2DUEL = 13,
        GAMETYPE_2DRAGSTER = 14,
        GAMETYPE_2COLLECTION = 15,
        GAMETYPE_2RACING = 16,
        GAMETYPE_4FOOTBALL = 17,
        GAMETYPE_4SUMO = 18,
} gametype;

extern int GAMETYPE_COUNT;
extern const char* GAMETYPE_NAMES[];

typedef enum {
        LEVELAREA_WIN = 0,

        LEVELAREA_WIN_GOOD = 1,
        LEVELAREA_WIN_BAD = 2,

        LEVELAREA_WARPZONE = 1,
        LEVELAREA_WARPZONE2 = 2,
        LEVELAREA_WARPZONE3 = 3,

        LEVELAREA_TOUCHDOWN_GRAY = 0,
        LEVELAREA_FIELDGOAL_GRAY = 1,
        LEVELAREA_TOUCHDOWN_GISH = 2,
        LEVELAREA_FIELDGOAL_GISH = 3,

        LEVELAREA_SUMO_DEATH = 0,

        LEVELAREA_DRAGSTER_GISH = 0,
        LEVELAREA_DRAGSTER_GRAY = 1,
        LEVELAREA_DRAGSTER_GREEN = 2,
        LEVELAREA_DRAGSTER_YELLOW = 3,
        LEVELAREA_DRAGSTER_RED = 4,

        LEVELAREA_TUTORIAL_LINE_1_TO_2 = 1,
        LEVELAREA_TUTORIAL_LINE_3_TO_4 = 2,
        LEVELAREA_TUTORIAL_LINE_5_TO_7 = 3,
        LEVELAREA_TUTORIAL_LINE_8_TO_12 = 4,
        LEVELAREA_TUTORIAL_LINE_13_TO_14 = 5,
        LEVELAREA_TUTORIAL_LINE_15_TO_19 = 6
} levelarea;

typedef enum {
  LOADLEVELRESULT_OK = 0,
  LOADLEVELRESULT_UNKNOWN = 1,
  LOADLEVELRESULT_FILEERROR = 2,
  LOADLEVELRESULT_TOO_MANY_OBJECTS = 3,
  LOADLEVELRESULT_TOO_MANY_ROPES = 4,
  LOADLEVELRESULT_TEXTURE_SIZEX_TOO_BIG = 5,
  LOADLEVELRESULT_TOO_MANY_BLOCK_LINES = 6
} loadlevelresult;

typedef struct
  {
  int blocknum;
  int blockx;
  int blocky;
  float friction;
  } _collision;

typedef struct
  {
  char background[32];
  int tileset;
  gametype gametype;
  int time;
  int area[64][4];
  unsigned char backgrid[256][256];
  unsigned char grid[256][256];
  unsigned char foregrid[256][256];
  int gridflags[256][256];
  int gridmod[256][256];
  float gridforce[256][256];
  float startposition[3];
  float ambient[4][3];
  int numofobjects;
  struct
    {
    int type;
    int texturenum;
    int link;
    float position[3];
    float angle;
    float size[2];
    float mass;
    float friction;
    float vertex[16][3];
    int lighttype;
    float lightcolor[3];
    float lightintensity;
    int ai;
    } object[256];
  int numofropes;
  struct
    {
    int type;
    int texturenum;
    int obj1;
    int obj1part;
    int obj2;
    int obj2part;
    } rope[1024];
  } _level;

extern unsigned int cryptdata[1048576];

extern int textureused[512];

extern _collision collision;

extern int levelblocknum;
extern float levelfriction;

extern _level level;

#endif /* GISH_GAME_LEVEL_H */
