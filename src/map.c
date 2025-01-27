#include "map.h"


void generate_map() {
    int height, width;
    getmaxyx(stdscr, height, width);
    srand(time(NULL));
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    int total_rooms = rand() % 2 + 6;
    Room** rooms = (Room**) calloc(total_rooms, sizeof(Room*));
    rooms = generate_rooms(rooms, total_rooms);
    display_rooms(rooms, total_rooms);
    for (int i = 0; i < width; i++) {
        attron(A_UNDERLINE);
        mvaddch(4, i, '-');
        attroff(A_UNDERLINE);
    }
    generate_corridors(rooms, total_rooms);
    getch();
}


Room** generate_rooms(Room** rooms, int total_rooms) {
    int height, width;
    getmaxyx(stdscr, height, width);
    int room_counter = 0;
    int to_enchant_room_index = rand() % (total_rooms - 3) + 1;
    while (room_counter < total_rooms) {
        Room* new_room = (Room*) malloc(sizeof(Room));
        new_room->height = rand() % 9 + 6;
        new_room->width = rand() % 9 + 6;
        new_room->corner_x = rand() % (width - new_room->width - 5) + 3;
        new_room->corner_y = rand() % (height - new_room->height - 10) + 8;
        if (!check_rooms(rooms, new_room, room_counter)) 
            continue;

        new_room->door_count = 3;
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

    qsort(rooms, total_rooms, sizeof(Room**), compare_rooms);
    
    for (int i = 1; i < rooms[0]->door_count; i++) {
        rooms[0]->doors_x[i] = NULL;
        rooms[0]->doors_y[i] = NULL;
    }

    rooms[0]->door_count = 1;
    rooms[0]->type = 'R';

    for (int i = 1; i < total_rooms - 1; i++) {
        if (i == to_enchant_room_index) {
            rooms[i]->type = 'R';
            rooms[i + 1]->type = 'E';
            rooms[i + 1]->door_count = 1;
            rooms[i + 1]->doors_x[1] = NULL;
            rooms[i + 1]->doors_y[1] = NULL;
            rooms[i + 1]->doors_x[2] = NULL;
            rooms[i + 1]->doors_y[2] = NULL;
            i++;
        }
        else {
            rooms[i]->type = 'R';
            rooms[i]->door_count = 2;
            rooms[i]->doors_x[2] = NULL;
            rooms[i]->doors_y[2] = NULL;
        }
    }

    for (int i = 1; i < rooms[total_rooms - 1]->door_count; i++) {
        rooms[total_rooms - 1]->doors_x[i] = NULL;
        rooms[total_rooms - 1]->doors_y[i] = NULL;
    }

    rooms[total_rooms - 1]->door_count = 1;
    rooms[total_rooms - 1]->type = 'R';

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
        rooms[i]->type == 'E' ? attron(A_UNDERLINE | COLOR_PAIR(5)) : attron (A_UNDERLINE | COLOR_PAIR(4));
        for (int j = 0; j < rooms[i]->width; j++) {
            printw("-");
        }
        rooms[i]->type == 'E' ? attroff(A_UNDERLINE | COLOR_PAIR(5)) : attroff(A_UNDERLINE | COLOR_PAIR(4));
        
        for (int j = 0; j < rooms[i]->height - 2; j++) {
            move(rooms[i]->corner_y + j + 1, rooms[i]->corner_x);
            rooms[i]->type == 'E' ? attron(A_UNDERLINE | COLOR_PAIR(6)) : attron(A_UNDERLINE | COLOR_PAIR(3));
            printw("|");
            rooms[i]->type == 'E' ? attroff(A_UNDERLINE | COLOR_PAIR(6)) : attroff(A_UNDERLINE | COLOR_PAIR(3));
            for (int k = 0; k < rooms[i]->width - 2; k++) {
                printw(".");
            }
            rooms[i]->type == 'E' ? attron(A_UNDERLINE | COLOR_PAIR(6)) : attron(A_UNDERLINE | COLOR_PAIR(3));
            printw("|");
            rooms[i]->type == 'E' ? attroff(A_UNDERLINE | COLOR_PAIR(6)) : attroff(A_UNDERLINE | COLOR_PAIR(3));
        }

        move(rooms[i]->corner_y + rooms[i]->height - 1, rooms[i]->corner_x);
        rooms[i]->type == 'E' ? attron(A_UNDERLINE | COLOR_PAIR(5)) : attron (A_UNDERLINE | COLOR_PAIR(4));
        for (int j = 0; j < rooms[i]->width; j++) {
            printw("-");
        }
        rooms[i]->type == 'E' ? attroff(A_UNDERLINE | COLOR_PAIR(5)) : attroff(A_UNDERLINE | COLOR_PAIR(4));

        for (int j = 0; j < rooms[i]->door_count; j++) {
            move(rooms[i]->doors_y[j], rooms[i]->doors_x[j]);
            attron(COLOR_PAIR(2));
            printw("/");
            attroff(COLOR_PAIR(2));
        }

        int num_pillars = rand() % 2 + 1;
        for (int j = 0; j < num_pillars; j++) {
            int pillar_x = rooms[i]->corner_x + 2 + rand() % (rooms[i]->width - 4);
            int pillar_y = rooms[i]->corner_y + 2 + rand() % (rooms[i]->height - 4);
            while (mvinch(pillar_y + 1, pillar_x) == '/' ||
                mvinch(pillar_y - 1, pillar_x) == '/' || 
                mvinch(pillar_y, pillar_x + 1) == '/' || 
                mvinch(pillar_y, pillar_x - 1) == '/' ||
                mvinch(pillar_y - 1, pillar_x) == 'O' || 
                mvinch(pillar_y, pillar_x + 1) == 'O' || 
                mvinch(pillar_y, pillar_x - 1) == 'O' ||
                mvinch(pillar_y + 1, pillar_x) == 'O') {
                    pillar_x = rooms[i]->corner_x + 2 + rand() % (rooms[i]->width - 4);
                    pillar_y = rooms[i]->corner_y + 2 + rand() % (rooms[i]->height - 4);
            }
            mvaddch(pillar_y, pillar_x, 'O');
        }
    }

    refresh();
}


