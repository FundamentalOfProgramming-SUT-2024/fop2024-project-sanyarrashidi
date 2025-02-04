#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include "map.h"
#include "menus.h"


void save_rooms(Room**, Player*, int);
Room** read_rooms(Player*, int);
void save_corridors_to_file(Player*, char**, int, int, int, bool);
char** read_corridors(Player*, int, int, int, bool);
void save_player(Player*);
void save_backpack(Player*, Backpack*);
Backpack* read_backpack(Player*);