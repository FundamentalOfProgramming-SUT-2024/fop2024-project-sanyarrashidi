#include "game.h"


void game_ui(Player* player) {
    int height, width;
    getmaxyx(stdscr, height, width);
    init_color(10, 1000, 843, 0);
    init_color(11, 0, 0, 1000);
    init_color(12, 1000, 0, 0);
    init_color(13, 0, 1000, 0);
    init_pair(1, 11, COLOR_BLACK);
    init_pair(2, 13, COLOR_BLACK);
    init_pair(3, 10, COLOR_BLACK);
    init_pair(4, 12, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);

    Room** rooms = generate_map();
    refresh();
    char** corridors = save_map();
    clear();
    display_single_room(rooms[0]);
    for (int i = 0; i < width; i++) {
        attron(A_UNDERLINE);
        mvaddch(4, i, '-');
        attroff(A_UNDERLINE);
    }

    char* new_gold = (char*) malloc(10 * sizeof(char));
    char* prev_gold = (char*) malloc(10 * sizeof(char));
    int claimed_gold = 0;
    sprintf(prev_gold, "%d", player->gold);
    mvprintw(1, 1, "Your name: %s", player->username);
    attron(COLOR_PAIR(3));
    mvprintw(2, 1, "Gold earned: 0");
    mvprintw(3, 1, "Total gold: %s", prev_gold); 
    attroff(COLOR_PAIR(3));

    Backpack* backpack = (Backpack*) malloc(sizeof(Backpack));
    backpack->count_weapons = 1;
    backpack->weapons = (Weapon**) calloc(5, sizeof(Weapon*));
    backpack->weapons[0] = (Weapon*) malloc(sizeof(Weapon));
    backpack->weapons[0]->type = 'M';
    backpack->weapons[0]->damage = 5;
    backpack->weapons[0]->symbol = '\u2692';
    backpack->default_weapon = backpack->weapons[0];
    backpack->spells = (Spell**) calloc(5, sizeof(Spell*));
    backpack->count_spells = 0;
    backpack->food = (Food**) calloc(5, sizeof(Food*));
    backpack->count_food = 0;

    char command;
    int current_y = rooms[0]->corner_y + 1;
    int current_x = rooms[0]->corner_x + 1;
    char prev_char = (char) mvinch(current_y, current_x);
    int hidden_door_x, hidden_door_y;
    bool found_hidden = false;
    for (int i = 0; i < 8; i++) {
        if (rooms[i]->door_count == 3) {
            hidden_door_x = rooms[i]->hidden_x[0];
            hidden_door_y = rooms[i]->hidden_y[0];
            break;
        }
    }

    Coin* found_coin = NULL;
    Spell* found_spell = NULL;
    Food* found_food = NULL;
    Weapon* found_weapon = NULL;
    bool trap_triggered = false;
    bool bottom_reached = false;
    bool top_reached = false;
    bool left_reached = false;
    bool right_reached = false;
    move_player(player, rooms[0]->corner_y + 1, rooms[0]->corner_x + 1);

    while ((command = getch()) != 'q') {
        char next_char;
        move(1, strlen(player->username) + 14);
        clrtoeol();
        move(current_y, current_x);

        top_reached = (current_y == 0);
        bottom_reached = (current_y == height - 1);
        left_reached = (current_x == 0);
        right_reached = (current_x == width - 1);

        switch (command) {
        case '8':
            if (top_reached) {
                break;
            }
            next_char = (char) mvinch(current_y - 1, current_x);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;                   
                current_y--;                             
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '6':
            if (right_reached) {
                break;
            }
            next_char = (char) mvinch(current_y, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_x++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '2':
            if (bottom_reached) {
                break;
            }
            next_char = (char) mvinch(current_y + 1, current_x);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '4':
            if (left_reached) {
                break;
            }
            next_char = (char) mvinch(current_y, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_x--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '7':
            if (top_reached || left_reached) {
                break;
            }
            next_char = (char) mvinch(current_y - 1, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y--;
                current_x--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '9':
            if (top_reached || right_reached) {
                break;
            }
            next_char = (char) mvinch(current_y - 1, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y--;
                current_x++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '3':
            if (bottom_reached || right_reached) {
                break;
            }
            next_char = (char) mvinch(current_y + 1, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y++;
                current_x++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '1':
            if (bottom_reached || left_reached) {
                break;
            }
            next_char = (char) mvinch(current_y + 1, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(4));
                    mvprintw(current_y, current_x, "F");
                    attroff(COLOR_PAIR(4));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y++;
                current_x--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
        case 'c':
            if (found_coin != NULL) {
                found_coin->claimed = true;
                mvprintw(1, width / 2 - 6, "Coin claimed!");
                char* new_gold = (char*) malloc(10 * sizeof(char));
                char* prev_gold = (char*) malloc(10 * sizeof(char));
                claimed_gold += 10;
                player->gold += 10;
                prev_char = '.';
                if (!strcmp(player->difficulty, "easy")) {
                    player->score += 10;
                }
                else if (!strcmp(player->difficulty, "medium")) {
                    player->score += 20;
                }
                else {
                    player->score += 30;
                }
                new_gold[0] = '\0';
                prev_gold[0] = '\0';
                move(2, 0);
                clrtoeol();
                move(3, 0);
                clrtoeol();
                move(current_y, current_x);
                sprintf(new_gold, "%d", claimed_gold);
                sprintf(prev_gold, "%d", player->gold);
                attron(COLOR_PAIR(3));
                mvprintw(2, 1, "Gold earned: %s", new_gold);
                mvprintw(3, 1, "Total gold: %s", prev_gold); 
                attroff(COLOR_PAIR(3));
            }

            if (found_spell != NULL) {
                found_spell->claimed = true;
                mvprintw(1, width / 2 - 6, "Spell claimed!");
                prev_char = '.';
                if (backpack->count_spells == 0) {
                    backpack->default_spell = found_spell;
                }

                bool new_spell = true;
                for (int i = 0; i < backpack->count_spells; i++) {
                    if (backpack->spells[i]->type == found_spell->type) {
                        backpack->spells[i]->amount++;
                        new_spell = false;
                    }
                }

                if (new_spell) {
                    backpack->spells[backpack->count_spells] = found_spell;
                    backpack->spells[backpack->count_spells]->amount = 1;
                    backpack->count_spells++;
                }
            }

            if (found_food != NULL) {
                found_food->claimed = true;
                mvprintw(1, width / 2 - 6, "Food claimed!");
                prev_char = '.';
                if (backpack->count_food == 0) {
                    backpack->default_food = found_food;
                }

                bool new_food = true;
                for (int i = 0; i < backpack->count_food; i++) {
                    if (backpack->food[i]->type == found_food->type) {
                        backpack->food[i]->amount++;
                        new_food = false;
                    }
                }

                if (new_food) {
                    backpack->food[backpack->count_food] = found_food;
                    backpack->food[backpack->count_food]->amount = 1;
                    backpack->count_food++;
                }
            }

            if (found_weapon != NULL) {
                found_weapon->claimed = true;
                mvprintw(1, width / 2 - 6, "Weapon claimed!");
                prev_char = '.';
                
                bool new_weapon = true;
                for (int i = 0; i < backpack->count_weapons; i++) {
                    if (backpack->weapons[i]->type == found_weapon->type) {
                        if (found_weapon->type == 'D') {
                            backpack->weapons[i]->ammo += 10;
                            new_weapon = false;
                        }
                        else if (found_weapon->type == 'W') {
                            backpack->weapons[i]->ammo += 8;
                            new_weapon = false;
                        }
                        else if (found_weapon->type == 'A') {
                            backpack->weapons[i]->ammo += 20;
                            new_weapon = false;
                        }
                    }
                }

                if (new_weapon) {
                    backpack->weapons[backpack->count_weapons] = found_weapon;
                    if (found_weapon->type == 'D') {
                        backpack->weapons[backpack->count_weapons]->ammo = 10;
                    }
                    else if (found_weapon->type == 'W') {
                        backpack->weapons[backpack->count_weapons]->ammo = 8;
                    }
                    else if (found_weapon->type == 'A') {
                        backpack->weapons[backpack->count_weapons]->ammo = 20;
                    }
                    backpack->count_weapons++;
                }
            }
            break;
        case 'f':
            player->fast_paced = !player->fast_paced;
            if (player->fast_paced) {
                mvprintw(1, strlen(player->username) + 12, "\u26A1");
            }
            else {
                mvprintw(1, strlen(player->username) + 12, " ");
            }
        default:
            break;
        }

        if (prev_char == '+') {
            Room* visited_room = find_room_by_door(rooms, current_y, current_x);
            if (!visited_room->visited) {
                visited_room->visited = true;
                display_single_room(visited_room);
                if (visited_room->type == 'E') {
                    mvprintw(1, width / 2 - 18, "You have discovered the enchant room!");
                }
                else {
                    mvprintw(1, width / 2 - 15, "You have discovered a new room!");
                }
                move_player(player, current_y, current_x);
            }
        }

        if (!found_hidden && found_hidden_door(current_y, current_x, hidden_door_y, hidden_door_x)) {
            found_hidden = true;
            mvaddch(hidden_door_y, hidden_door_x, '$');
            mvprintw(1, width / 2 - 15, "You have found a hidden door!");
        }

        move(current_y, current_x);

        if (prev_char == '+' || prev_char == '#' || prev_char == '$') {
            if (!right_reached && (corridors[current_y][current_x + 1] == '#' || corridors[current_y][current_x + 1] == '+')) {
                mvaddch(current_y, current_x + 1, corridors[current_y][current_x + 1]);
            }
            if (!left_reached && (corridors[current_y][current_x - 1] == '#' || corridors[current_y][current_x - 1] == '+')) {
                mvaddch(current_y, current_x - 1, corridors[current_y][current_x - 1]);
            }
            if (!top_reached && (corridors[current_y - 1][current_x] == '#' || corridors[current_y - 1][current_x] == '+')) {
                mvaddch(current_y - 1, current_x, corridors[current_y - 1][current_x]);
            }
            if (!bottom_reached && (corridors[current_y + 1][current_x] == '#' || corridors[current_y + 1][current_x] == '+')) {
                mvaddch(current_y + 1, current_x, corridors[current_y + 1][current_x]);
            }
            if (!left_reached && !top_reached && (corridors[current_y - 1][current_x - 1] == '#' || corridors[current_y - 1][current_x - 1] == '+')) {
                mvaddch(current_y - 1, current_x - 1, corridors[current_y - 1][current_x - 1]);
            }
            if (!bottom_reached && !left_reached && (corridors[current_y + 1][current_x - 1] == '#' || corridors[current_y + 1][current_x - 1] == '+')) {
                mvaddch(current_y + 1, current_x - 1, corridors[current_y + 1][current_x - 1]);
            }
            if (!top_reached && !right_reached && (corridors[current_y - 1][current_x + 1] == '#' || corridors[current_y - 1][current_x + 1] == '+')) {
                mvaddch(current_y - 1, current_x + 1, corridors[current_y - 1][current_x + 1]);
            }
            if (!bottom_reached && !right_reached && (corridors[current_y + 1][current_x + 1] == '#' || corridors[current_y + 1][current_x + 1] == '+')) {
                mvaddch(current_y + 1, current_x + 1, corridors[current_y + 1][current_x + 1]);
            }
        }

        trap_triggered = stepped_on_trap(rooms, current_y, current_x, width);
        found_coin = stepped_on_loot(rooms, current_y, current_x, width);
        found_spell = stepped_on_spell(rooms, current_y, current_x, width);
        found_food = stepped_on_food(rooms, current_y, current_x, width);
        found_weapon = stepped_on_weapon(rooms, current_y, current_x, width);

        refresh();
        move(current_y, current_x);
    }
}

void move_player(Player* player, int y, int x) {
    attron(A_BOLD);
    if (!strcmp(player->color, "blue")) {
        attron(COLOR_PAIR(1));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(1));
    }
    else if (!strcmp(player->color, "yellow")) {
        attron(COLOR_PAIR(3));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(3));
    }
    else if (!strcmp(player->color, "green")) {
        attron(COLOR_PAIR(2));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(2));
    }
    else if (!strcmp(player->color, "white")) {
        mvprintw(y, x, player->hero);
    }
    else if (!strcmp(player->color, "red")) {
        attron(COLOR_PAIR(4));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(4));
    }
    attroff(A_BOLD);
}


bool found_hidden_door(int y, int x, int door_y, int door_x) {
    if (door_y == y - 1 && door_x == x) {
        return true;
    }
    else if (door_y == y + 1 && door_x == x) {
        return true;
    }
    else if (door_y == y && door_x == x - 1) {
        return true;
    }
    else if (door_y == y && door_x == x + 1) {
        return true;
    }

    return false;
}


bool stepped_on_trap(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->trap_count; j++) {
            if (rooms[i]->traps[j]->y == y && rooms[i]->traps[j]->x == x) {
                if (!rooms[i]->traps[j]->found) {
                    mvprintw(1, width / 2 - 15, "You have stepped on a trap!");
                }
                rooms[i]->traps[j]->found = true;
                return true;
            }
        }
    }

    return false;
}


