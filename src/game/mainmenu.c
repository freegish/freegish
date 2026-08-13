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

#include <stdlib.h>
#include <time.h>

#include "../game/mainmenu.h"
#include "../game/animation.h"
#include "../game/gameaudio.h"
#include "../game/config.h"
#include "../game/credits.h"
#include "../game/editor.h"
#include "../game/english.h"
#include "../game/game.h"
#include "../game/lighting.h"
#include "../game/music.h"
#include "../game/gameobject.h"
#include "../game/options.h"
#include "../game/physics.h"
#include "../game/player.h"
#include "../game/socket.h"
#include "../game/vsmode.h"
#include "../game/debug.h"
#include "../audio/audio.h"
#include "../input/joystick.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../math/vector.h"
#include "../menu/menu.h"
#include "../sdl/event.h"
#include "../video/text.h"
#include "../video/texture.h"
#include "../sdl/video.h"

#define VERSION "Version 2.0.0"

void mainmenu(void)
  {
  int count,temp;
  int prevjoymenunum;
  //int registered;
  //unsigned int x,y;

  srand(time(NULL));

  load_datapack();

  setupphysics();

  setupobjecttypes();

  editor.mode=1;

  game.songnum=-1;
  game.currentsongnum=-1;
  resetorientation(view.orientation);
  soundsimulation(view.position,view.orientation);

  introscreen();

  resetmenuitems();

  joymenunum=1;
  game.turbomode=1;
  game.numofplayers = 1;

  while (!menuitem[0].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
		temp=(sizeof(TXT_EXIT)+sizeof(TXT_CREDITS)+sizeof(TXT_OPTIONS)+sizeof(TXT_VERSUS)+sizeof(TXT_START)-1)*8;

    createmenuitem(TXT_EXIT,(320+temp)|TEXT_END,380,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    createmenuitem(TXT_START,320-temp,380,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_S);
		temp-=(sizeof(TXT_START))*16;
    createmenuitem(TXT_VERSUS,320-temp,380,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_V);
		temp-=(sizeof(TXT_VERSUS))*16;
    createmenuitem(TXT_OPTIONS,320-temp,380,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_O);
		temp-=(sizeof(TXT_OPTIONS))*16;
    createmenuitem(TXT_CREDITS,320-temp,380,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_C);
    createmenuitem("Cryptic Sea",0,0,1,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_IMAGE,528);
    setmenuitem(MO_RESIZE,16,0,96,48);

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    if (keyboard[SCAN_T] && !prevkeyboard[SCAN_T])
      game.turbomode^=1;

    updateogg();
    checkmusic();
    game.songnum=8;
    soundsimulation(view.position,view.orientation);

    setuptextdisplay();

    glColor4f(1.0f,1.0f,1.0f,1.0f);
    displaybackground(470);

    if (game.turbomode)
      drawtext(TXT_TURBO_EDITION,(640|TEXT_END),470,10,1.0f,1.0f,1.0f,1.0f);

    drawtext(VERSION,0,470,10,1.0f,1.0f,1.0f,1.0f);

    drawtext(TXT_COPYRIGHT,(320|TEXT_CENTER),470,10,0.75f,0.75f,0.75f,1.0f);

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    if (menuitem[5].active)
      {
      launchwebpage("www.crypticsea.com");
      menuitem[5].active=0;
      }
    if (menuitem[4].active)
      {
      //game.songnum=-1;
      //checkmusic();

      prevjoymenunum=4;
      creditsmenu();
      joymenunum=prevjoymenunum;
      }
    if (menuitem[3].active)
      {
      game.songnum=-1;
      checkmusic();

      prevjoymenunum=3;
      optionsmenu();
      joymenunum=prevjoymenunum;
      }
    if (menuitem[2].active)
      {
      //game.songnum=-1;
      //checkmusic();

      prevjoymenunum=2;
      if (versus_numplayers < 2)
          setup_presets_to_players(&versus_numplayers, versus_is_joystick, versus_presets, 2);
      versusmenu();
      joymenunum=prevjoymenunum;
      }
    if (menuitem[1].active)
      {
      //game.songnum=-1;
      //checkmusic();

      prevjoymenunum=1;
      playermenu();
      joymenunum=prevjoymenunum;
      }
    }

  game.songnum=-1;
  checkmusic();
  }

void loadbackground(int texturenum,char *filename)
  {
  loadtexturepartial(texturenum,filename,0,0,256,256);
  loadtexturepartial(texturenum+1,filename,256,0,256,256);
  loadtexturepartial(texturenum+2,filename,512,0,256,256);
  loadtexturepartial(texturenum+3,filename,768,0,256,256);
  loadtexturepartial(texturenum+4,filename,0,256,256,256);
  loadtexturepartial(texturenum+5,filename,256,256,256,256);
  loadtexturepartial(texturenum+6,filename,512,256,256,256);
  loadtexturepartial(texturenum+7,filename,768,256,256,256);
  loadtexturepartial(texturenum+8,filename,0,512,256,256);
  loadtexturepartial(texturenum+9,filename,256,512,256,256);
  loadtexturepartial(texturenum+10,filename,512,512,256,256);
  loadtexturepartial(texturenum+11,filename,768,512,256,256);
  }

void displaybackground(int texturenum)
  {
  drawbackground(texturenum,0,0,256,256,800,600);
  drawbackground(texturenum+1,256,0,256,256,800,600);
  drawbackground(texturenum+2,512,0,256,256,800,600);
  drawbackground(texturenum+3,768,0,256,256,800,600);
  drawbackground(texturenum+4,0,256,256,256,800,600);
  drawbackground(texturenum+5,256,256,256,256,800,600);
  drawbackground(texturenum+6,512,256,256,256,800,600);
  drawbackground(texturenum+7,768,256,256,256,800,600);
  drawbackground(texturenum+8,0,512,256,256,800,600);
  drawbackground(texturenum+9,256,512,256,256,800,600);
  drawbackground(texturenum+10,512,512,256,256,800,600);
  drawbackground(texturenum+11,768,512,256,256,800,600);
  }

void setup_presets_to_players(int* numplayers_out, int* controllers, int* presets, int minplayers){
    resetmenuitems();

    // controllers is a 4-length int array where
    // 0 is kb
    // 1-4 is joystick

    // int teams[4] = {0, 1, 2, 3};

    int numplayers = *numplayers_out;

    joymenunum=1;

    while (!menuitem[0].active && !windowinfo.shutdown)
        {
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        numofmenuitems=0;

        int continue_button = createmenuitem(TXT_CONTINUE,640|TEXT_END,0,16,1.0f,1.0f,1.0f,1.0f);
        int left_arrows[4] = {-1, -1, -1, -1};
        int right_arrows[4] = {-1, -1, -1, -1};

        /* // need this if you want to have same keys for different players. Irrelevant for now.
        for (int player_index = 0; player_index < numplayers; player_index++){
            left_arrows[player_index] = createmenuitem(TXT_LEFT_ARROW, (80 - 80 + player_index * 160), 360+16+10, 10,1.0f,1.0f,1.0f,1.0f);
            left_arrows[player_index] = createmenuitem(TXT_RIGHT_ARROW, (80 + 80 + player_index * 160)|TEXT_END, 360+16+10, 10,1.0f,1.0f,1.0f,1.0f);
        }
        */

        checksystemmessages();
        checkkeyboard();
        checkmouse();
        checkjoystick();
        checkmenuitems();

        updateogg();
        checkmusic();
        game.songnum=8;
        soundsimulation(view.position,view.orientation);

        setuptextdisplay();

        glColor4f(1.0f,1.0f,1.0f,1.0f);
        //displaybackground(600);

        drawtext(TXT_PRESS_JUMP_BUTTON,(320|TEXT_CENTER),120,16,1.0f,1.0f,1.0f,1.0f);
        if (numplayers > 0){
            drawtext(TXT_PRESS_HEAVY_BUTTON,(320|TEXT_CENTER),120+16,16,1.0f,1.0f,1.0f,1.0f);
            drawtext(TXT_PRESS_RIGHT_CLICK_TO_LEAVE,(320|TEXT_CENTER),120+16+16,12,1.0f,1.0f,1.0f,1.0f);
            drawtext(TXT_PRESS_DELETE_TO_LEAVE,(320|TEXT_CENTER),120+16+16+12,12,1.0f,1.0f,1.0f,1.0f);
        }
        for (int player_index = 0; player_index < numplayers; player_index++){
            if (controllers[player_index] == 0){
                drawtext(TXT_KEYBOARD, (80 + player_index * 160)|TEXT_CENTER, 360, 16,1.0f,1.0f,1.0f,1.0f);

                drawtext(keyboardpresets[presets[player_index]].name, (80 + player_index * 160)|TEXT_CENTER, 360+16+10, 10,1.0f,1.0f,1.0f,1.0f);
            }
            else{
                drawtext(TXT_JOYSTICK, (80 + player_index * 160)|TEXT_CENTER, 360, 16,1.0f,1.0f,1.0f,1.0f);
                drawtext(joystick[controllers[player_index]-1].name, (80 + player_index * 160)|TEXT_CENTER, 360+16, 10,0.5f,0.5f,0.5f,1.0f);
                drawtext(joystickpresets[presets[player_index]].name, (80 + player_index * 160)|TEXT_CENTER, 360+16+10, 10, 1.0f,1.0f,1.0f,1.0f);
            }
        }
        if (numplayers < minplayers) {
            drawtext(TXT_TOO_FEW_PlAYERS,(640|TEXT_END),16,10,1.0f,1.0f,1.0f,1.0f, minplayers);
        }

        drawmenuitems();

        drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

        SDL_GL_SwapWindow(globalwindow);

        // joining
        if (numplayers < 4){
            for (int kbpreset=0;kbpreset<numkeyboardpresets;kbpreset++){
                int jump_button = keyboardpresets[kbpreset].key[KEYALIAS_JUMP];
                if (keyboard[jump_button] && !prevkeyboard[jump_button]){
                    controllers[numplayers] = 0;
                    presets[numplayers] = kbpreset;
                    numplayers++;
                    break;
                }
            }
            for (int jpreset=0; jpreset<numjoystickpresets; jpreset++){
                int joystick_index = joystickpresets[jpreset].joysticknum;
                int jump_button = joystickpresets[jpreset].button[KEYALIAS_JUMP];
                if (joystick[joystick_index].button[jump_button] && !prevjoystick[joystick_index].button[jump_button]){
                    controllers[numplayers] = joystick_index+1;
                    presets[numplayers] = jpreset;
                    numplayers++;
                    break;
                }
            }
        }
        // leaving
        for (int player_index = numplayers-1; player_index >= 0; player_index--){
            if (controllers[player_index] == 0){
                int heavy_button = keyboardpresets[presets[player_index]].key[KEYALIAS_HEAVY];
                if (keyboard[heavy_button] && !prevkeyboard[heavy_button]){
                    // found, exclude them
                    for (; player_index < numplayers-1; player_index++){
                        controllers[player_index] = controllers[player_index+1];
                        presets[player_index] = presets[player_index+1];
                    }
                    numplayers--;
                    break;
                }
            }
            else {
                int joystick_index = joystickpresets[controllers[player_index]-1].joysticknum;
                int heavy_button = joystickpresets[controllers[player_index]-1].button[KEYALIAS_HEAVY];
                if (joystick[joystick_index].button[heavy_button] && !prevjoystick[joystick_index].button[heavy_button]){
                    // found, exclude them
                    for (; player_index < numplayers-1; player_index++){
                        controllers[player_index] = controllers[player_index+1];
                        presets[player_index] = presets[player_index+1];
                    }
                    numplayers--;
                    break;
                }
            }
        }

        if (menuitem[continue_button].active){
            if (numplayers < minplayers) {
                menuitem[continue_button].active = 0;
                continue;
            }
            // save to out variables
            *numplayers_out = numplayers;
            break;
        }
        if (mouse.rmb && !prevmouse.rmb)
        if (mouse.y > 300 && (mouse.x / 160) < numplayers){
          for (int player_index = mouse.x / 160; player_index < numplayers-1; player_index++){
              controllers[player_index] = controllers[player_index+1];
              presets[player_index] = presets[player_index+1];
          }
          numplayers--;
        }
        if (keyboard[SCAN_DELETE] && !prevkeyboard[SCAN_DELETE]){
          numplayers--;
        }
    }
    resetmenuitems();
}
void bind_presets_to_controls(int numplayers, int* controllers, int* presets){
    // setup controls
    for (int player_index = 0; player_index < numplayers; player_index++){
        if (controllers[player_index] == 0){
            control[player_index] = keyboardpresets[presets[player_index]];
        }
        else{
            control[player_index] = joystickpresets[presets[player_index]];
        }
    }
}
void versusmenu(void)
  {
  int count;
  int unlocked;

  unlocked=0;
  for (count=0;count<6;count++)
  if (player[count].unlock[WON_NORMAL])
    unlocked=1;

  resetmenuitems();

  joymenunum=1;

  while (!menuitem[0].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(TXT_BACK,0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);

    count=64;
    createmenuitem(TXT_SUMO,(144|TEXT_CENTER),304,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_S);
    createmenuitem(TXT_FOOTBALL,(320|TEXT_CENTER),304,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_F);
    createmenuitem(TXT_GREED,(512|TEXT_CENTER),304,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_G);
    createmenuitem(TXT_DUEL,(152|TEXT_CENTER),444,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_U);
    createmenuitem(TXT_DRAGSTER,(320|TEXT_CENTER),444,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_D);
    if (unlocked)
      {
      createmenuitem(TXT_COLLECTION,(512|TEXT_CENTER),444,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_HOTKEY,SCAN_C);
      }
    else{
        createmenuitemempty(); //stupid
    }
    int change_players = createmenuitem(TXT_CHANGE_PLAYERS, 640|TEXT_END, 0, 16, 1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_H);

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    updateogg();
    checkmusic();
    game.songnum=8;
    soundsimulation(view.position,view.orientation);

    setuptextdisplay();

    glColor4f(1.0f,1.0f,1.0f,1.0f);
    displaybackground(600);

    if (!unlocked)
      {
      drawbackground(256+68,448,328,96,96,640,480);
      drawtext("?????",(512|TEXT_CENTER),444,16,1.0f,1.0f,1.0f,1.0f);
      }

    drawmenuitems();


    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    for (count=1;count<7;count++)
    if (menuitem[count].active){
        bind_presets_to_controls(versus_numplayers, versus_is_joystick, versus_presets);
        versusmodemenu(count-1);
    }
    if (menuitem[change_players].active)
        setup_presets_to_players(&versus_numplayers, versus_is_joystick, versus_presets, 2);
    }

  resetmenuitems();
  }

void storyscreen(void)
  {
  int count;
  int simtimer;
  int simcount;
  int loaddelay;
  int screennum;

  loaddelay=0;
  screennum=0;

  loadbackground(824,"story1.png");

  simtimer=SDL_GetTicks();

  resetmenuitems();

  joymenunum=1;

  while (!menuitem[0].active && screennum<8 && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(TXT_BACK,0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    createmenuitem(TXT_NEXT,(640|TEXT_END),0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_N);

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    updateogg();
    checkmusic();
    game.songnum=8;
    soundsimulation(view.position,view.orientation);

    setuptextdisplay();

    glColor4f(1.0f,1.0f,1.0f,1.0f);
    displaybackground(824);

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    simcount=0;
    while (SDL_GetTicks()-simtimer>20 && simcount<5)
      {
      simcount++;
      count=SDL_GetTicks()-simtimer-20;
      simtimer=SDL_GetTicks()-count;

      loaddelay++;
      if (loaddelay>=400)
        {
        screennum++;
        if (screennum==1)
          loadbackground(824,"story2.png");
        if (screennum==2)
          loadbackground(824,"story3.png");
        if (screennum==3)
          loadbackground(824,"story4.png");
        if (screennum==4)
          loadbackground(824,"story5.png");
        if (screennum==5)
          loadbackground(824,"story6.png");
        if (screennum==6)
          loadbackground(824,"story7.png");
        if (screennum==7)
          loadbackground(824,"story8.png");
        loaddelay=0;
        }
      }
    if (menuitem[1].active)
      {
      screennum++;
      if (screennum==1)
        loadbackground(824,"story2.png");
      if (screennum==2)
        loadbackground(824,"story3.png");
      if (screennum==3)
        loadbackground(824,"story4.png");
      if (screennum==4)
        loadbackground(824,"story5.png");
      if (screennum==5)
        loadbackground(824,"story6.png");
      if (screennum==6)
        loadbackground(824,"story7.png");
      if (screennum==7)
        loadbackground(824,"story8.png");
      loaddelay=0;
      menuitem[1].active=0;
      }
    }

  resetmenuitems();
  }

void introscreen(void)
  {
  int count,count2=0;
  int simtimer;
  int simcount;
  int loaddelay;
  float alpha;
  int secretsequence;
  float vec[3];

  srand(time(NULL));

  loaddelay=0;

  secretsequence=0;

  simtimer=SDL_GetTicks();

  resetmenuitems();

  while (!menuitem[0].active && loaddelay<200 && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(" ",0,0,640,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    if (secretsequence==0)
      count2=3;
    if (secretsequence==1)
      count2=3;
    if (secretsequence==2)
      count2=2;
    if (secretsequence==3)
      count2=2;
    if (secretsequence==4)
      count2=0;
    if (secretsequence==5)
      count2=1;
    if (secretsequence==6)
      count2=0;
    if (secretsequence==7)
      count2=1;
    if (secretsequence==8)
      count2=4;
    if (secretsequence==9)
      count2=5;
    if (secretsequence==10)
      {
      if (game.turbomode==0)
        {
        vec[0]=0.0f;
        vec[1]=0.0f;
        vec[2]=0.0f;
        playsound(10,vec,NULL,0.4f,0,1.0f,-1,0);
        }
      game.turbomode=1;
      }

    for (count=0;count<KEYALIAS_LENGTH;count++)
      {
      if (keyboard[control[0].key[count]] && !prevkeyboard[control[0].key[count]])
        {
        if (count==count2)
          secretsequence++;
        else
          secretsequence=0;
        }

      if (control[0].joysticknum!=-1)
        {
        if (count==0)
        if (joystick[control[0].joysticknum].axis[0]<=-0.5f && prevjoystick[control[0].joysticknum].axis[0]>-0.5f)
          {
          if (count==count2)
            secretsequence++;
          else
            secretsequence=0;
          }
        if (count==1)
        if (joystick[control[0].joysticknum].axis[0]>=0.5f && prevjoystick[control[0].joysticknum].axis[0]<0.5f)
          {
          if (count==count2)
            secretsequence++;
          else
            secretsequence=0;
          }
        if (count==2)
        if (joystick[control[0].joysticknum].axis[1]<=-0.5f && prevjoystick[control[0].joysticknum].axis[1]>-0.5f)
          {
          if (count==count2)
            secretsequence++;
          else
            secretsequence=0;
          }
        if (count==3)
        if (joystick[control[0].joysticknum].axis[1]>=0.5f && prevjoystick[control[0].joysticknum].axis[1]<0.5f)
          {
          if (count==count2)
            secretsequence++;
          else
            secretsequence=0;
          }
        if (control[0].button[count]!=-1)
        if (joystick[control[0].joysticknum].button[control[0].button[count]] && !prevjoystick[control[0].joysticknum].button[control[0].button[count]])
          {
          if (count==count2)
            secretsequence++;
          else
            secretsequence=0;
          }
        }
      }

    updateogg();
    checkmusic();
    game.songnum=8;
    soundsimulation(view.position,view.orientation);

    setuptextdisplay();

    if (loaddelay<100)
      {
      alpha=(float)(100-loaddelay)/100.0f;
      glColor4f(1.0f,1.0f,1.0f,alpha);
      displaybackground(800);
      }
    else
      {
      alpha=(float)(loaddelay-100)/100.0f;
      glColor4f(1.0f,1.0f,1.0f,alpha);
      displaybackground(470);

      drawtext(TXT_EXIT,(320+72+128),380,16,0.5f,0.5f,0.5f,alpha);
      drawtext(TXT_START,(320-72-112)|TEXT_END,380,16,0.5f,0.5f,0.5f,alpha);
      drawtext(TXT_VERSUS,(320-72)|TEXT_END,380,16,0.5f,0.5f,0.5f,alpha);
      drawtext(TXT_OPTIONS,(320)|TEXT_CENTER,380,16,0.5f,0.5f,0.5f,alpha);
      drawtext(TXT_CREDITS,(320+72),380,16,0.5f,0.5f,0.5f,alpha);

      if (game.turbomode)
        drawtext(TXT_TURBO_EDITION,(640|TEXT_END),470,10,1.0f,1.0f,1.0f,1.0f);
      drawtext(VERSION,0,470,10,1.0f,1.0f,1.0f,1.0f);

      drawtext(TXT_COPYRIGHT,(320|TEXT_CENTER),470,10,0.75f,0.75f,0.75f,alpha);
      }
    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    simcount=0;
    while (SDL_GetTicks()-simtimer>20 && simcount<5)
      {
      simcount++;
      count=SDL_GetTicks()-simtimer-20;
      simtimer=SDL_GetTicks()-count;

      loaddelay++;
      }
    }

  resetmenuitems();
  }