int compare_rooms(const void* a, const void* b) {
    Room* first = *(Room**) a;
    Room* second = *(Room**) b;
    
    return first->corner_x - second->corner_x;
    
}

void generate_corridors(Room** rooms, int total_rooms) {
    int height, width;
    getmaxyx(stdscr, height, width);

    for (int i = 1; i < total_rooms; i++) {
        Room* prev;
        if (rooms[i-1]->type == 'E') {
            prev = rooms[i-2];
        }
        else {
            prev = rooms[i-1];
        }
        Room* curr = rooms[i];

        int chosen_door_prev = rand() % prev->door_count;
        while (mvinch(prev->doors_y[chosen_door_prev], prev->doors_x[chosen_door_prev]) == '+') {
            chosen_door_prev = rand() % prev->door_count;
        }
        
        mvaddch(prev->doors_y[chosen_door_prev], prev->doors_x[chosen_door_prev], '+');

        int chosen_door_curr = rand() % curr->door_count;
        while (mvinch(curr->doors_y[chosen_door_curr], curr->doors_x[chosen_door_curr]) == '+') {
            chosen_door_curr = rand() % curr->door_count;
        }
        
        mvaddch(curr->doors_y[chosen_door_curr], curr->doors_x[chosen_door_curr], '+');
        
        Door closest_prev = {prev, prev->doors_x[chosen_door_prev], prev->doors_y[chosen_door_prev]};
        Door closest_curr = {curr, curr->doors_x[chosen_door_curr], curr->doors_y[chosen_door_curr]};

        if (prev->doors_x[chosen_door_prev] == prev->corner_x) {
            closest_prev.edge = 4;
        } 
        else if (prev->doors_x[chosen_door_prev] == prev->corner_x + prev->width - 1) {
            closest_prev.edge = 2;
        } 
        else if (prev->doors_y[chosen_door_prev] == prev->corner_y) {
            closest_prev.edge = 1;
        } 
        else {
            closest_prev.edge = 3;
        }

        if (curr->doors_x[chosen_door_curr] == curr->corner_x) {
            closest_curr.edge = 4;
        } 
        else if (curr->doors_x[chosen_door_curr] == curr->corner_x + curr->width - 1) {
            closest_curr.edge = 2;
        } 
        else if (curr->doors_y[chosen_door_curr] == curr->corner_y) {
            closest_curr.edge = 1;
        } 
        else {
            closest_curr.edge = 3;
        }
        
        draw_corridor(closest_prev, closest_curr, rooms, total_rooms);
    }
}


