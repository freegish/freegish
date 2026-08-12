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

#include "../game/debug.h"
#include "../game/config.h"
#include "../game/options.h"
#include "../game/english.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../input/joystick.h"
#include "../menu/menu.h"
#include "../sdl/event.h"
#include "../sdl/video.h"
#include "../video/text.h"
#include "../video/texture.h"
#include "../sdl/video.h"

// extern
_option option;
_control control[CONTROLS_LENGTH];
_control keyboardpresets[PRESETS_LENGTH];
_control joystickpresets[PRESETS_LENGTH];
int numkeyboardpresets;
int numjoystickpresets;

int versus_numplayers;
int versus_is_joystick[4];
int versus_presets[4];

char datapacks_folder[32];
char loaded_datapack[32];

// local
char slider_was_clicked = -1;
int clicked_keyboard_preset = -1;
int clicked_joystick_preset = -1;

void drawkeyboardkeyrow(char* text, keyalias keyalias_, _control* preset, int offset, int menuitem_offset, int y){
    drawtext(text,0,y+offset*16,16,0.75f,0.75f,0.75f,1.0f);
    if (!menuitem[menuitem_offset+offset].active)
      drawtext(keyboardlabel[preset->key[keyalias_]],320,y+offset*16,16,1.0f,1.0f,1.0f,1.0f);
    else
      drawtext("?",320,y+offset*16,16,1.0f,1.0f,1.0f,1.0f);
}
void drawjoystickkeyrow(char* text, keyalias keyalias_, _control* preset, int offset, int menuitem_offset, int y){
    drawtext(text,0,y+offset*16,16,0.75f,0.75f,0.75f,1.0f);
    if (!menuitem[menuitem_offset+offset].active){
      if (preset->button[offset]!=-1)
        drawtext(TXT_BUTTON" /i",320,y+offset*16,16,1.0f,1.0f,1.0f,1.0f,preset->button[offset]+1);
      else
        drawtext(TXT_AXIS,320,y+offset*16,16,1.0f,1.0f,1.0f,1.0f);
      }
    else
      drawtext("?",320,y+offset*16,16,1.0f,1.0f,1.0f,1.0f);
}

void bindkeyboardkeys(_control* preset, int first_corresponding_keyboard_item){
    for (int count=0;count<KEYALIAS_LENGTH;count++)
    if (menuitem[first_corresponding_keyboard_item + count].active){
        for (int count2=1;count2<SDL_NUM_SCANCODES;count2++)
        if (keyboardlabel[count2][0]!=0) // label isn't empty
        if (keyboard[count2] && !prevkeyboard[count2]){
        preset->key[count]=count2; // set key
        menuitem[first_corresponding_keyboard_item + count].active=0;
        }
        if (keyboard[SCAN_DELETE] && !prevkeyboard[SCAN_DELETE])
        {
        preset->key[count]=0;
        menuitem[first_corresponding_keyboard_item + count].active=0;
        }
    }
}

void bindjoystickkeys(_control* preset, int first_corresponding_joystick_item){
    if (preset->joysticknum!=-1)
    for (int count=0;count<KEYALIAS_LENGTH;count++)
    if (menuitem[first_corresponding_joystick_item + count].active){
        for (int count2=0;count2<joystick[preset->joysticknum].numofbuttons;count2++)
        if (joystick[preset->joysticknum].button[count2] && !prevjoystick[preset->joysticknum].button[count2]){
            preset->button[count]=count2;
            menuitem[first_corresponding_joystick_item + count].active=0;
        }
        if (keyboard[SCAN_DELETE] && !prevkeyboard[SCAN_DELETE]){
            preset->button[count]=-1;
            menuitem[first_corresponding_joystick_item + count].active=0;
        }
    }
}

