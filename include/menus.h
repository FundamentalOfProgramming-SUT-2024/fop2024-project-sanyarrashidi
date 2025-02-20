#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<ctype.h>
#include "authenticate.h"
#include "music.h"


char main_menu(Player*);
void show_main_menu(Player*);
void show_profile(Player*);
void show_scoreboard(Player*);
int compare_players_by_score(void*, void*);
void settings(Player*);
void show_settings(Player*);
void change_difficulty(Player*);
void change_color(Player*);
void change_hero(Player*);
Player* extract_players_stats(int*);
// void extract_tracks(Mix_Music**);