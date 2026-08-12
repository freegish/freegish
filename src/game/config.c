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

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../game/config.h"
#include "../game/options.h"
#include "../game/socket.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../menu/menu.h"
#include "../parser/parser.h"
#include "../sdl/event.h"
#include "../video/text.h"
#include "../sdl/video.h"

_config config;

#ifdef WINDOWS
#define USERPATH(X) "%s/Gish" X
#else
#if MAC
#define USERPATH(X) "%s/Library/Application Support/Gish" X
#else
#define USERPATH(X) "%s/.freegish" X
#endif
#endif
#ifdef WINDOWS
#include <direct.h>
#define MKDIR(PATHNAME) _mkdir(PATHNAME)
#define USERENV "APPDATA"
#define snprintf sprintf_s
#else
#include <unistd.h>
#define MKDIR(PATHNAME) mkdir(PATHNAME, S_IRWXU | S_IRWXG | S_IRWXO)
#define USERENV "HOME"
#endif

char* userpath(char *result, char *subdir, char *file)
  {
  struct stat st;
  char *env=getenv(USERENV);

  if (!env)
    goto fail;

  if (snprintf(result,PATH_MAX,USERPATH(""),env) < 0)
    goto fail;

  /* Ignore failure. May exist already. */
  MKDIR(result);

  if (stat(result,&st)==-1 || !(st.st_mode & S_IFDIR))
    goto fail;

  if (subdir)
    {
    if (snprintf(result,PATH_MAX,USERPATH("/%s"),env,subdir) < 0)
      goto fail;

    /* Ignore failure. May exist already. */
    MKDIR(result);

    if (stat(result,&st)==-1 || !(st.st_mode & S_IFDIR))
      goto fail;

    if (!file)
      return result;

    if (snprintf(result,PATH_MAX,USERPATH("/%s/%s"),env,subdir,file) < 0)
      goto fail;
    else
      return result;
    }

  else
    {
    if (snprintf(result,PATH_MAX,USERPATH("/%s"),env,file) < 0)
      goto fail;
    else
      return result;
    }

  fail:

  if (file)
    strncpy(result,file,PATH_MAX);
  else
    strncpy(result,".",PATH_MAX);

  result[PATH_MAX-1]='\0'; /* Safety first! */
  return result;
  }

