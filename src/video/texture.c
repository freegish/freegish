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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <assert.h>

#include "../video/texture.h"
#include "../video/text.h" // just for drawing Loading... centered when loading all textures
#include "../game/animation.h"
#include "../game/english.h" // just for drawing Loading... when loading all textures
#include "../game/options.h"
#include "../game/debug.h"
#include "../sdl/endian.h"
#include "../sdl/file.h"
#include "../sdl/video.h" // just for updating screen when loading all textures

unsigned int *lastrgba = NULL;
int lastfullwidth, lastfullheight;
char lastfilename[256] = "";
_texture texture[2048];
_tgaheader tgaheader;

/*
 * This should handle a variety common PNG formats 
 * most importantly 8bit palletized with alpha channel
 */
int loadtexturepng(const char *filename, unsigned int **rgba, int *width, int *height)
{
	FILE *fp;
	int load_status;
	
	if((fp = fopen(filename, "rb")) == NULL)
	{
		if(debug_texture_load) fprintf(stderr, "Texture Load Failed: %s\n", filename);
		load_status = -1;
	}
	else
	{
		unsigned char header[8];
		fread(header, 1, 8, fp);
		if(png_sig_cmp(header, 0, 8))
		{
			if(debug_texture_load) fprintf(stderr, "PNG file not recognized: %s\n", filename);
			load_status = -2;
		}
		else
		{
			png_structp png_ptr;
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (png_ptr)
			{
				png_infop info_ptr;
				info_ptr = png_create_info_struct(png_ptr);
				if (info_ptr)
				{
					if(setjmp(png_jmpbuf(png_ptr)))
					{
						if(debug_texture_load) fprintf(stderr, "Error during init_io for %s\n", filename);
						load_status = -3;
					}
					else
					{
						int number_passes;
						png_init_io(png_ptr, fp);
						png_set_sig_bytes(png_ptr, 8);
						png_read_info(png_ptr, info_ptr);
						png_uint_32 texwidth = 0;
						png_uint_32 texheight = 0;
						int bit_depth = 0;
                        			int color_type = 0;
                        			int interlace_type = 0;
			                        png_get_IHDR(png_ptr, info_ptr, &texwidth, &texheight, &bit_depth, &color_type, 0, 0, 0);

						/* expand paletted colors into true rgb */
						if (color_type == PNG_COLOR_TYPE_PALETTE)
							png_set_expand(png_ptr);

						/* expand grayscale images to the full 8 bits */
						if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
							png_set_expand(png_ptr);

						/* expand images with transparency to full alpha channels */
						if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
                            			png_set_expand(png_ptr);

						/* tell libpng to strip 16 bit depth files down to 8 bits */
						if (bit_depth == 16)
							png_set_strip_16(png_ptr);

						/* fill upto 4 byte RGBA - we always want an alpha channel*/
						if (bit_depth == 8 && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
							png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

						// XXX: is this required? we're not handling interlaced PNGs ...
						if (interlace_type)
							number_passes = png_set_interlace_handling(png_ptr);
						else
							number_passes = 1;

						png_start_read_image(png_ptr);
						//png_read_update_info(png_ptr, info_ptr);

						if(setjmp(png_jmpbuf(png_ptr)))
						{
							if(debug_texture_load) fprintf(stderr, "Error during read_image for %s\n", filename);
							load_status = -4;
						}
						else
						{
							int w, h, y;
							w = texwidth;
							h = texheight;
							*width = w;
							*height = h;
							*rgba = (unsigned int *) malloc(w*h*4);
							memset(*rgba, 0, w*h*4);
							for(y = 0; y < h; y++)
								png_read_row(png_ptr, ((png_bytep)*rgba + (w*4*y)), NULL);
							load_status = 0;
						}
					}
					png_read_end(png_ptr, info_ptr);
					png_destroy_read_struct(&png_ptr, &info_ptr, (png_infop*)0);
				}
				free(info_ptr);
			}
			free(png_ptr);
		}
		fclose(fp);
	}
	return load_status;
}

int loadtexturetga(const char *filename, unsigned int **rgba, int *width, int *height)
  {
	bool isAlpha = false;
  int count,count2;
  int red,green,blue,alpha;
  unsigned char origin;
  FILE *fp;

  if ((fp=fopen(filename,"rb"))==NULL)
    {
    if (debug_texture_load) printf("Texture \"%s\" failed: fopen error\n",filename);
    return -1;
    }

  fseek(fp,2,SEEK_CUR);
  fread2(&tgaheader.imagetypecode,1,1,fp);
  if (tgaheader.imagetypecode!=2 && tgaheader.imagetypecode!=3)
    {
    if (debug_texture_load) printf("Texture \"%s\" failed: bad format\n",filename);
    fclose(fp);
    return -2;
    }

  fseek(fp,9,SEEK_CUR);
  fread2(&tgaheader.imagewidth,2,1,fp);
  fread2(&tgaheader.imageheight,2,1,fp);
  fread2(&tgaheader.pixeldepth,1,1,fp);
  fread2(&origin,1,1,fp);
  origin=(origin>>4)&3;

  isAlpha = false;

  for (count=0;count<tgaheader.imageheight;count++)
  for (count2=0;count2<tgaheader.imagewidth;count2++)
    {
    blue=fgetc(fp);
    green=fgetc(fp);
    red=fgetc(fp);
    if (tgaheader.pixeldepth==32)
      alpha=fgetc(fp);
    else
      alpha=255;

    if (alpha!=255)
      isAlpha = true;

    if (!bigendian)
      {
      if (origin==0)
        tgaheader.imagedata[(tgaheader.imageheight-1-count)*tgaheader.imagewidth+count2]=(alpha<<24)+(blue<<16)+(green<<8)+red;
      if (origin==1)
        tgaheader.imagedata[(tgaheader.imageheight-1-count)*tgaheader.imagewidth+(tgaheader.imagewidth-1-count2)]=(alpha<<24)+(blue<<16)+(green<<8)+red;
      if (origin==2)
        tgaheader.imagedata[count*tgaheader.imagewidth+count2]=(alpha<<24)+(blue<<16)+(green<<8)+red;
      if (origin==3)
        tgaheader.imagedata[count*tgaheader.imagewidth+(tgaheader.imagewidth-1-count2)]=(alpha<<24)+(blue<<16)+(green<<8)+red;
      }
    else
      {
      if (origin==0)
        tgaheader.imagedata[(tgaheader.imageheight-1-count)*tgaheader.imagewidth+count2]=(red<<24)+(green<<16)+(blue<<8)+alpha;
      if (origin==1)
        tgaheader.imagedata[(tgaheader.imageheight-1-count)*tgaheader.imagewidth+(tgaheader.imagewidth-1-count2)]=(red<<24)+(green<<16)+(blue<<8)+alpha;
      if (origin==2)
        tgaheader.imagedata[count*tgaheader.imagewidth+count2]=(red<<24)+(green<<16)+(blue<<8)+alpha;
      if (origin==3)
        tgaheader.imagedata[count*tgaheader.imagewidth+(tgaheader.imagewidth-1-count2)]=(red<<24)+(green<<16)+(blue<<8)+alpha;
      }
    }

  fclose(fp);

  //if ((tgaheader.imagewidth&(tgaheader.imagewidth-1))!=0)
  //  return -3;
  //if ((tgaheader.imageheight&(tgaheader.imageheight-1))!=0)
  //  return -4;

  *width = tgaheader.imagewidth;
  *height = tgaheader.imageheight;

  *rgba = (unsigned int *) malloc((*width) * (*height) * 4);

  memcpy(*rgba,tgaheader.imagedata, (*width) * (*height) * 4);

  return 0;
  }

char * getextension(char *filename)
{
	size_t len;
	char *extension;
	len = strlen(filename);
	extension = filename + len;
	while (*extension != '.' && extension != filename) { extension--; }
	if (extension == filename) extension = NULL;
	return extension;
}

bool hasextension(char *filename, const char *desiredextension)
{
	char * extension = getextension(filename);
	return extension != NULL && strcmp(extension, desiredextension) == 0;
}

void changeextension(char *filename, const char *newextension)
{
	char * extension = getextension(filename);
	if (extension == NULL) {
		return;
	}
	strcpy(extension, newextension);
}

int loadtexturefile(const char *filename, unsigned int **rgba, int *width, int *height)
{
	int result = -1;

	char filename_with_folder[256];
	sprintf(filename_with_folder, "%s/%s/texture/%s", datapacks_folder, loaded_datapack, filename); // MAYBE: add ability to list several datapacks and try loading them one by one
	filename_with_folder[255] = 0; // safety first

	char *newfilename = strdup(filename_with_folder);
	char *extension = getextension(newfilename);

	if (extension == NULL) {
		size_t len;
		char *newnewfilename;
		if (debug_texture_load)
			printf("No extension found in filename \"%s\". Appending PNG extension.\n", filename);

		len = strlen(newfilename);
		newnewfilename = (char*) malloc(sizeof(char) * (len + 5));
		strcpy(newnewfilename, newfilename);
		free(newfilename);
		newfilename = newnewfilename;

		strcpy(newfilename + len, ".png");
	}

	if (hasextension(newfilename, ".png")) {
		result = loadtexturepng(newfilename, rgba, width, height);
		if (result < 0) {
			if (debug_texture_load)
				printf("\"%s\" failed to load. Trying TGA extension instead.\n", newfilename);

			changeextension(newfilename, ".tga");
			result = loadtexturetga(newfilename, rgba, width, height);
		}
	}
	else if (hasextension(newfilename, ".tga")) {
		result = loadtexturetga(newfilename, rgba, width, height);
		if (result < 0) {
			if (debug_texture_load)
				printf("\"%s\" failed to load. Trying PNG extension instead.\n", newfilename);
			changeextension(newfilename, ".png");
			result = loadtexturepng(newfilename, rgba, width, height);
		}
	}
	else {
		printf("Extension of \"%s\" not recognized.\n", newfilename);
		result = -1;
	}
	if (result < 0) {
		if (debug_texture_load)
			printf("\"%s\" failed to load. Giving up.\n", newfilename);

		(*width) = 0;
		(*height) = 0;
	}

	free(newfilename);

	return result;
}

int loadtexture(int texturenum,const char *filename,int mipmap,int wraps,int wrapt,int magfilter,int minfilter)
{
	int result;

	if (debug_texture_load) printf("Loading \"%s\" into #%i...\n", filename, texturenum);

	if (texture[texturenum].rgba[0])
	{
		free(texture[texturenum].rgba[0]);
		texture[texturenum].rgba[0] = NULL;
	}

	result = loadtexturefile(filename, &(texture[texturenum].rgba[0]), &(texture[texturenum].sizex), &(texture[texturenum].sizey));

	texture[texturenum].mipmaplevels=1;
	texture[texturenum].format=GL_RGBA;
	texture[texturenum].alphamap=1;
	texture[texturenum].normalmap=0;
	texture[texturenum].glossmap=0;
	texture[texturenum].wraps=wraps;
	texture[texturenum].wrapt=wrapt;
	texture[texturenum].magfilter=magfilter;
	texture[texturenum].minfilter=minfilter;

	if (mipmap) {
		generatemipmap(texturenum);
	}
	setuptexture(texturenum);

	memset(texture[texturenum].filename, 0, 256);
	strcpy(texture[texturenum].filename,filename);

	return result;
}
void loadtexturepartial(int texturenum,const char *filename,int startx,int starty,int sizex,int sizey)
{
	int result;
	unsigned int *fullrgba;
	int fullwidth, fullheight;
	int x, y;

	if (debug_texture_load) printf("Loading \"%s\" partially into #%i...\n", filename, texturenum);

	if (texture[texturenum].rgba[0])
	{
		free(texture[texturenum].rgba[0]);
		texture[texturenum].rgba[0] = NULL;
	}

	if (strcmp(lastfilename, filename) == 0)
	{
		fullrgba = lastrgba;
		fullwidth = lastfullwidth;
		fullheight = lastfullheight;
	}
	else
	{
		result = loadtexturefile(filename, &fullrgba, &fullwidth, &fullheight);

		if (result != 0)
			return;

		if (lastrgba)
			free(lastrgba);

		strcpy(lastfilename, filename);
		lastrgba = fullrgba;
		lastfullwidth = fullwidth;
		lastfullheight = fullheight;
	}

	texture[texturenum].rgba[0] = (unsigned int *)malloc(sizex*sizey*4);

	memset(texture[texturenum].rgba[0], 128, sizex*sizey*4);
	{
		int maxy = min(fullheight, starty+sizey)-starty;
		for (y = 0; y < maxy; y++)
		{
			int maxx = min(fullwidth, startx+sizex)-startx;
			for (x = 0; x < maxx; x++)
			{
				texture[texturenum].rgba[0][x + y * sizex]=fullrgba[(startx+x) + (starty+y) * fullwidth];
			}
		}
	}

	texture[texturenum].sizex = sizex;
	texture[texturenum].sizey = sizey;
	texture[texturenum].mipmaplevels=1;
	texture[texturenum].format=GL_RGBA;
	texture[texturenum].alphamap=1;
	texture[texturenum].normalmap=0;
	texture[texturenum].glossmap=0;
	texture[texturenum].wraps=GL_CLAMP;
	texture[texturenum].wrapt=GL_CLAMP;
	texture[texturenum].magfilter=GL_NEAREST;
	texture[texturenum].minfilter=GL_NEAREST;
	texture[texturenum].filename[0] = 0;
	setuptexture(texturenum);
}

void generatemipmap(int texturenum)
  {
  int count,count2,count3,count4;
  int mipmaplevel;
  int mipmaplevelmax;
  unsigned int red,green,blue,alpha,temp;

  if (texture[texturenum].format!=GL_RGBA)
    return;

  mipmaplevelmax=1;
  while ((texture[texturenum].sizex>>mipmaplevelmax)!=0)
    mipmaplevelmax++;

  texture[texturenum].mipmaplevels=mipmaplevelmax;

  for (mipmaplevel=1;mipmaplevel<mipmaplevelmax;mipmaplevel++)
    {
    free(texture[texturenum].rgba[mipmaplevel]);
    texture[texturenum].rgba[mipmaplevel]=(unsigned int *) malloc((texture[texturenum].sizex>>mipmaplevel)*(texture[texturenum].sizey>>mipmaplevel)*4);
    if (texture[texturenum].normalmap)
      {
      free(texture[texturenum].normal[mipmaplevel]);
      texture[texturenum].normal[mipmaplevel]=(unsigned int *) malloc((texture[texturenum].sizex>>mipmaplevel)*(texture[texturenum].sizey>>mipmaplevel)*4);
      }
    for (count=0;count<(texture[texturenum].sizey>>mipmaplevel);count++)
      for (count2=0;count2<(texture[texturenum].sizex>>mipmaplevel);count2++)
        {
        red=0;
        green=0;
        blue=0;
        alpha=0;
        for (count3=0;count3<2;count3++)
        for (count4=0;count4<2;count4++)
          {
          temp=texture[texturenum].rgba[mipmaplevel-1][(count*2+count3)*(texture[texturenum].sizex>>(mipmaplevel-1))+(count2*2+count4)];
          red+=temp&0xFF;
          green+=(temp>>8)&0xFF;
          blue+=(temp>>16)&0xFF;
          alpha+=(temp>>24)&0xFF;
          }

        red>>=2;
        green>>=2;
        blue>>=2;
        alpha>>=2;
        texture[texturenum].rgba[mipmaplevel][count*(texture[texturenum].sizex>>mipmaplevel)+count2]=(alpha<<24)+(blue<<16)+(green<<8)+red;

        if (texture[texturenum].normalmap)
          {
          red=0;
          green=0;
          blue=0;
          alpha=0;
          for (count3=0;count3<2;count3++)
          for (count4=0;count4<2;count4++)
            {
            temp=texture[texturenum].normal[mipmaplevel-1][(count*2+count3)*(texture[texturenum].sizex>>(mipmaplevel-1))+(count2*2+count4)];
            red+=temp&0xFF;
            green+=(temp>>8)&0xFF;
            blue+=(temp>>16)&0xFF;
            alpha+=(temp>>24)&0xFF;
            }
  
          red>>=2;
          green>>=2;
          blue>>=2;
          alpha>>=2;
          texture[texturenum].normal[mipmaplevel][count*(texture[texturenum].sizex>>mipmaplevel)+count2]=(alpha<<24)+(blue<<16)+(green<<8)+red;
          }
        }
    }
  }

void setuptexture(int texturenum)
  {
  int count,count2;

  glBindTexture(GL_TEXTURE_2D,texture[texturenum].glname);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture[texturenum].wraps);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture[texturenum].wrapt);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture[texturenum].magfilter);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture[texturenum].minfilter);

  texture[texturenum].isalpha=0;

  for (count=0;count<texture[texturenum].sizey;count++)
  for (count2=0;count2<texture[texturenum].sizex;count2++)
    {
    if (!bigendian)
      {
      if ((texture[texturenum].rgba[0][count*texture[texturenum].sizex+count2]>>24)!=255)
        {
        texture[texturenum].isalpha=1;
        texture[texturenum].alphamap=1;
        }
      }
    else
      {
      if ((texture[texturenum].rgba[0][count*texture[texturenum].sizex+count2]&255)!=255)
        {
        texture[texturenum].isalpha=1;
        texture[texturenum].alphamap=1;
        }
      }
    }

  for (count=0;count<texture[texturenum].mipmaplevels;count++)
    glTexImage2D(GL_TEXTURE_2D,count,texture[texturenum].format,(texture[texturenum].sizex>>count),(texture[texturenum].sizey>>count),
                 0,texture[texturenum].format,GL_UNSIGNED_BYTE,texture[texturenum].rgba[count]);

  if (texture[texturenum].normalmap)
    {
    glBindTexture(GL_TEXTURE_2D,texture[texturenum].glnamenormal);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture[texturenum].wraps);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture[texturenum].wrapt);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture[texturenum].magfilter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture[texturenum].minfilter);

    for (count=0;count<texture[texturenum].mipmaplevels;count++)
      glTexImage2D(GL_TEXTURE_2D,count,GL_RGBA,(texture[texturenum].sizex>>count),(texture[texturenum].sizey>>count),
                   0,GL_RGBA,GL_UNSIGNED_BYTE,texture[texturenum].normal[count]);
    }
  if (texture[texturenum].glossmap)
    {
    glBindTexture(GL_TEXTURE_2D,texture[texturenum].glnamegloss);
  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture[texturenum].wraps);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture[texturenum].wrapt);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture[texturenum].magfilter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture[texturenum].minfilter);
  
    for (count=0;count<texture[texturenum].mipmaplevels;count++)
      glTexImage2D(GL_TEXTURE_2D,count,GL_ALPHA,(texture[texturenum].sizex>>count),(texture[texturenum].sizey>>count),
                   0,GL_ALPHA,GL_UNSIGNED_BYTE,texture[texturenum].gloss[count]);
    }
  }

