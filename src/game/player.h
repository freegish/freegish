#ifndef GISH_GAME_PLAYER_H
#define GISH_GAME_PLAYER_H
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

void playermenu(void);
void saveplayers(void);
void loadplayers(void);
void playerstartmenu(void);
void playerdifficultymenu(void);
void collectionmenu(void);
void singlelevelmenu(void);

typedef enum
{
	WON_NORMAL = 0,
	WON_HARD = 1,
	WON_LUDICROUS = 2,
	WON_MINI_GISH = 3,
} unlocks;

typedef struct
  {
  char name[32];
  int levelnum;
  int numoflives;
  int totalscore;
  int difficulty;
  int gamepassed;
  int highscore;
  int unlock[16];
  } _player;

extern int playernum;
extern _player player[16];

#endif /* GISH_GAME_PLAYER_H */