void loadconfig(void)
  {
  int count,count2;
  char tempstr[32];
  char path[PATH_MAX];

  config.resolutionx=800;
  config.resolutiony=600;
  config.bitsperpixel=32;
  config.depthbits=24;
  config.stencilbits=8;
  config.fullscreen=0;
  config.sound=1;
  config.music=1;
  config.joystick=1;

  option.sound=1;
  option.music=1;
  option.soundvolume=1.0f;
  option.musicvolume=1.0f;
  numkeyboardpresets = 1;
  numjoystickpresets = 0;

  for (count = 0; count < CONTROLS_LENGTH; count++){
    control[count].joysticknum = -1;
    for (count2 = 0; count2 < KEYALIAS_LENGTH; count2++){
        control[count].key[count2] = -1;
        control[count].button[count2] = -1;
    }
    for (count2 = 0; count2 < 4; count2++){
        control[count].axis[count2] = -1;
    }
  }

  strcpy(keyboardpresets[0].name, "Default preset");
  keyboardpresets[0].key[KEYALIAS_LEFT]=SCAN_LEFT;
  keyboardpresets[0].key[KEYALIAS_RIGHT]=SCAN_RIGHT;
  keyboardpresets[0].key[KEYALIAS_DOWN]=SCAN_DOWN;
  keyboardpresets[0].key[KEYALIAS_UP]=SCAN_UP;
  keyboardpresets[0].key[KEYALIAS_STICK]=SCAN_A;
  keyboardpresets[0].key[KEYALIAS_JUMP]=SCAN_SPACE;
  keyboardpresets[0].key[KEYALIAS_SLIDE]=SCAN_S;
  keyboardpresets[0].key[KEYALIAS_HEAVY]=SCAN_D;
  keyboardpresets[0].joysticknum=-1;
  keyboardpresets[0].axis[0]=-1;
  keyboardpresets[0].axis[1]=-1;
  for (count=0;count<KEYALIAS_LENGTH;count++)
    keyboardpresets[0].button[count]=-1;

  loadtextfile(userpath(path,NULL,"config.txt"));
  optionreadint(&config.resolutionx,"screenwidth=");
  optionreadint(&config.resolutiony,"screenheight=");
  optionreadint(&config.bitsperpixel,"bitsperpixel=");
  optionreadint(&config.depthbits,"depthbits=");
  optionreadint(&config.stencilbits,"stencilbits=");
  optionreadint(&config.fullscreen,"fullscreen=");
  optionreadint(&config.sound,"sound=");
  optionreadint(&config.music,"music=");
  optionreadint(&config.joystick,"joystick=");

  optionreadint(&option.sound,"soundon=");
  optionreadint(&option.music,"musicon=");
  count=-1;
  optionreadint(&count,"soundvolume=");
  if (count!=-1)
    option.soundvolume=(float)count/100.0f;
  count=-1;
  optionreadint(&count,"musicvolume=");
  if (count!=-1)
    option.musicvolume=(float)count/100.0f;

  optionreadint(&numkeyboardpresets,"numkeyboardpresets=");
  optionreadint(&numjoystickpresets,"numjoystickpresets=");

  for (count=0;count<numkeyboardpresets;count++){
    sprintf(tempstr,"keyboardpreset%dname=",count+1);
    optionreadstring(&keyboardpresets[count].name, tempstr, 32);
    for (count2=0;count2<KEYALIAS_LENGTH;count2++){
      sprintf(tempstr,"keyboardpreset%dkey%d=",count+1,count2+1);
      optionreadint((int*)&keyboardpresets[count].key[count2],tempstr);
    }
  }
  for (count=0; count<numjoystickpresets;count++){
    sprintf(tempstr,"joystickpreset%dname=",count+1);
    optionreadstring(&joystickpresets[count].name, tempstr, 32);
    sprintf(tempstr,"joystickpreset%djoysticknum=",count+1);
    optionreadint(&joystickpresets[count].joysticknum,tempstr);

    for (count2=0;count2<4;count2++)
      {
      sprintf(tempstr,"joystickpreset%daxis%d=",count+1,count2+1);
      optionreadint(&joystickpresets[count].axis[count2],tempstr);
      }
    for (count2=0;count2<16;count2++)
      {
      sprintf(tempstr,"joystickpreset%dbutton%d=",count+1,count2+1);
      optionreadint(&joystickpresets[count].button[count2],tempstr);
      }
  }
  optionreadint(&versus_numplayers,"versus_numplayers=");
  for (count=0; count<versus_numplayers;count++){
    sprintf(tempstr,"versus_is_joystick%d=",count+1);
    optionreadint(&versus_is_joystick[count],tempstr);
    sprintf(tempstr,"versus_presets%d=",count+1);
    optionreadint(&versus_presets[count],tempstr);
  }

  optionreadstring(datapacks_folder, "datapacks_folder=", 32);
  optionreadstring(loaded_datapack, "loaded_datapack=", 32);

  if (datapacks_folder[0] == 0)
      strcpy(datapacks_folder, "datapacks");
  if (loaded_datapack[0] == 0)
      strcpy(loaded_datapack, "freegish");

  windowinfo.resolutionx=config.resolutionx;
  windowinfo.resolutiony=config.resolutiony;
  windowinfo.bitsperpixel=config.bitsperpixel;
  windowinfo.depthbits=config.depthbits;
  windowinfo.stencilbits=config.stencilbits;
  windowinfo.fullscreen=config.fullscreen;
  }

