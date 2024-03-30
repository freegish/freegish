#ifndef GISH_VIDEO_TEXTURE_H
#define GISH_VIDEO_TEXTURE_H
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

#include "../video/opengl.h"

int loadtexturepng(const char *filename, unsigned int **rgba, int *width, int *height);
int loadtexturetga(const char *filename, unsigned int **rgba, int *width, int *height);
int loadtexture(int texturenum,const char *filename,int mipmap,int wraps,int wrapt,int magfilter,int minfilter);
void loadtexturepartial(int texturenum,const char *filename,int startx,int starty,int sizex,int sizey);
void generatemipmap(int texturenum);
void setuptexture(int texturenum);
int texturecmp(int texturenum1, int texturenum2);
int load_all_textures(void);
void look_for_texture_in_folders(int texturenum, int numofloadedtextures);

// im going to try to lay out all the used textures
// TODO: replace this comment with a single function that loads all the needed textures, with appropriate comments about not used texture locations
// TODO: maybe create a system which loads textures only to unoccupied array places
// 0-255: current textures for the level, used in levelload
// 256-319: textures from the textures folder "text000.png"
// 320-324: cage1,2,3,4 and x.png
// 325-329: not used
// 330-335: lighting textures, setuplightingtextures()
// 336-359: not used
// 360-369: ropes, particles
// 370-382: face
// 383-389: not used
// 390-402: gray face
// 403-409: not used
// 410-423: b face
// 424-429: not used
// 430-443: r face
// 444-461: not used
// 462 complete
// 463-464: not used
// 465 breatalk
// 466-467: not used
// 468 lvlcomplete
// 469 gishy
// 470-481 gishtitle
// 482-511: not used
// 512-527: lots of talks: gish honey paunchy vis bas etc
// 528 cryptic sea
// 529 turbo
// 530-535: versus modes textures
// 536-539: not used
// 540-551: level
// 552-559: not used
// 560-571: congrats
// 572-579: not used
// 580-591: blank
// 592-599: not used
// 600-611: vsmode
// 612-619: not used
// 620-631: storymode
// 632-639: not used
// 640-645: border, loaded partially
// 
// 700-712: 0-12.png
// 
// 720-723: gishhud greyhud tarballs key
// 
// 820-823: versus modes ingame texture
// 824-???
// 1024-1607: animations
// 

typedef enum {
	EDITBLOCK_TEXTURE = 999,
	// ??? = 330,  // these are lighting textures, they are set up in lighting.c
	// ??? = 331,
	// ??? = 332,
	OLDSCHOOL2_TEXTURE = 333,
	OLDSCHOOL1_TEXTURE = 334,
	// ??? = 335,
	TEXT_START_TEXTURE = 1000,
	ANIMATIONS_START_TEXTURE = 1024,
	TEXTUREEDIT_START_TEXTURE = 1792,

} used_textures;

typedef struct
  {
  int sizex;
  int sizey;
  int mipmaplevels;
  int format;
  int wraps;
  int wrapt;
  int magfilter;
  int minfilter;
  int isalpha;
  int alphamap;
  int normalmap;
  int glossmap;
  GLuint glname;
  unsigned int *rgba[16];	// XXX: use uint32_t
  int glnamenormal;
  unsigned int *normal[16];	// XXX: use uint32_t
  int glnamegloss;
  unsigned char *gloss[16];
  char filename[256];
  } _texture;

typedef struct
  {
  unsigned char imagetypecode;
  short int imagewidth;
  short int imageheight;
  unsigned char pixeldepth;
  unsigned int imagedata[1024*1024];
  } _tgaheader;

extern char lasttextureloaded[32];
extern _texture texture[2048];
extern _tgaheader tgaheader;

#endif /* GISH_VIDEO_TEXTURE_H */
