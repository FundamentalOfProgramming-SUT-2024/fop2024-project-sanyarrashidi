#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<time.h>
#include "map.h"
#include "menus.h"


void game_ui(Player*);
void move_player(Player*, int, int);
bool found_hidden_door(int, int, int, int);