void saveconfig(void)
  {
  int count,count2;
  char tempstr[32];
  FILE *fp;
  char path[PATH_MAX];

  config.resolutionx=windowinfo.resolutionx;
  config.resolutiony=windowinfo.resolutiony;
  config.bitsperpixel=windowinfo.bitsperpixel;
  config.depthbits=windowinfo.depthbits;
  config.stencilbits=windowinfo.stencilbits;
  config.fullscreen=windowinfo.fullscreen;

  if ((fp=fopen(userpath(path,NULL,"config.txt"),"wb"))==NULL)
    return;

  optionwriteint(fp, &config.resolutionx,"screenwidth=");
  optionwriteint(fp, &config.resolutiony,"screenheight=");
  optionwriteint(fp, &config.bitsperpixel,"bitsperpixel=");
  optionwriteint(fp, &config.depthbits,"depthbits=");
  optionwriteint(fp, &config.stencilbits,"stencilbits=");
  optionwriteint(fp, &config.fullscreen,"fullscreen=");
  optionwriteint(fp, &config.sound,"sound=");
  optionwriteint(fp, &config.music,"music=");
  optionwriteint(fp, &config.joystick,"joystick=");

  optionwriteint(fp, &option.sound,"soundon=");
  optionwriteint(fp, &option.music,"musicon=");
  count=option.soundvolume*100.0f;
  optionwriteint(fp, &count,"soundvolume=");
  count=option.musicvolume*100.0f;
  optionwriteint(fp, &count,"musicvolume=");
  optionwriteint(fp, &numkeyboardpresets,"numkeyboardpresets=");
  optionwriteint(fp, &numjoystickpresets,"numjoystickpresets=");

  for (count=0;count<numkeyboardpresets;count++){
    sprintf(tempstr,"keyboardpreset%dname=",count+1);
    optionwritestring(fp, keyboardpresets[count].name, tempstr, 32);
    for (count2=0;count2<KEYALIAS_LENGTH;count2++){
      sprintf(tempstr,"keyboardpreset%dkey%d=",count+1,count2+1);
      optionwriteint(fp, (int*)&keyboardpresets[count].key[count2],tempstr);
    }

    keyboardpresets[count].joysticknum = -1;
    for (count2=0;count2<4;count2++){
        keyboardpresets[count].axis[count2] = -1;
    }
    for (count2=0;count2<16;count2++){
        keyboardpresets[count].button[count2] = -1;
    }
  }
  for (count=0; count<numjoystickpresets;count++){
    sprintf(tempstr,"joystickpreset%dname=",count+1);
    optionwritestring(fp, joystickpresets[count].name, tempstr, 32);
    sprintf(tempstr,"joystickpreset%djoysticknum=",count+1);
    optionwriteint(fp, &joystickpresets[count].joysticknum,tempstr);

    for (count2=0;count2<KEYALIAS_LENGTH;count2++)
        joystickpresets[count].key[count2] = -1;
    for (count2=0;count2<4;count2++)
      {
      sprintf(tempstr,"joystickpreset%daxis%d=",count+1,count2+1);
      optionwriteint(fp, &joystickpresets[count].axis[count2],tempstr);
      }
    for (count2=0;count2<16;count2++)
      {
      sprintf(tempstr,"joystickpreset%dbutton%d=",count+1,count2+1);
      optionwriteint(fp, &joystickpresets[count].button[count2],tempstr);
      }
  }

  optionwriteint(fp, &versus_numplayers,"versus_numplayers=");
  for (count=0; count<versus_numplayers;count++){
    sprintf(tempstr,"versus_is_joystick%d=",count+1);
    optionwriteint(fp, &versus_is_joystick[count],tempstr);
    sprintf(tempstr,"versus_presets%d=",count+1);
    optionwriteint(fp, &versus_presets[count],tempstr);
  }
  optionwritestring(fp, datapacks_folder, "datapacks_folder=", 32);
  optionwritestring(fp, loaded_datapack, "loaded_datapack=", 32);
  fclose(fp);
  }

void notsupportedmenu(void)
  {
  int count;
  char *glvendor;
  char *glrenderer;
  char *glversion;
  char *ext;
  FILE *fp;
  char path[PATH_MAX];

  glvendor=(char *) glGetString(GL_VENDOR);
  glrenderer=(char *) glGetString(GL_RENDERER);
  glversion=(char *) glGetString(GL_VERSION);
  ext=(char *) glGetString(GL_EXTENSIONS);

  resetmenuitems();

  while (!menuitem[0].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem("Exit",8,8,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    count=352;
    createmenuitem("NVIDIA Drivers",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;
    createmenuitem("ATI Drivers",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;
    createmenuitem("Intel Drivers",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkmenuitems();

    setuptextdisplay();

    count=224;
    drawtext("Multitexture required",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;
    count+=16;
    drawtext("Make sure you have the",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;
    drawtext("latest video card drivers",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;
    drawtext("for your computer",(320|TEXT_CENTER),count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;

    count=432;
    drawtext("GL_VERSION: /s",8,count,12,1.0f,1.0f,1.0f,1.0f,glversion);
    count+=12;
    drawtext("GL_VENDOR: /s",8,count,12,1.0f,1.0f,1.0f,1.0f,glvendor);
    count+=12;
    drawtext("GL_RENDERER: /s",8,count,12,1.0f,1.0f,1.0f,1.0f,glrenderer);
    count+=12;

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    if (menuitem[1].active)
      {
      launchwebpage("www.nvidia.com/content/drivers/drivers.asp");
      menuitem[1].active=0;
      }
    if (menuitem[2].active)
      {
      launchwebpage("ati.amd.com/support/driver.html");
      menuitem[2].active=0;
      }
    if (menuitem[3].active)
      {
      launchwebpage("downloadcenter.intel.com");
      menuitem[3].active=0;
      }
    }

  resetmenuitems();

  if ((fp=fopen(userpath(path,NULL,"glreport.txt"),"wb"))==NULL)
    return;

  fprintf(fp,"%s\r\n",glversion);
  fprintf(fp,"%s\r\n",glvendor);
  fprintf(fp,"%s\r\n",glrenderer);
  fprintf(fp,"%s\r\n",ext);

  fclose(fp);
  }

void optionreadint(int *ptr,char *str)
  {
  if (findstring(str))
    *ptr=getint();

  parser.textloc=0;
  }

void optionwriteint(FILE *fp, int *ptr,char *str)
  {
  fprintf(fp,"%s%d\r\n",str,*ptr);
  }

void optionreadstring(char *ptr,char *str,int size)
  {
  if (findstring(str))
    getstring(ptr,size);

  parser.textloc=0;
  }

void optionwritestring(FILE *fp, char *ptr,char *str,int size)
  {
  fprintf(fp,"%s%s\r\n",str,ptr);
  }

