#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<wchar.h>
#include<time.h>
#include<unistd.h>
#include "map.h"
#include "menus.h"


void game_ui(Player*);
void move_player(Player*, int, int);
bool found_hidden_door(int, int, int, int);
bool stepped_on_trap(Room**, int, int, int);
Coin* stepped_on_loot(Room**, int, int, int);