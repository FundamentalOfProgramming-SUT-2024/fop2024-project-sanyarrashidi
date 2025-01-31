#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<wchar.h>
#include<time.h>
#include<unistd.h>
#include "map.h"
#include "menus.h"


typedef struct {
    int count_weapons;
    Weapon* default_weapon;
    Weapon** weapons;
    int count_spells;
    Spell* default_spell;
    Spell** spells;
    int count_food;
    Food* default_food;
    Food** food;
} Backpack;


void game_ui(Player*);
void move_player(Player*, int, int);
bool found_hidden_door(int, int, int, int);
bool stepped_on_trap(Room**, int, int, int);
Coin* stepped_on_loot(Room**, int, int, int);
Spell* stepped_on_spell(Room**, int, int, int);
Food* stepped_on_food(Room**, int, int, int);
Weapon* stepped_on_weapon(Room**, int, int, int);
void inventory_menu(Player*, Backpack*);
void weapon_menu(Player*, Backpack*);
void show_defaults(Player*, Backpack*);