void set_profile_buttons_menu(_control *preset, char is_joystick){
  int count,count2,count3;

  count=0;

  joystickmenu=0;

  resetmenuitems();

  while (!menuitem[0].active && !windowinfo.shutdown){
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(TXT_BACK,0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    int first_item = numofmenuitems;
    for (count = 0; count < KEYALIAS_LENGTH; count++){
        createmenuitem("        ",320,144 + 16 * count,16,1.0f,1.0f,1.0f,1.0f);
    }

    if (is_joystick){
        char* texts[5] = {
            TXT_NONE,
            TXT_JOY1,
            TXT_JOY2,
            TXT_JOY3,
            TXT_JOY4
        };
        for (count = -1; count <= 3; count++){
            if (preset->joysticknum == count){
                createmenuitem(texts[count+1],320,112,16,1.0f,1.0f,1.0f,1.0f);
                setmenuitem(MO_SET,&preset->joysticknum,count == 3 ? -1 : count+1); // if last joystick, then -1, else next joystick
            }
        }
    }
    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    setuptextdisplay();

    if (is_joystick){
        drawtext(TXT_JOYSTICKS,320,90,10,1.0f,1.0f,1.0f,1.0f);
        drawtext(TXT_CONNECTED": /i",320,100,10,1.0f,1.0f,1.0f,1.0f, numofjoysticks);
    }

    if (is_joystick){
        drawtext(TXT_JOYSTICK,0,64,16,1.0f,1.0f,1.0f,1.0f);
    }
    else{
        drawtext(TXT_KEYBOARD,0,64,16,1.0f,1.0f,1.0f,1.0f);
    }
    drawtext(preset->name,0,80,16,1.0f,1.0f,1.0f,1.0f);

    char* keypickoptions[KEYALIAS_LENGTH] = {
        TXT_MOVE_LEFT,
        TXT_MOVE_RIGHT,
        TXT_MOVE_DOWN,
        TXT_MOVE_UP,
        TXT_STICK,
        TXT_JUMP,
        TXT_SLIDE,
        TXT_HEAVY,
        TXT_START_PAUSE
    };
    keyalias keyaliases[KEYALIAS_LENGTH] = { // keyaliases are literally equal to counts....
        KEYALIAS_LEFT,
        KEYALIAS_RIGHT,
        KEYALIAS_DOWN,
        KEYALIAS_UP,
        KEYALIAS_STICK,
        KEYALIAS_JUMP,
        KEYALIAS_SLIDE,
        KEYALIAS_HEAVY,
        KEYALIAS_START_PAUSE
    };

    for (count=0; count < KEYALIAS_LENGTH; count++){
        if (is_joystick)
            drawjoystickkeyrow(keypickoptions[count], keyaliases[count], preset, count, first_item, 144);
        else
            drawkeyboardkeyrow(keypickoptions[count], keyaliases[count], preset, count, first_item, 144);
    }

    if (is_joystick)
    if (preset->joysticknum!=-1)
      drawtext(joystick[preset->joysticknum].name,320,128,10,0.5f,0.5f,0.5f,1.0f);

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    if (is_joystick){
        bindjoystickkeys(preset, first_item);
    }
    else{
        bindkeyboardkeys(preset, first_item);
    }
  }

  resetmenuitems();

  joystickmenu=1;
}

void optionsmenu(void){
  // set_profile_buttons_menu();
  int count,count2,count3;
  int x;
  count=0;
  int deletepreset = 0;
  int picking_dataset = 0;
  int debug_options = 0;
  int num_known_datapacks = 0;
  char known_datapacks[32][32];

  joystickmenu=0;
  resetmenuitems();

  while (!menuitem[0].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(TXT_BACK,0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    
    int first_keyboard_preset_item = -1;
    int create_new_keyboard_preset = -1;
    int first_joystick_preset_item = -1;
    int create_new_joystick_preset = -1;
    int preset_name_input = -1;
    int create_button = -1;

    if (!picking_dataset && !debug_options){
        first_keyboard_preset_item = numofmenuitems;;
        for (count = 0; count < numkeyboardpresets; count++){
            createmenuitem(keyboardpresets[count].name,16,140 + 40 * count,16,1.0f,1.0f,1.0f,1.0f);
        }

        create_new_keyboard_preset = createmenuitem(TXT_NEW_PRESET,16,140 + 40 * count,16,1.0f,1.0f,1.0f,1.0f);

        first_joystick_preset_item = numofmenuitems;
        for (count = 0; count < numjoystickpresets; count++){
            createmenuitem(joystickpresets[count].name,320,140 + 40 * count,16,1.0f,1.0f,1.0f,1.0f);
        }

        create_new_joystick_preset = createmenuitem(TXT_NEW_PRESET,320,140 + 40 * count,16,1.0f,1.0f,1.0f,1.0f);

        x = 16;
        if (clicked_keyboard_preset != -1){
            count=140+clicked_keyboard_preset*40-16;
            preset_name_input = createmenuitem("                ",x,count,16,1.0f,1.0f,1.0f,1.0f);
            setmenuitem(MO_STRINGINPUT,keyboardpresets[clicked_keyboard_preset].name);
            menuitem[numofmenuitems-1].active = 1;
            if (keyboardpresets[clicked_keyboard_preset].name[0]!=0){
                create_button = createmenuitem(TXT_CREATE,x,count+32,16,1.0f,1.0f,1.0f,1.0f);
            }
        }
        x = 320;
        if (clicked_joystick_preset != -1){
            count=140+clicked_joystick_preset*40-16;
            preset_name_input = createmenuitem("                ",x,count,16,1.0f,1.0f,1.0f,1.0f);
            setmenuitem(MO_STRINGINPUT,joystickpresets[clicked_joystick_preset].name);
            menuitem[numofmenuitems-1].active = 1;
            if (joystickpresets[clicked_joystick_preset].name[0]!=0){
                create_button = createmenuitem(TXT_CREATE,x,count+32,16,1.0f,1.0f,1.0f,1.0f);
            }
        }
        int delete_preset = createmenuitem(TXT_DELETE_PRESET,320|TEXT_CENTER,140 + max(numkeyboardpresets, numjoystickpresets) * 40 + 32,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &deletepreset);
    }

    if (option.sound)
      {
      createmenuitem(TXT_SOUND_ON,0,32,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_SET,&option.sound,0);
      }
    else
      {
      createmenuitem(TXT_SOUND_OFF,0,32,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_SET,&option.sound,1);
      }
    if (option.music)
      {
      createmenuitem(TXT_MUSIC_ON,0,48,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_SET,&option.music,0);
      }
    else
      {
      createmenuitem(TXT_MUSIC_OFF,0,48,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_SET,&option.music,1);
      }
    int datapack_picker = createmenuitem(TXT_PICK_DATAPACK,0,64,16,1.0f,1.0f,1.0f,1.0f);
    int loaded_datapack_item = -1;
    int y = 64;
    int offset = 16;
    if (!picking_dataset){
        loaded_datapack_item = createmenuitem(loaded_datapack,0,y+offset,16,1.0f,1.0f,1.0f,1.0f);
    }
    else{
        loaded_datapack_item = numofmenuitems;
        for (count = 0; count < num_known_datapacks; count++){
          createmenuitem(known_datapacks[count],0, y+offset*(count+1),16,1.0f,1.0f,1.0f,1.0f);
      }
    }
    y = 0;
    int video_options = createmenuitem(TXT_VIDEOOPTIONS,(640|TEXT_END),0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_V);
    y += 16;
    createmenuitem(TXT_DEBUG_OPTIONS,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_TOGGLE, &debug_options);
    if (debug_options){
        y += 16;
        createmenuitem(TXT_debug_level_saveload,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_level_saveload);
        y += 16;
        createmenuitem(TXT_debug_texture_load,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_texture_load);
        y += 16;
        createmenuitem(TXT_debug_character_positions,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_character_positions);
        y += 16;
        createmenuitem(TXT_debug_objectnums,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_objectnums);
        y += 16;
        createmenuitem(TXT_debug_objectlinks,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_objectlinks);
        y += 16;
        createmenuitem(TXT_debug_unlocks,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_unlocks);
        y += 16;
        createmenuitem(TXT_debug_grid_blocknums,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_grid_blocknums);
        y += 16;
        createmenuitem(TXT_debug_next_level,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_next_level);
        y += 16;
        createmenuitem(TXT_debug_render_bonds,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_render_bonds);
        y += 16;
        createmenuitem(TXT_debug_render_player_vertices,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_render_player_vertices);
        y += 16;
        createmenuitem(TXT_debug_render_level_lines,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_render_level_lines);
        y += 16;
        createmenuitem(TXT_debug_ropeedit_points,(640|TEXT_END),y,16,1.0f,1.0f,1.0f,1.0f);
        setmenuitem(MO_TOGGLE, &debug_ropeedit_points);
    }
    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    setuptextdisplay();

    if (!picking_dataset && !debug_options){
        drawtext(TXT_KEYBOARD_PRESETS,16+16,140-20,10,1.0f,1.0f,1.0f,1.0f);
        drawtext(TXT_JOYSTICK_PRESETS,320+16,140-20,10,1.0f,1.0f,1.0f,1.0f);
        drawtext(TXT_JOYSTICKS,320,90,10,1.0f,1.0f,1.0f,1.0f);
        drawtext(TXT_CONNECTED": /i",320,100,10,1.0f,1.0f,1.0f,1.0f, numofjoysticks);
    }

    //drawtext("delete_preset /i",640|TEXT_END,470,10,1.0f,1.0f,1.0f,1.0f, delete_preset);
    //drawtext("create_button /i",640|TEXT_END,460,10,1.0f,1.0f,1.0f,1.0f, create_button);
    //drawtext("numkeyboardpresets /i",640|TEXT_END,450,10,1.0f,1.0f,1.0f,1.0f, numkeyboardpresets);
    //drawtext("numjoystickpresets /i",640|TEXT_END,440,10,1.0f,1.0f,1.0f,1.0f, numjoystickpresets);
    //drawtext("first_keyboard_preset_item /i",640|TEXT_END,430,10,1.0f,1.0f,1.0f,1.0f, first_keyboard_preset_item);
    //drawtext("create_new_keyboard_preset /i",640|TEXT_END,420,10,1.0f,1.0f,1.0f,1.0f, create_new_keyboard_preset);
    //drawtext("first_joystick_preset_item /i",640|TEXT_END,410,10,1.0f,1.0f,1.0f,1.0f, first_joystick_preset_item);
    //drawtext("create_new_joystick_preset /i",640|TEXT_END,400,10,1.0f,1.0f,1.0f,1.0f, create_new_joystick_preset);
    //drawtext("deletepreset /i",640|TEXT_END,390,10,1.0f,1.0f,1.0f,1.0f, deletepreset);

    //drawtext(TXT_PLAYER,0,80,16,1.0f,1.0f,1.0f,1.0f);
    //drawtext(TXT_KEY,320,80,16,1.0f,1.0f,1.0f,1.0f);
    //drawtext(TXT_JOYSTICK,480,80,16,1.0f,1.0f,1.0f,1.0f);

    drawmenuitems();

    drawsliderbars();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);

    // raw mouse events should be handled first because otherwise they might override button actions
    if (mouse.lmb && !prevmouse.lmb){
        // deactivate any new preset creations

        if (!((create_button != -1 && menuitem[create_button].active) || (preset_name_input != -1 && menuitem[preset_name_input].active))){
            clicked_keyboard_preset = -1;
            clicked_joystick_preset = -1;
        }

        // click should start on the slider
        if (mouse.x>=160 && mouse.x<288){
            if (mouse.y>=32 && mouse.y<48)
                slider_was_clicked = 0;
            if (mouse.y>=48 && mouse.y<64)
                slider_was_clicked = 1;
        }
    }

    // but then mouse can be moved anywhere
    if (mouse.lmb){
      if (slider_was_clicked == 0)
        option.soundvolume=max(min((float)(mouse.x-160)/128.0f, 1), 0);
      if (slider_was_clicked == 1)
        option.musicvolume=max(min((float)(mouse.x-160)/128.0f, 1), 0);
    }

    if (!mouse.lmb && prevmouse.lmb){
        slider_was_clicked = -1;
    }

    if (!picking_dataset){
        // clicked a keyboard preset from a list
        for (count = first_keyboard_preset_item; count < create_new_keyboard_preset; count++){
            if (menuitem[count].active){
                if (deletepreset){
                    for (int preset_index = count - first_keyboard_preset_item; preset_index < numkeyboardpresets-1; preset_index++){
                        keyboardpresets[preset_index] = keyboardpresets[preset_index+1];
                    }
                    numkeyboardpresets--;
                    deletepreset = 0;
                    menuitem[count].active = 0;
                }
                else{
                    set_profile_buttons_menu(&keyboardpresets[count - first_keyboard_preset_item], 0);
                }
            }
        }
        // clicked a joystick preset from a list
        for (count = first_joystick_preset_item; count < create_new_joystick_preset; count++){
            if (menuitem[count].active){
                if (deletepreset){
                    for (int preset_index = count - first_joystick_preset_item; preset_index < numjoystickpresets-1; preset_index++){
                        joystickpresets[preset_index] = joystickpresets[preset_index+1];
                    }
                    numjoystickpresets--;
                    deletepreset = 0;
                    menuitem[count].active = 0;
                }
                else{
                    set_profile_buttons_menu(&joystickpresets[count - first_joystick_preset_item], 1);
                }
            }
        }
        // creating new presets
        if (menuitem[create_new_keyboard_preset].active) {
            clicked_keyboard_preset = numkeyboardpresets;
            clicked_joystick_preset = -1;
            menuitem[create_new_keyboard_preset].active = 0;
        }
        if (menuitem[create_new_joystick_preset].active) {
            clicked_keyboard_preset = -1;
            clicked_joystick_preset = numjoystickpresets;
            menuitem[create_new_joystick_preset].active = 0;
        }
        // filled in the name of the preset, create button was pressed
        if (create_button != -1)
        if (menuitem[create_button].active){
            if(clicked_keyboard_preset != -1){
                numkeyboardpresets++;
                clicked_keyboard_preset = -1;
                set_profile_buttons_menu(&keyboardpresets[numkeyboardpresets-1], 0);
            }
            if(clicked_joystick_preset != -1){
                numjoystickpresets++;
                clicked_joystick_preset = -1;
                set_profile_buttons_menu(&joystickpresets[numjoystickpresets-1], 1);
            }
            menuitem[create_button].active = 0;
        }
        if (menuitem[datapack_picker].active || menuitem[loaded_datapack_item].active){
            char path[MAX_PATH];
            strcpy(path, datapacks_folder);
            listfiles(path,"*",known_datapacks,1);
            num_known_datapacks=0;
            while (known_datapacks[num_known_datapacks][0]!=0)
              num_known_datapacks++;

            menuitem[datapack_picker].active = 0;
            menuitem[loaded_datapack_item].active = 0;
            picking_dataset = 1;
        }
    }
    else{
        // picking a datapack
        if (menuitem[datapack_picker].active){
            picking_dataset = 0;
            menuitem[datapack_picker].active = 0;
        }
        for (count = 0; count < num_known_datapacks; count++){
            if (menuitem[loaded_datapack_item + count].active){
                strcpy(loaded_datapack, known_datapacks[count]);
                picking_dataset = 0;
                menuitem[loaded_datapack_item + count].active = 0;
                load_datapack();
            }
        }
    }
    if (menuitem[video_options].active)
      videooptionsmenu();
  }
  
    clicked_keyboard_preset = -1;
    clicked_joystick_preset = -1;
    resetmenuitems();

    joystickmenu=1;
}

void load_datapack(void){
  load_font_and_mouse();
  load_all_static_textures();
  loadanimations();
  if (config.sound){
    loadoggs();
    loadwavs();
  }
}

void videooptionsmenu(void)
  {
  int count,count2;
  int prevvideomodenum;
  int videomodenum;
  char restext[64];
  int fullscreen;
  int bitsperpixel;
  char *glvendor;
  char *glrenderer;
  char *glversion;

  glvendor=(char *) glGetString(GL_VENDOR);
  glrenderer=(char *) glGetString(GL_RENDERER);
  glversion=(char *) glGetString(GL_VERSION);

  videomodenum=-1;
  for (count=0;count<numofsdlvideomodes;count++)
  if (sdlvideomode[count].bitsperpixel==32)
    {
    if (windowinfo.resolutionx==sdlvideomode[count].resolutionx)
    if (windowinfo.resolutiony==sdlvideomode[count].resolutiony)
      videomodenum=count;
    }
  prevvideomodenum=videomodenum;
  fullscreen=windowinfo.fullscreen;
  bitsperpixel=windowinfo.bitsperpixel;

  resetmenuitems();

  while (!menuitem[0].active && !menuitem[1].active && !windowinfo.shutdown)
    {
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    numofmenuitems=0;
    createmenuitem(TXT_BACK,0,0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_ESC);
    createmenuitem(TXT_APPLY,(640|TEXT_END),0,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_HOTKEY,SCAN_A);

    count2=64;
    sprintf_s(restext, 64, "%d videomodes found", numofsdlvideomodes);
    drawtext(restext, 0, 32, 16, 1.0f, 1.0f, 1.0f, 1.0f);
    for (count=numofsdlvideomodes-1;count>=0;count--)
    if (sdlvideomode[count].resolutionx>=640)
    if (sdlvideomode[count].bitsperpixel==32)
      {
      sprintf_s(restext, 64, "%dx%d:%d", sdlvideomode[count].resolutionx, sdlvideomode[count].resolutiony, sdlvideomode[count].bitsperpixel);
      createmenuitem(restext,0,count2,16,1.0f,1.0f,1.0f,1.0f);
      setmenuitem(MO_SET,&videomodenum,count);

      count2+=16;
      }

    count=48;
    createmenuitem(TXT_FULLSCREEN,320,count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_TOGGLE,&fullscreen);
    setmenuitem(MO_HOTKEY,SCAN_F);
    count+=16;
    count+=16;
    count+=16;
    createmenuitem(TXT_16BIT,320,count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_SET,&bitsperpixel,16);
    count+=16;
    createmenuitem(TXT_32BIT,320,count,16,1.0f,1.0f,1.0f,1.0f);
    setmenuitem(MO_SET,&bitsperpixel,32);
    count+=16;

    checksystemmessages();
    checkkeyboard();
    checkmouse();
    checkjoystick();
    checkmenuitems();

    setuptextdisplay();

    drawtext(TXT_RESOLUTION,0,48,16,1.0f,1.0f,1.0f,1.0f);
    drawtext(TXT_COLOR,320,80,16,1.0f,1.0f,1.0f,1.0f);

    count=400;
    drawtext(TXT_OPENGLINFO,0,count,16,1.0f,1.0f,1.0f,1.0f);
    count+=16;
    if (glext.multitexture)
      drawtext("GL_ARB_multitexture",0,count,12,0.0f,1.0f,0.0f,1.0f);
    else
      drawtext("GL_ARB_multitexture",0,count,12,0.25f,0.25f,0.25f,1.0f);
    count+=12;
    if (glext.texture_env_dot3)
      drawtext("GL_ARB_texture_env_dot3",0,count,12,0.0f,1.0f,0.0f,1.0f);
    else
      drawtext("GL_ARB_texture_env_dot3",0,count,12,0.25f,0.25f,0.25f,1.0f);
    count+=12;
    drawtext("GL_VERSION: /s",0,count,12,1.0f,1.0f,1.0f,1.0f,glversion);
    count+=12;
    drawtext("GL_VENDOR: /s",0,count,12,1.0f,1.0f,1.0f,1.0f,glvendor);
    count+=12;
    drawtext("GL_RENDERER: /s",0,count,12,1.0f,1.0f,1.0f,1.0f,glrenderer);
    count+=12;

    drawmenuitems();

    drawmousecursor(768+font.cursornum,mouse.x,mouse.y,16,1.0f,1.0f,1.0f,1.0f);

    SDL_GL_SwapWindow(globalwindow);
    }

  if (menuitem[1].active)
    {
    if (prevvideomodenum==videomodenum)
    if (windowinfo.fullscreen==fullscreen)
    if (windowinfo.bitsperpixel==bitsperpixel)
      return;

    if (videomodenum==-1)
      return;

    windowinfo.resolutionx=sdlvideomode[videomodenum].resolutionx;
    windowinfo.resolutiony=sdlvideomode[videomodenum].resolutiony;
    windowinfo.fullscreen=fullscreen;
    windowinfo.bitsperpixel=bitsperpixel;
  
    if (windowinfo.bitsperpixel==16)
      {
      SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
      SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,6);
      SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
      SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);
      }
    if (windowinfo.bitsperpixel==32)
      {
      SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
      SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
      SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
      SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
      }

    SDL_DisplayMode mode = { (windowinfo.bitsperpixel==32) ? SDL_PIXELFORMAT_RGB888 : SDL_PIXELFORMAT_RGB565, windowinfo.resolutionx, windowinfo.resolutiony, 0, 0 };
    SDL_SetWindowDisplayMode(globalwindow, &mode);
    SDL_SetWindowSize(globalwindow, windowinfo.resolutionx, windowinfo.resolutiony);
    (windowinfo.fullscreen) ? SDL_SetWindowFullscreen(globalwindow, SDL_WINDOW_FULLSCREEN) : SDL_SetWindowFullscreen(globalwindow, 0);

    for (count=0;count<2048;count++)
      if (texture[count].sizex!=0)
        setuptexture(count);
    }

  resetmenuitems();
  }

