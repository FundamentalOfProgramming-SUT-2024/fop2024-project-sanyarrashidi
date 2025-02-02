#include "map.h"


Room** generate_map(int difficulty, int level) {
    int height, width;
    getmaxyx(stdscr, height, width);
    srand(time(NULL));
    init_color(10, 1000, 843, 0);
    init_color(11, 0, 0, 1000);
    init_color(12, 1000, 0, 0);
    init_color(13, 0, 1000, 0);
    init_color(14, 900, 500, 0);
    init_pair(1, 11, COLOR_BLACK);
    init_pair(2, 13, COLOR_BLACK);
    init_pair(3, 10, COLOR_BLACK);
    init_pair(4, 12, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, 14, COLOR_BLACK);
    int total_rooms = rand() % 2 + 6;
    Room** rooms = read_rooms(1);
    // rooms = generate_rooms(rooms, total_rooms, level, difficulty);
    display_rooms(rooms, total_rooms);
    clear(); // temp
    generate_corridors(rooms, total_rooms);

    return rooms;
}


Room** generate_rooms(Room** rooms, int total_rooms, int level, int difficulty) {
    int height, width;
    getmaxyx(stdscr, height, width);
    static bool sword_generated = false;
    int room_counter = 0;
    int to_enchant_room_index = rand() % (total_rooms - 3) + 1;
    while (room_counter < total_rooms) {
        Room* new_room = (Room*) malloc(sizeof(Room));
        new_room->total_rooms = total_rooms;
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

        new_room->trap_count = (new_room->height * new_room->width) / 30;
        new_room->traps = (Trap**) calloc(new_room->trap_count, sizeof(Trap*));
        for (int i = 0; i < new_room->trap_count; i++) {
            new_room->traps[i] = (Trap*) malloc(sizeof(Trap));
            new_room->traps[i]->x = new_room->corner_x + 2 + rand() % (new_room->width - 4);
            new_room->traps[i]->y = new_room->corner_y + 2 + rand() % (new_room->height - 4);
            new_room->traps[i]->found = false;
        }

        new_room->coin_count = (new_room->height * new_room->width) / 30;
        new_room->coins = (Coin**) calloc(new_room->coin_count, sizeof(Coin*));
        int coin_counter = 0;
        while (coin_counter < new_room->coin_count) {
            new_room->coins[coin_counter] = (Coin*) calloc(1, sizeof(Coin));
            
            bool valid_position = false;
            while (!valid_position) {
                new_room->coins[coin_counter]->x = new_room->corner_x + 1 + rand() % (new_room->width - 2);
                new_room->coins[coin_counter]->y = new_room->corner_y + 1 + rand() % (new_room->height - 2);
                valid_position = true;  
                for (int i = 0; i < new_room->trap_count; i++) {
                    if (new_room->coins[coin_counter]->x == new_room->traps[i]->x &&
                        new_room->coins[coin_counter]->y == new_room->traps[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
            }
            
            new_room->coins[coin_counter]->claimed = false;
            coin_counter++;
        }

        new_room->spell_count = rand() % 2;
        if (new_room->spell_count) {
            new_room->spells = (Spell**) calloc(new_room->spell_count, sizeof(Spell*));
            new_room->spells[0] = (Spell*) malloc(sizeof(Spell));
            int prob = rand() % 3;
            if (prob == 0) {
                new_room->spells[0]->type = 'H';
                new_room->spells[0]->symbol = L'\U0001F496';
            }
            else if (prob == 1) {
                new_room->spells[0]->type = 'S';
                new_room->spells[0]->symbol = L'\u26A1';
            }
            else {
                new_room->spells[0]->type = 'D';
                new_room->spells[0]->symbol = L'\u2620';
            }
            
            new_room->spells[0]->claimed = false;
            bool valid_position = false;
            while (!valid_position) {
                new_room->spells[0]->x = new_room->corner_x + 1 + rand() % (new_room->width - 2);
                new_room->spells[0]->y = new_room->corner_y + 1 + rand() % (new_room->height - 2);
                valid_position = true;  
                for (int i = 0; i < new_room->trap_count; i++) {
                    if (new_room->spells[0]->x == new_room->traps[i]->x && new_room->spells[0]->y == new_room->traps[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
                for (int i = 0; i < new_room->coin_count; i++) {
                    if (new_room->spells[0]->x == new_room->coins[i]->x && new_room->spells[0]->y == new_room->coins[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
            }
        }

        new_room->food_count = rand() % 3;
        if (new_room->food_count) {
            new_room->food = (Food**) calloc(new_room->food_count, sizeof(Food*));
            for (int i = 0; i < new_room->food_count; i++) {
                new_room->food[i] = (Food*) malloc(sizeof(Food));
                new_room->food[i]->claimed = false;
                int prob = rand() % 10 + 1;
                if (prob <= 6) {
                    new_room->food[i]->type = 'N';
                    new_room->food[i]->symbol = L'\U0001F36B';
                }
                else if (prob <= 9) {
                    new_room->food[i]->type = 'S';
                    new_room->food[i]->symbol = L'\U0001F35F';
                }
                else {
                    new_room->food[i]->type = 'M';
                    new_room->food[i]->symbol = L'\U0001F354';
                }

                new_room->food[i]->claimed = false;
                bool valid_position = false;
                while (!valid_position) {
                    new_room->food[i]->x = new_room->corner_x + 1 + rand() % (new_room->width - 2);
                    new_room->food[i]->y = new_room->corner_y + 1 + rand() % (new_room->height - 2);
                    valid_position = true;  
                    for (int j = 0; j < new_room->trap_count; j++) {
                        if (new_room->food[i]->x == new_room->traps[j]->x && new_room->food[i]->y == new_room->traps[j]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                    for (int j = 0; j < new_room->coin_count; j++) {
                        if (new_room->food[i]->x == new_room->coins[j]->x && new_room->food[i]->y == new_room->coins[j]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                    for (int j = 0; j < new_room->spell_count; j++) {
                        if (new_room->food[i]->x == new_room->spells[j]->x && new_room->food[i]->y == new_room->spells[j]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                }
            }
        }

        new_room->weapon_count = rand() % 2;
        if (new_room->weapon_count) {
            new_room->weapons = (Weapon**) calloc(1, sizeof(Weapon*));
            new_room->weapons[0] = (Weapon*) malloc(sizeof(Weapon));
            int prob = rand() % 10 + 1;
            if (sword_generated) {
                if (prob <= 5) {
                    new_room->weapons[0]->type = 'A';
                    new_room->weapons[0]->symbol = L'\U0001F3F9';
                    new_room->weapons[0]->damage = 5;
                }
                else if (prob <= 8) {
                    new_room->weapons[0]->type = 'D';
                    new_room->weapons[0]->symbol = L'\U0001F5E1';
                    new_room->weapons[0]->damage = 12;
                }
                else {
                    new_room->weapons[0]->type = 'W';
                    new_room->weapons[0]->symbol = L'\U0001FA84';
                    new_room->weapons[0]->damage = 15;
                }
            }
            else {
                if (prob <= 4) {
                    new_room->weapons[0]->type = 'A';
                    new_room->weapons[0]->symbol = L'\U0001F3F9';
                    new_room->weapons[0]->damage = 5;
                }
                else if (prob <= 7) {
                    new_room->weapons[0]->type = 'D';
                    new_room->weapons[0]->symbol = L'\U0001F5E1';
                    new_room->weapons[0]->damage = 12;
                }
                else if (prob <= 9) {
                    new_room->weapons[0]->type = 'S';
                    new_room->weapons[0]->symbol = L'\u2694';
                    new_room->weapons[0]->damage = 10;
                    sword_generated = true;
                }
                else {
                    new_room->weapons[0]->type = 'W';
                    new_room->weapons[0]->symbol = L'\U0001FA84';
                    new_room->weapons[0]->damage = 15;
                }
            }

            new_room->weapons[0]->claimed = false;
            bool valid_position = false;
            while (!valid_position) {
                new_room->weapons[0]->x = new_room->corner_x + 1 + rand() % (new_room->width - 2);
                new_room->weapons[0]->y = new_room->corner_y + 1 + rand() % (new_room->height - 2);
                valid_position = true;  
                for (int i = 0; i < new_room->trap_count; i++) {
                    if (new_room->weapons[0]->x == new_room->traps[i]->x && new_room->weapons[0]->y == new_room->traps[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
                for (int i = 0; i < new_room->coin_count; i++) {
                    if (new_room->weapons[0]->x == new_room->coins[i]->x && new_room->weapons[0]->y == new_room->coins[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
                for (int i = 0; i < new_room->spell_count; i++) {
                    if (new_room->weapons[0]->x == new_room->spells[i]->x && new_room->weapons[0]->y == new_room->spells[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
                for (int i = 0; i < new_room->food_count; i++) {
                    if (new_room->weapons[0]->x == new_room->food[i]->x && new_room->weapons[0]->y == new_room->food[i]->y) {
                        valid_position = false; 
                        break;
                    }
                }
            }

            valid_position = true;
            new_room->monster_count = rand() % 3 + (new_room->height * new_room->width) / 35;
            new_room->monsters = (Monster**) calloc(new_room->monster_count, sizeof(Monster*));
            for (int j = 0; j < new_room->monster_count; j++) {
                new_room->monsters[j] = (Monster*) calloc(1, sizeof(Monster));
                valid_position = false;
                while (!valid_position) {
                    new_room->monsters[j]->x = new_room->corner_x + 2 + rand() % (new_room->width - 4);
                    new_room->monsters[j]->y = new_room->corner_y + 2 + rand() % (new_room->height - 4);
                    valid_position = true;  
                    for (int i = 0; i < new_room->trap_count; i++) {
                        if (new_room->monsters[j]->x == new_room->traps[i]->x && new_room->monsters[j]->y == new_room->traps[i]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                    for (int i = 0; i < new_room->coin_count; i++) {
                        if (new_room->monsters[j]->x == new_room->coins[i]->x && new_room->monsters[j]->y == new_room->coins[i]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                    for (int i = 0; i < new_room->spell_count; i++) {
                        if (new_room->monsters[j]->x == new_room->spells[i]->x && new_room->monsters[j]->y == new_room->spells[i]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                    for (int i = 0; i < new_room->food_count; i++) {
                        if (new_room->monsters[j]->x == new_room->food[i]->x && new_room->monsters[j]->y == new_room->food[i]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                    for (int i = 0; i < new_room->weapon_count; i++) {
                        if (new_room->monsters[j]->x == new_room->weapons[i]->x && new_room->monsters[j]->y == new_room->weapons[i]->y) {
                            valid_position = false; 
                            break;
                        }
                    }
                }

                int prob = rand() % 10 + 1;
                switch (level) {
                case 1:
                    if (prob <= 5) {
                        new_room->monsters[j]->type = 'D';
                        new_room->monsters[j]->damage = 3 * difficulty;
                        new_room->monsters[j]->hp = 5;
                        new_room->monsters[j]->alive = true;
                    }
                    else {
                        new_room->monsters[j]->type = 'F';
                        new_room->monsters[j]->damage = 5 * difficulty;
                        new_room->monsters[j]->hp = 10;
                        new_room->monsters[j]->alive = true;
                    }
                    break;
                case 2:
                    if (prob <= 3) {
                        new_room->monsters[j]->type = 'D';
                        new_room->monsters[j]->damage = 3 * difficulty;
                        new_room->monsters[j]->hp = 5;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 7) {
                        new_room->monsters[j]->type = 'F';
                        new_room->monsters[j]->damage = 5 * difficulty;
                        new_room->monsters[j]->hp = 10;
                        new_room->monsters[j]->alive = true;
                    }
                    else {
                        new_room->monsters[j]->type = 'G';
                        new_room->monsters[j]->damage = 7 * difficulty;
                        new_room->monsters[j]->hp = 15;
                        new_room->monsters[j]->alive = true;
                    }
                    break;
                case 3:
                    if (prob <= 1) {
                        new_room->monsters[j]->type = 'D';
                        new_room->monsters[j]->damage = 3 * difficulty;
                        new_room->monsters[j]->hp = 5;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 2) {
                        new_room->monsters[j]->type = 'F';
                        new_room->monsters[j]->damage = 5 * difficulty;
                        new_room->monsters[j]->hp = 10;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 5) {
                        new_room->monsters[j]->type = 'G';
                        new_room->monsters[j]->damage = 7 * difficulty;
                        new_room->monsters[j]->hp = 15;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 9) {
                        new_room->monsters[j]->type = 'S';
                        new_room->monsters[j]->damage = 10 * difficulty;
                        new_room->monsters[j]->hp = 20;
                        new_room->monsters[j]->alive = true;
                    }
                    else {
                        new_room->monsters[j]->type = 'U';
                        new_room->monsters[j]->damage = 15 * difficulty;
                        new_room->monsters[j]->hp = 30;
                        new_room->monsters[j]->alive = true;
                    }
                    break;
                case 4:
                    if (prob <= 1) {
                        new_room->monsters[j]->type = 'D';
                        new_room->monsters[j]->damage = 3 * difficulty;
                        new_room->monsters[j]->hp = 5;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 2) {
                        new_room->monsters[j]->type = 'F';
                        new_room->monsters[j]->damage = 5 * difficulty;
                        new_room->monsters[j]->hp = 10;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 3) {
                        new_room->monsters[j]->type = 'G';
                        new_room->monsters[j]->damage = 7 * difficulty;
                        new_room->monsters[j]->hp = 10;
                        new_room->monsters[j]->alive = true;
                    }
                    else if (prob <= 5) {
                        new_room->monsters[j]->type = 'S';
                        new_room->monsters[j]->damage = 10 * difficulty;
                        new_room->monsters[j]->hp = 20;
                        new_room->monsters[j]->alive = true;
                    }
                    else {
                        new_room->monsters[j]->type = 'U';
                        new_room->monsters[j]->damage = 15 * difficulty;
                        new_room->monsters[j]->hp = 30;
                        new_room->monsters[j]->alive = true;
                    }
                    break;
                default:
                    break;
                }
            }
        }

        new_room->visited = false;
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
    rooms[0]->visited = true;
    if (level == 1) {
        rooms[0]->monster_count = 0;
        rooms[0]->monsters = NULL;
    }
    

    for (int i = 1; i < total_rooms - 1; i++) {
        if (i == to_enchant_room_index) {
            rooms[i]->type = 'R';
            rooms[i + 1]->type = 'E';
            rooms[i + 1]->weapon_count = 0;
            rooms[i + 1]->coin_count = 0;
            rooms[i + 1]->trap_count = 0;
            rooms[i + 1]->food_count = 0;
            rooms[i + 1]->monster_count = 0;
            rooms[i + 1]->weapons = NULL;
            rooms[i + 1]->coins = NULL;
            rooms[i + 1]->traps = NULL;
            rooms[i + 1]->food = NULL;
            rooms[i + 1]->monsters = NULL;
            rooms[i + 1]->spell_count = rooms[i + 1]->height * rooms[i + 1]->width / 12;
            rooms[i + 1]->spells = (Spell**) calloc(rooms[i + 1]->spell_count, sizeof(Spell*));
            for (int j = 0; j < rooms[i + 1]->spell_count; j++) {
                rooms[i + 1]->spells[j] = (Spell*) calloc(1, sizeof(Spell));
                rooms[i + 1]->spells[j]->x = rooms[i + 1]->corner_x + 1 + rand() % (rooms[i + 1]->width - 2);
                rooms[i + 1]->spells[j]->y = rooms[i + 1]->corner_y + 1 + rand() % (rooms[i + 1]->height - 2);
                rooms[i + 1]->spells[j]->claimed = false;
                int prob = rand() % 3;
                if (prob == 0) {
                    rooms[i + 1]->spells[j]->type = 'H';
                    rooms[i + 1]->spells[j]->symbol = L'\U0001F496';
                }
                else if (prob == 1) {
                    rooms[i + 1]->spells[j]->type = 'S';
                    rooms[i + 1]->spells[j]->symbol = L'\u26A1';
                }
                else {
                    rooms[i + 1]->spells[j]->type = 'D';
                    rooms[i + 1]->spells[j]->symbol = L'\u2620';
                }
            }
            rooms[i + 1]->door_count = 1;
            rooms[i + 1]->doors_x[1] = NULL;
            rooms[i + 1]->doors_y[1] = NULL;
            rooms[i + 1]->doors_x[2] = NULL;
            rooms[i + 1]->doors_y[2] = NULL;
            rooms[i + 1]->hidden_x = NULL;
            rooms[i + 1]->hidden_y = NULL;
            i++;
        }
        else {
            rooms[i]->type = 'R';
            rooms[i]->door_count = 2;
            rooms[i]->doors_x[2] = NULL;
            rooms[i]->doors_y[2] = NULL;
            rooms[i]->hidden_x = NULL;
            rooms[i]->hidden_y = NULL;
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
        rooms[i]->pillar_count = num_pillars;
        for (int j = 0; j < num_pillars; j++) {
            int pillar_x = rooms[i]->corner_x + 2 + rand() % (rooms[i]->width - 4);
            int pillar_y = rooms[i]->corner_y + 2 + rand() % (rooms[i]->height - 4);
            rooms[i]->pillars_x = (int*) calloc(num_pillars, sizeof(int));
            rooms[i]->pillars_y = (int*) calloc(num_pillars, sizeof(int));
            rooms[i]->pillars_x[j] = pillar_x;
            rooms[i]->pillars_y[j] = pillar_y;
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
                    bool flag = false;
                    for (int k = 0; k < rooms[i]->trap_count; k++) {
                        if (pillar_x == rooms[i]->traps[k]->x && pillar_y == rooms[i]->traps[k]->y) {
                            flag = true;
                        }
                    }
                    for (int k = 0; k < rooms[i]->coin_count; k++) {
                        if (pillar_x == rooms[i]->coins[k]->x && pillar_y == rooms[i]->coins[k]->y) {
                            flag = true;
                        }
                    }
                    for (int k = 0; k < rooms[i]->spell_count; k++) {
                        if (pillar_x == rooms[i]->spells[k]->x && pillar_y == rooms[i]->spells[k]->y) {
                            flag = true;
                        }
                    }
                    for (int k = 0; k < rooms[i]->food_count; k++) {
                        if (pillar_x == rooms[i]->food[k]->x && pillar_y == rooms[i]->food[k]->y) {
                            flag = true;
                        }
                    }
                    for (int k = 0; k < rooms[i]->weapon_count; k++) {
                        if (pillar_x == rooms[i]->weapons[k]->x && pillar_y == rooms[i]->weapons[k]->y) {
                            flag = true;
                        }
                    }
                    for (int k = 0; k < rooms[i]->monster_count; k++) {
                        if (pillar_x == rooms[i]->monsters[k]->x && pillar_y == rooms[i]->monsters[k]->y) {
                            flag = true;
                        }
                    }
                    if (flag) {
                        continue;
                    }
            }
            mvaddch(pillar_y, pillar_x, 'O');
        }
    }
}

void display_single_room(Room* Room) {
    move(Room->corner_y, Room->corner_x);
    Room->type == 'E' ? attron(A_UNDERLINE | COLOR_PAIR(5)) : attron (A_UNDERLINE | COLOR_PAIR(4));
    for (int j = 0; j < Room->width; j++) {
        printw("-");
    }
    Room->type == 'E' ? attroff(A_UNDERLINE | COLOR_PAIR(5)) : attroff(A_UNDERLINE | COLOR_PAIR(4));
    
    for (int j = 0; j < Room->height - 2; j++) {
        move(Room->corner_y + j + 1, Room->corner_x);
        Room->type == 'E' ? attron(COLOR_PAIR(6)) : attron(COLOR_PAIR(2));
        printw("|");
        Room->type == 'E' ? attroff(COLOR_PAIR(6)) : attroff(COLOR_PAIR(2));
        for (int k = 0; k < Room->width - 2; k++) {
            printw(".");
        }
        Room->type == 'E' ? attron(COLOR_PAIR(6)) : attron(COLOR_PAIR(2));
        printw("|");
        Room->type == 'E' ? attroff(COLOR_PAIR(6)) : attroff(COLOR_PAIR(2));
    }

    move(Room->corner_y + Room->height - 1, Room->corner_x);
    Room->type == 'E' ? attron(A_UNDERLINE | COLOR_PAIR(5)) : attron (A_UNDERLINE | COLOR_PAIR(4));
    for (int j = 0; j < Room->width; j++) {
        printw("-");
    }
    Room->type == 'E' ? attroff(A_UNDERLINE | COLOR_PAIR(5)) : attroff(A_UNDERLINE | COLOR_PAIR(4));

    for (int j = 0; j < Room->door_count; j++) {
        if (Room->door_count == 3 && Room->doors_x[j] == Room->hidden_x[0]) {
            continue;
        }
        move(Room->doors_y[j], Room->doors_x[j]);
        printw("+");
    }

    for (int j = 0; j < Room->pillar_count; j++) {
        attron(COLOR_PAIR(5));
        mvaddch(Room->pillars_y[j], Room->pillars_x[j], 'O');
        attroff(COLOR_PAIR(5));
    }

    for (int j = 0; j < Room->coin_count; j++) {
        if (Room->coins[j]->claimed) {
            continue;
        }
        attron(COLOR_PAIR(3));
        mvprintw(Room->coins[j]->y, Room->coins[j]->x, "\u25CC");
        attroff(COLOR_PAIR(3));
    }

    for (int j = 0; j < Room->spell_count; j++) {
        if (Room->spells[j]->claimed) {
            continue;
        }
        attron(COLOR_PAIR(6));
        mvprintw(Room->spells[j]->y, Room->spells[j]->x, "\U0001F70F");
        attroff(COLOR_PAIR(6));
    }

    for (int j = 0; j < Room->food_count; j++) {
        if (Room->food[j]->claimed) {
            continue;
        }
        attron(COLOR_PAIR(7));
        mvprintw(Room->food[j]->y, Room->food[j]->x, "@");
        attroff(COLOR_PAIR(7));
    }

    for (int j = 0; j < Room->weapon_count; j++) {
        if (Room->weapons[j]->claimed) {
            continue;
        }
        attron(COLOR_PAIR(5));
        mvprintw(Room->weapons[j]->y, Room->weapons[j]->x, "\u2692");
        attroff(COLOR_PAIR(5));
    }

    for (int j = 0; j < Room->monster_count; j++) {
        if (!Room->monsters[j]->alive) {
            continue;
        }
        attron(COLOR_PAIR(4));
        mvprintw(Room->monsters[j]->y, Room->monsters[j]->x, "%c", Room->monsters[j]->type);
        attroff(COLOR_PAIR(4));
    }
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

        if (closest_curr.parent->type == 'E') {
            closest_prev.parent->hidden_x = (int*) calloc(1, sizeof(int));
            closest_prev.parent->hidden_y = (int*) calloc(1, sizeof(int));
            closest_prev.parent->hidden_x[0] = closest_prev.x;
            closest_prev.parent->hidden_y[0] = closest_prev.y;
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

            for (int i = current_x - 2; i < end.x; i++) {
                mvaddch(end.y, i, '#');
            }            
        }
        else if (current_y < end.y) {
            for (int i = current_y; i <= end.y; i++) {
                mvaddch(i, current_x - 2, '#');
            }

            for (int i = current_x - 2; i < end.x; i++) {
                mvaddch(end.y, i, '#');
            }
        }
        else {
            mvaddch(end.y, end.x - 1, '#');
            mvaddch(end.y, end.x - 2, '#');
        }
        break;
    default:
        break;
    }
}


char** save_map() {
    int height, width;
    getmaxyx(stdscr, height, width);
    char** saved_map = (char**) calloc(height, sizeof(char*));
    for (int i = 0; i < height; i++) {
        saved_map[i] = (char*) calloc(width, sizeof(char));
        for (int j = 0; j < width; j++) {
            saved_map[i][j] = (char) mvinch(i, j);
        }
    }

    return saved_map;
}


Room* find_room_by_door(Room** rooms, int y, int x) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < rooms[i]->door_count; j++) {
            if (rooms[i]->doors_x[j] == x && rooms[i]->doors_y[j] == y) {
                return rooms[i];
            }
        }
    }
}