Coin* stepped_on_loot(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->coin_count; j++) {
            if (!rooms[i]->coins[j]->claimed && rooms[i]->coins[j]->y == y && rooms[i]->coins[j]->x == x) {
                mvprintw(1, width / 2 - 15, "Loot found! Press C to claim it.");
                return rooms[i]->coins[j];
            }
        }
    }

    return NULL;
}


Spell* stepped_on_spell(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->spell_count; j++) {
            if (!rooms[i]->spells[j]->claimed && rooms[i]->spells[j]->y == y && rooms[i]->spells[j]->x == x) {
                char* type = (char*) malloc(10 * sizeof(char));
                if (rooms[i]->spells[j]->type == 'H') {
                    type = "Health";
                }
                else if (rooms[i]->spells[j]->type == 'S') {
                    type = "Speed";
                }
                else {
                    type = "Damage";
                }
                mvprintw(1, width / 2 - 18, "%s spell found! Press C to claim it.", type);
                return rooms[i]->spells[j];
            }
        }
    }

    return NULL;
}


Food* stepped_on_food(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->food_count; j++) {
            if (!rooms[i]->food[j]->claimed && rooms[i]->food[j]->y == y && rooms[i]->food[j]->x == x) {
                char* type = (char*) malloc(10 * sizeof(char));
                if (rooms[i]->food[j]->type == 'N') {
                    type = "Normal";
                }
                else if (rooms[i]->food[j]->type == 'S') {
                    type = "Special";
                }
                else {
                    type = "Magic";
                }
                mvprintw(1, width / 2 - 15, "%s food found! Press C to claim it.", type);
                return rooms[i]->food[j];
            }
        }
    }

    return NULL;
}


Weapon* stepped_on_weapon(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->weapon_count; j++) {
            if (!rooms[i]->weapons[j]->claimed && rooms[i]->weapons[j]->y == y && rooms[i]->weapons[j]->x == x) {
                char* type = (char*) malloc(10 * sizeof(char));
                if (rooms[i]->weapons[j]->type == 'D') {
                    type = "Dagger";
                }
                else if (rooms[i]->weapons[j]->type == 'S') {
                    type = "Sword";
                }
                else if (rooms[i]->weapons[j]->type == 'W') {
                    type = "Magic wand";
                }
                else {
                    type = "Arrow";
                }
                mvprintw(1, (width + strlen(type)) / 2 - 15, "%s weapon found! Press C to claim it.", type);
                return rooms[i]->weapons[j];
            }
        }
    }

    return NULL;
}