void drawsliderbars(void)
  {
  float vec[3];

  glDisable(GL_TEXTURE_2D);

  glBegin(GL_QUADS);

  if (option.sound)
    {
    glColor4f(0.25f,0.25f,0.25f,1.0f);

    vec[0]=160.0f;
    vec[1]=40.0f-1.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
  
    vec[0]=160.0f+128.0f;
    vec[1]=40.0f-5.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
  
    vec[0]=160.0f+128.0f;
    vec[1]=40.0f+5.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
  
    vec[0]=160.0f;
    vec[1]=40.0f+1.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    glColor4f(0.75f,0.75f,0.75f,1.0f);

    vec[0]=160.0f+option.soundvolume*128.0f-2.0f;
    vec[1]=40.0f-7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    vec[0]=160.0f+option.soundvolume*128.0f+2.0f;
    vec[1]=40.0f-7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    vec[0]=160.0f+option.soundvolume*128.0f+2.0f;
    vec[1]=40.0f+7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    vec[0]=160.0f+option.soundvolume*128.0f-2.0f;
    vec[1]=40.0f+7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
    }
  if (option.music)
    {
    glColor4f(0.25f,0.25f,0.25f,1.0f);

    vec[0]=160.0f;
    vec[1]=56.0f-1.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
  
    vec[0]=160.0f+128.0f;
    vec[1]=56.0f-5.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
  
    vec[0]=160.0f+128.0f;
    vec[1]=56.0f+5.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
  
    vec[0]=160.0f;
    vec[1]=56.0f+1.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    glColor4f(0.75f,0.75f,0.75f,1.0f);

    vec[0]=160.0f+option.musicvolume*128.0f-2.0f;
    vec[1]=56.0f-7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    vec[0]=160.0f+option.musicvolume*128.0f+2.0f;
    vec[1]=56.0f-7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    vec[0]=160.0f+option.musicvolume*128.0f+2.0f;
    vec[1]=56.0f+7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);

    vec[0]=160.0f+option.musicvolume*128.0f-2.0f;
    vec[1]=56.0f+7.0f;
    convertscreenvertex(vec,font.sizex,font.sizey);
    glVertex3fv(vec);
    }

  glEnd();

  glEnable(GL_TEXTURE_2D);
  }