void draw_corridor(Door start, Door end, Room** rooms, int total_rooms) {
    int flag;
    switch (start.edge) {
    case 1:
        mvaddch(start.y - 1, start.x, '#');
        mvaddch(start.y - 2, start.x, '#');
        if (start.x >= end.x) {
            if (start.y < end.y) {
                for (int i = start.x; i >= start.parent->corner_x - 2; i--) {
                    mvaddch(start.y - 2, i, '#');
                }

                for (int i = start.y - 2; i <= start.parent->corner_y + start.parent->height + 2; i++) {
                    mvaddch(i, start.parent->corner_x - 2, '#');
                }
            }
            else {
                for (int i = start.x; i >= end.x; i--) {
                    mvaddch(start.y - 2, i, '#');
                }
            }
        }
        else {
            if (start.parent->corner_x + start.parent->width + 1 >= end.parent->corner_x && start.y < end.y) {
                for (int i = start.x; i >= start.parent->corner_x - 2; i--) {
                    mvaddch(start.y - 2, i, '#');
                }

                for (int i = start.y - 2; i <= end.parent->corner_y + end.parent->height + 2; i++) {
                    mvaddch(i, start.parent->corner_x - 2, '#');
                }

                for (int i = start.parent->corner_x - 2; i <= end.parent->corner_x - 2; i++) {
                    mvaddch(end.parent->corner_y + end.parent->height + 2, i, '#');
                }
            }
            else {
                flag = 0;
                for (int i = start.x; i < end.parent->corner_x - 1; i++) {
                    flag = 1;
                    mvaddch(start.y - 2, i, '#');
                }
                if (!flag) {
                    for (int i = start.x; i <= end.x; i++) {
                        mvaddch(start.y - 2, i, '#');
                    }
                }
            }
        }
        connect_doors(start, end);
        break;
    case 2:
        if (start.x >= end.x) {
            mvaddch(start.y, start.x + 1, '#');
            mvaddch(start.y, start.x + 2, '#');
            if (start.y > end.y) {
                for (int i = start.y; i >= start.parent->corner_y - 2; i--) {
                    mvaddch(i, start.x + 2, '#');
                }

                for (int i = start.x + 2; i >= end.x; i--) {
                    mvaddch(start.parent->corner_y - 2, i, '#');
                }
            }
            else {
                for (int i = start.y; i <= start.parent->corner_y + start.parent->height + 2; i++) {
                    mvaddch(i, start.x + 2, '#');
                }

                for (int i = start.x + 2; i >= end.x; i--) {
                    mvaddch(start.parent->corner_y + start.parent->height + 2, i, '#');
                }
            }
        }
        else {
            flag = 0;
            for (int i = start.x + 1; i < end.parent->corner_x - 1; i++) {
                flag = 1;
                mvaddch(start.y, i, '#');
            }
            if (!flag) {
                for (int i = start.x + 1; i <= end.x; i++) {
                    mvaddch(start.y, i, '#');
                }
            }
        }
        connect_doors(start, end);
        break;
    case 3:
        mvaddch(start.y + 1, start.x, '#');
        mvaddch(start.y + 2, start.x, '#');
        if (start.x >= end.x) {
            if (start.y > end.y) {
                for (int i = start.x; i >= start.parent->corner_x - 2; i--) {
                    mvaddch(start.y + 2, i, '#');
                }

                for (int i = start.y + 2; i >= start.parent->corner_y - 2; i--) {
                    mvaddch(i, start.parent->corner_x - 2, '#');
                }
            }
            else {
                for (int i = start.x; i >= end.x; i--) {
                    mvaddch(start.y + 2, i, '#');
                }
            }
        }
        else {
            if (start.parent->corner_x + start.parent->width + 1 >= end.parent->corner_x && start.y > end.y) {
                for (int i = start.x; i >= start.parent->corner_x - 2; i--) {
                    mvaddch(start.y + 2, i, '#');
                }

                for (int i = start.y + 2; i >= start.parent->corner_y - 2; i--) {
                    mvaddch(i, start.parent->corner_x - 2, '#');
                }

                for (int i = start.parent->corner_x - 2; i <= end.parent->corner_x - 2; i++) {
                    mvaddch(start.parent->corner_y - 2, i, '#');
                }
            }
            else {
                flag = 0;
                for (int i = start.x; i < end.parent->corner_x - 1; i++) {
                    flag = 1;
                    mvaddch(start.y + 2, i, '#');
                }
                if (!flag) {
                    for (int i = start.x; i <= end.x; i++) {
                        mvaddch(start.y + 2, i, '#');
                    }
                }
            }
        }
        connect_doors(start, end);
        break;
    case 4:
        mvaddch(start.y, start.x - 1, '#');
        mvaddch(start.y, start.x - 2, '#');
        if (start.x == end.x) {
            mvaddch(start.y, start.x - 1, ' ');
            mvaddch(start.y, start.x - 2, ' ');
        }
        else {
            if (start.y < end.y) {
                for (int i = start.y; i <= start.parent->corner_y + start.parent->height + 2; i++) {
                    mvaddch(i, start.x - 2, '#');
                }

                for (int i = start.x - 2; i < end.parent->corner_x - 1; i++) {
                    mvaddch(start.parent->corner_y + start.parent->height + 2, i, '#');
                }
            }
            else {
                for (int i = start.y; i >= start.parent->corner_y - 2; i--) {
                    mvaddch(i, start.x - 2, '#');
                }

                for (int i = start.x - 2; i < end.parent->corner_x - 1; i++) {
                    mvaddch(start.parent->corner_y - 2, i, '#');
                }
            }
        }
        connect_doors(start, end);
        break;
    default:
        break;
    }

    refresh();
}