int texturecmp(int texturenum1, int texturenum2){
	// 0 when identical, 1 when different
    int same = 0;
	int sx1 = texture[texturenum1].sizex;
	int sy1 = texture[texturenum1].sizey;
	int sx2 = texture[texturenum2].sizex;
	int sy2 = texture[texturenum2].sizey;
	same = sx1 == sx2 && sy1 == sy2;
	if (!same) return 1;
	for (int i=0;i<texture[texturenum1].sizex*texture[texturenum1].sizey;i++){
		int r1 = texture[texturenum1].rgba[0][i]; // can only compare the largest mipmap level
		int r2 = texture[texturenum2].rgba[0][i];
	    same = r1 == r2;
		if (!same)
			return 1;
	}
    return 0;
}

int load_all_text_textures(void){
	int numoftextures = 0;
	char texturelist[1024][32];

	char* folders[] = {
	  "texture",
	  "tile01/texture",
	  "tile02/texture",
	  "tile03/texture",
	  "tile04/texture",
	  "tile05/texture",
	  "tile06/texture",
	  "tile07/texture",
	};
	char* extensions[] = {
	  "text*.png",
	  "text*.tga",
	};
	char current_folder[256];
	char current_texture[256];
	int current_texture_count = ANIMATIONS_START_TEXTURE;

	// unload all the animations
	for (int count=0; count<=16; count++){
	    animation[count].loaded = 2;
	}
	numofanimations = ANIMATIONS_START_TEXTURE;

	// load all existing textures
	for (int foldercount = 0; foldercount < 8; foldercount++)
	for (int extensioncount = 0; extensioncount < 2; extensioncount++){
		strcpy(current_folder, folders[foldercount]);
	    listfiles(current_folder, extensions[extensioncount], texturelist, 0); // should memset 0 texturelist or not??
	    numoftextures = 0;
        while (texturelist[numoftextures][0]!=0)
            numoftextures++;
        for (int count=0;count<numoftextures;count++){
			sprintf(current_texture, "%s/%s", folders[foldercount], texturelist[count]);
			assert(current_texture_count < 2048);
            loadtexture(current_texture_count, texturelist[count],0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
			current_texture_count++;
        }
	}
	return current_texture_count - ANIMATIONS_START_TEXTURE;
}

void load_all_static_textures(void){
    int count;
    char loadfilename[32]="loading00.png";
    char texfilename[32]="text000.png";

    count=(rand()%11)+1;
    loadfilename[7]=48+count/10;
    loadfilename[8]=48+count%10;
    loadbackground(800,loadfilename);

    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    setuptextdisplay();
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    displaybackground(800);
    drawtext(TXT_LOADING,(320|TEXT_CENTER),448,16,1.0f,1.0f,1.0f,1.0f);
    SDL_GL_SwapWindow(globalwindow);

    for (count=0;count<64;count++){
        texfilename[4]=48+(count/100)%10;
        texfilename[5]=48+(count/10)%10;
        texfilename[6]=48+count%10;
        loadtexture(count+256,texfilename,0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    }
    loadtexture(256+64,"cage1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(256+65,"cage2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(256+66,"cage3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(256+67,"cage4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(256+68,"x.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    setuplightingtextures();

    loadbackground(470,"gishtitle.png");

    loadtexture(462,"complete.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(465,"breatalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(468,"lvlcomplete.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(469,"gishy.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(512,"gishtalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(513,"honeytalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(514,"paunchytalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(515,"vistalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(516,"bastalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(517,"sistalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(518,"heratalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(519,"badending.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(520,"goodending.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(521,"sewerimg.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(522,"caveimg.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(523,"hellimg.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(524,"egyptimg.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(525,"churchimg.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(526,"warpimg.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(527,"satantalk.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(528,"crypticsea.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(529,"turbo.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(530,"sumo.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(531,"football.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(532,"greed.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(533,"duel.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(534,"dragster.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(535,"collect.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadbackground(540,"level.png");

    loadbackground(560,"congrats.png");
    loadbackground(580,"blank.png");

    loadbackground(600,"vsmode.png");
    loadbackground(620,"storymode.png");

    loadtexturepartial(640,"border.png",0,0,256,256);
    loadtexturepartial(641,"border.png",256,0,256,256);
    loadtexturepartial(642,"border.png",512,0,256,256);
    loadtexturepartial(643,"border.png",0,256,256,256);
    loadtexturepartial(644,"border.png",256,256,256,256);
    loadtexturepartial(645,"border.png",512,256,256,256);

    loadtexture(820,"gishwin.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(821,"graywin.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(822,"fight.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(823,"ready.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(700,"0.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(701,"1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(702,"2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(703,"3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(704,"4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(705,"5.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(706,"6.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(707,"7.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(708,"8.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(709,"9.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(710,"10.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(711,"11.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(712,"12.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(720,"gishhud.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(721,"greyhud.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(722,"tarballs.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(723,"key.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(360,"rope.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(361,"chain.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(362,"piston1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(363,"tar2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(364,"poop.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(365,"dust.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(366,"bubble.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(367,"blood.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(368,"tar.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(369,"amber.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(370,"face.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(371,"facem1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(372,"facem2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(373,"facem3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(374,"facem4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(375,"faceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(376,"faceb2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(377,"faceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(378,"faceh1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(379,"faceh2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(380,"faceh3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(381,"faceh4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(382,"gishhurt.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(370+20,"gface.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(371+20,"gfacem1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(372+20,"gfacem2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(373+20,"gfacem3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(374+20,"gfacem4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(375+20,"gfaceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(376+20,"gfaceb2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(377+20,"gfaceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(378+20,"gfaceh01.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(379+20,"gfaceh02.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(380+20,"gfaceh03.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(381+20,"gfaceh03.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(382+20,"grayhurt.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(370+40,"bface.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(371+40,"bfacem1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(372+40,"bfacem2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(373+40,"bfacem3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(374+40,"bfacem4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(375+40,"bfaceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(376+40,"bfaceb2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(377+40,"bfaceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(378+40,"bfaceh1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(379+40,"bfaceh2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(380+40,"bfaceh3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(381+40,"bfaceh3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(382+40,"bhurt.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(383+40,"btar.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);

    loadtexture(370+60,"rface.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(371+60,"rfacem1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(372+60,"rfacem2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(373+60,"rfacem3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(374+60,"rfacem4.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(375+60,"rfaceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(376+60,"rfaceb2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(377+60,"rfaceb1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(378+60,"rfaceh1.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(379+60,"rfaceh2.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(380+60,"rfaceh3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(381+60,"rfaceh3.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(382+60,"rhurt.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
    loadtexture(383+60,"rtar.png",0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR);
}

void load_font_and_mouse(void){
    loadtexture(TEXT_START_TEXTURE,"font00.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);
    loadtexture(TEXT_START_TEXTURE+1,"font01.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);
    loadtexture(768,"mouse00.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);
    loadtexture(769,"mouse00.png",0,GL_CLAMP,GL_CLAMP,GL_LINEAR,GL_LINEAR);
}
void look_for_texture_in_folders(int texturenum, int numofloadedtextures){
	for (int count = 0; count<numofloadedtextures; count++)
	if(texturecmp(texturenum, ANIMATIONS_START_TEXTURE+count) == 0){ // same texture
        strcpy(texture[texturenum].filename, texture[ANIMATIONS_START_TEXTURE+count].filename);
        texture[texturenum].filename[255] = '\0'; /* Safety first! */
		return;
	}
}