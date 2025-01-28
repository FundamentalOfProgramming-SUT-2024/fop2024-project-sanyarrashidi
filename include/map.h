#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<time.h>
#include<stdbool.h>


typedef struct {
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