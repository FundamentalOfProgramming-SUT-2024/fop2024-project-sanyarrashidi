#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<time.h>


typedef struct {
    int corner_x;
    int corner_y;
    int height;
    int width;
    int door_count;
    int* doors_x;
    int* doors_y;
} Room;


void generate_map();
Room** generate_rooms(Room**, int);
int check_rooms(Room**, Room*, int);
void display_rooms(Room**, int);