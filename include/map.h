#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<time.h>
#include<stdbool.h>


typedef struct {
    int x;
    int y;
    bool found;
} Trap;


typedef struct {
    int x;
    int y;
    bool claimed;
} Coin;


typedef struct {
    char type; // 'M' for Mace, 'S' for Sword, 'A' for Arrow, 'W' for Magic Wand, 'D' for Dagger
    int damage;
    int ammo;
    char symbol;
    int x;
    int y;
    bool claimed;
} Weapon;


typedef struct {
    char type; // 'H' for Health, 'S' for Speed, 'D' for Damage
    int amount;
    char symbol;
    int x;
    int y;
    bool claimed;
} Spell;


typedef struct {
    char type; // 'N' for Normal, 'S' for Special, 'M' for Magic
    int amount;
    char symbol;
    int x;
    int y;
    bool claimed;
} Food;


typedef struct {
    int total_rooms;
    int corner_x;
    int corner_y;
    int height;
    int width;
    int door_count;
    int* doors_x;
    int* doors_y;
    int pillar_count;
    int* pillars_x;
    int* pillars_y;
    int* hidden_x;
    int* hidden_y;
    char type; // 'R' for regular, 'E' for enchant, 'F' for fight, 'T' for treasure,
    int trap_count;
    Trap** traps;
    int coin_count;
    Coin** coins;
    int weapon_count;
    Weapon** weapons;
    int spell_count;
    Spell** spells;
    int food_count;
    Food** food;
    bool visited; 
} Room;


typedef struct {
    Room* parent;
    int x;
    int y;
    int edge;
} Door;


Room** generate_map();
Room** generate_rooms(Room**, int);
int check_rooms(Room**, Room*, int);
void display_rooms(Room**, int);
void display_single_room(Room*);
int compare_rooms(const void*, const void*);
void generate_corridors(Room**, int);
void draw_corridor(Door, Door, Room**, int);
int is_door_available(int, int);
void connect_doors(Door, Door);
char** save_map();
Room* find_room_by_door(Room**, int, int);