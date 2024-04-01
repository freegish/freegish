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
void loadlevel(char *filename);
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
