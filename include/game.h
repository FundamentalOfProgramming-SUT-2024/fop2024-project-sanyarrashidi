#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<wchar.h>
#include<time.h>
#include<unistd.h>
#include<sys/time.h>
#include "database.h"


void game_ui(Player*, bool);
int load_level(Player*, Backpack*, Room**, char**, char**, int*, int);
void move_player(Player*, int, int);
bool found_hidden_door(int, int, int, int);
bool stepped_on_trap(Room**, Player*, int, int, int);
Coin* stepped_on_loot(Room**, int, int, int);
Spell* stepped_on_spell(Room**, int, int, int);
Food* stepped_on_food(Room**, int, int, int);
Weapon* stepped_on_weapon(Room**, int, int, int);
void inventory_menu(Player*, Backpack*);
void weapon_menu(Player*, Backpack*);
void show_defaults(Player*, Backpack*);
void show_health_bar(Player*);
void show_hunger_bar(Player*);
long get_current_time();
void death(int);
Room* get_current_room(Room**, int, int);
void show_game_bar(Player*, Backpack*, int);
bool show_pause_menu(Player*, int);
void show_options_menu(Player*, int);