int is_door_available(int y, int x) {
    if (mvinch(y - 1, x) == '#' || mvinch(y + 1, x) == '#' || mvinch(y, x - 1) == '#' || mvinch(y, x + 1) == '#') {
        return 0;
    }

    return 1;
}


void connect_doors(Door start, Door end) {
    int current_y, current_x;
    getyx(stdscr, current_y, current_x);
    switch (end.edge) {
    case 1:
        if (current_y == end.y) {
            if (mvinch(current_y - 1, current_x) != '#') {
                mvaddch(current_y - 1, current_x, '#');
                mvaddch(current_y - 2, current_x, '#'); 
            }
            else {
                mvaddch(current_y - 1, current_x, ' ');
                mvaddch(current_y - 2, current_x, ' ');
            }

            for (int i = current_x; i <= end.x; i++) {
                mvaddch(current_y - 2, i, '#');
            }

            mvaddch(end.y - 1, end.x, '#');
        } 
        else if (current_y < end.y) {
            for (int i = current_x; i <= end.x; i++) {
                mvaddch(current_y, i, '#');
            }

            for (int i = current_y + 1; i < end.y; i++) {
                mvaddch(i, end.x, '#');
            }
        }
        else {
            if (current_x >= end.parent->corner_x && current_x <= end.parent->corner_x + end.parent->width) {
                for (int i = current_y; i >= end.parent->corner_y + end.parent->height + 2; i--) {
                    mvaddch(i, current_x, '#');
                }

                for (int i = current_x; i >= end.parent->corner_x - 2; i--) {
                    mvaddch(end.parent->corner_y + end.parent->height + 2, i, '#');
                }

                for (int i = end.parent->corner_y + end.parent->height + 2; i >= end.y - 2; i--) {
                    mvaddch(i, end.parent->corner_x - 2, '#');
                }

                for (int i = end.parent->corner_x - 2; i <= end.x; i++) {
                    mvaddch(end.y - 2, i, '#');
                }

                mvaddch(end.y - 1, end.x, '#');
            }
            else {
                for (int i = current_y; i >= end.y - 2; i--) {
                    mvaddch(i, current_x, '#');
                }

                for (int i = current_x; i <= end.x; i++) {
                    mvaddch(end.y - 2, i, '#');
                }

                for (int i = end.y - 2; i < end.y; i++) {
                    mvaddch(i, end.x, '#');
                }
            }
        }
        break;
    case 2:
        if (current_y < end.parent->corner_y - 1) {
            for (int i = current_x; i <= end.x + 2; i++) {
                mvaddch(current_y, i, '#');
            }

            for (int i = current_y; i <= end.y; i++) {
                mvaddch(i, end.x + 2, '#');
            }

            for (int i = end.x + 2; i > end.x; i--) {
                mvaddch(end.y, i, '#');
            }
        }
        else if (current_y > end.parent->corner_y + end.parent->height + 1) {
            for (int i = current_x; i <= end.x + 2; i++) {
                mvaddch(current_y, i, '#');
            }

            for (int i = current_y; i >= end.y; i--) {
                mvaddch(i, end.x + 2, '#');
            }

            for (int i = end.x + 2; i > end.x; i--) {
                mvaddch(end.y, i, '#');
            }
        }
        else {
            if (current_y < end.parent->corner_y + end.parent->height / 2) {
                for (int i = current_y; i >= end.parent->corner_y - 2; i--) {
                    mvaddch(i, current_x, '#');
                }

                for (int i = current_x; i <= end.x + 2; i++) {
                    mvaddch(end.parent->corner_y - 2, i, '#');
                }

                for (int i = end.parent->corner_y - 2; i <= end.y; i++) {
                    mvaddch(i, end.x + 2, '#');
                }

                for (int i = end.x + 2; i > end.x; i--) {
                    mvaddch(end.y, i, '#');
                }
            }
            else {
                for (int i = current_y; i <= end.parent->corner_y + end.parent->height + 2; i++) {
                    mvaddch(i, current_x, '#');
                }

                for (int i = current_x; i <= end.x + 2; i++) {
                    mvaddch(end.parent->corner_y + end.parent->height + 2, i, '#');
                }

                for (int i = end.parent->corner_y + end.parent->height + 2; i >= end.y; i--) {
                    mvaddch(i, end.x + 2, '#');
                }

                for (int i = end.x + 2; i > end.x; i--) {
                    mvaddch(end.y, i, '#');
                }
            }
        }
        break;
    case 3:
        if (current_y == end.y) {
            if (mvinch(current_y + 1, current_x) != '#') {
                mvaddch(current_y + 1, current_x, '#');
                mvaddch(current_y + 2, current_x, '#');
            }
            else {
                mvaddch(current_y + 1, current_x, ' ');
                mvaddch(current_y + 2, current_x, ' ');
            }
             
            for (int i = current_x; i <= end.x; i++) {
                mvaddch(current_y + 2, i, '#');
            }
            mvaddch(end.y + 1, end.x, '#');
        } 
        else if (current_y > end.y) {
            for (int i = current_x; i <= end.x; i++) {
                mvaddch(current_y, i, '#');
            }

            for (int i = current_y - 1; i > end.y; i--) {
                mvaddch(i, end.x, '#');
            }
        }
        else {
            if (current_x >= end.parent->corner_x && current_x <= end.parent->corner_x + end.parent->width) {
                for (int i = current_y; i <= end.parent->corner_y - 2; i++) {
                    mvaddch(i, current_x, '#');
                }

                for (int i = current_x; i >= end.parent->corner_x - 2; i--) {
                    mvaddch(end.parent->corner_y - 2, i, '#');
                }

                for (int i = end.parent->corner_y - 2; i <= end.y + 2; i++) {
                    mvaddch(i, end.parent->corner_x - 2, '#');
                }

                for (int i = end.parent->corner_x - 2; i <= end.x; i++) {
                    mvaddch(end.y + 2, i, '#');
                }

                mvaddch(end.y + 1, end.x, '#');
            }
            else {
                for (int i = current_y; i <= end.y + 2; i++) {
                    mvaddch(i, current_x, '#');
                }

                for (int i = current_x; i <= end.x; i++) {
                    mvaddch(end.y + 2, i, '#');
                }

                for (int i = end.y + 2; i > end.y; i--) {
                    mvaddch(i, end.x, '#');
                }
            }
        }
        break;
    case 4:
        mvaddch(current_y, current_x - 1, '#');
        mvaddch(current_y, current_x - 2, '#');
        if (current_y > end.y) {
            for (int i = current_y; i >= end.y; i--) {
                mvaddch(i, current_x - 2, '#');
            }

            mvaddch(end.y, end.x - 1, '#');
            mvaddch(end.y, end.x - 2, '#');
            mvaddch(end.y, end.x - 3, '#');
        }
        else if (current_y < end.y) {
            for (int i = current_y; i <= end.y; i++) {
                mvaddch(i, current_x - 2, '#');
            }

            mvaddch(end.y, end.x - 1, '#');
            mvaddch(end.y, end.x - 2, '#');
            mvaddch(end.y, end.x - 3, '#');
        }
        break;
    default:
        break;
    }
}


