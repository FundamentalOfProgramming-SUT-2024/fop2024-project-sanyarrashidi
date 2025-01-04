#include "map.h"


void generate_map() {
    int height, width;
    getmaxyx(stdscr, height, width);
    srand(time(NULL));
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    int total_rooms = rand() % 5 + 6;
    Room** rooms = (Room**) calloc(total_rooms, sizeof(Room*));
    rooms = generate_rooms(rooms, total_rooms);
    display_rooms(rooms, total_rooms);
    getch();
}


Room** generate_rooms(Room** rooms, int total_rooms) {
    int height, width;
    getmaxyx(stdscr, height, width);
    int room_counter = 0;
    while (room_counter < total_rooms) {
        Room* new_room = (Room*) malloc(sizeof(Room));
        new_room->height = rand() % 9 + 6;
        new_room->width = rand() % 9 + 6;
        new_room->corner_x = rand() % (width - new_room->width - 4) + 2;
        new_room->corner_y = rand() % (height - new_room->height - 4) + 2;
        if (!check_rooms(rooms, new_room, room_counter)) 
            continue;

        new_room->door_count = rand() % 2 + 1;
        new_room->doors_x = (int*) calloc(new_room->door_count, sizeof(int));
        new_room->doors_y = (int*) calloc(new_room->door_count, sizeof(int));
        // placing each door on 1 edge
        int edges[4] = {0, 0, 0, 0};
        for (int i = 0; i < new_room->door_count; i++) {
            int edge = rand() % 4 + 1;
            while (edges[edge - 1]) {
                edge = rand() % 4 + 1;
            }
            edges[edge - 1] = 1;
            switch (edge) {
            // top
            case 1:
                new_room->doors_x[i] = new_room->corner_x + 1 + rand() % (new_room->width - 2);
                new_room->doors_y[i] = new_room->corner_y;
                break;
            // right
            case 2:
                new_room->doors_x[i] = new_room->corner_x + new_room->width - 1;
                new_room->doors_y[i] = new_room->corner_y + 1 + rand() % (new_room->height - 2);
                break;
            // bottom
            case 3:
                new_room->doors_x[i] = new_room->corner_x + 1 + rand() % (new_room->width - 2);
                new_room->doors_y[i] = new_room->corner_y + new_room->height - 1;
                break;
            // left
            case 4:
                new_room->doors_x[i] = new_room->corner_x;
                new_room->doors_y[i] = new_room->corner_y + 1 + rand() % (new_room->height - 2);
                break;
            default:
                break;
            }
        }
        rooms[room_counter] = new_room;
        room_counter++;
    }

    return rooms;
}


int check_rooms(Room** rooms, Room* room, int room_count) {
    for (int i = 0; i < room_count; i++) {
        // at least 10 spaces between rooms
        if (!(room->corner_x + room->width + 10 <= rooms[i]->corner_x || // Right
              room->corner_x >= rooms[i]->corner_x + rooms[i]->width + 10 || // Left
              room->corner_y + room->height + 10 <= rooms[i]->corner_y || // Below
              room->corner_y >= rooms[i]->corner_y + rooms[i]->height + 10)) { // Above
            return 0;
        }
    }

    return 1;
}


void display_rooms(Room** rooms, int total_rooms) {
    clear();
    for (int i = 0; i < total_rooms; i++) {
        move(rooms[i]->corner_y, rooms[i]->corner_x);
        attron(A_UNDERLINE | COLOR_PAIR(4));
        for (int j = 0; j < rooms[i]->width; j++) {
            printw("-");
        }
        attroff(A_UNDERLINE | COLOR_PAIR(4));
        
        for (int j = 0; j < rooms[i]->height - 2; j++) {
            move(rooms[i]->corner_y + j + 1, rooms[i]->corner_x);
            attron(A_UNDERLINE | COLOR_PAIR(3));
            printw("|");
            attroff(A_UNDERLINE | COLOR_PAIR(3));
            for (int k = 0; k < rooms[i]->width - 2; k++) {
                printw(".");
            }
            attron(A_UNDERLINE | COLOR_PAIR(3));
            printw("|");
            attroff(A_UNDERLINE | COLOR_PAIR(3));
        }

        move(rooms[i]->corner_y + rooms[i]->height - 1, rooms[i]->corner_x);
        attron(A_UNDERLINE | COLOR_PAIR(4));
        for (int j = 0; j < rooms[i]->width; j++) {
            printw("-");
        }
        attroff(A_UNDERLINE | COLOR_PAIR(4));

        for (int j = 0; j < rooms[i]->door_count; j++) {
            move(rooms[i]->doors_y[j], rooms[i]->doors_x[j]);
            attron(COLOR_PAIR(2));
            printw("+");
            attroff(COLOR_PAIR(2));
        }
    }

    refresh();
}