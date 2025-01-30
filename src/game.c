#include "game.h"


void game_ui(Player* player) {
    int height, width;
    getmaxyx(stdscr, height, width);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
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
    char* prev_gold = (char*) malloc(10 * sizeof(char));
    sprintf(prev_gold, "%d", player->gold);
    mvprintw(1, 1, "Your name: %s", player->username);
    mvprintw(2, 1, "Gold earned: %s", prev_gold);
    mvprintw(3, 1, "Total gold: %s", prev_gold); // should be updated in the loop
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
                mvaddch(current_y, current_x, prev_char); 
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
                mvaddch(current_y, current_x, prev_char);
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
                mvaddch(current_y, current_x, prev_char);
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
                mvaddch(current_y, current_x, prev_char);
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
                mvaddch(current_y, current_x, prev_char);
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
                mvaddch(current_y, current_x, prev_char);
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
                mvaddch(current_y, current_x, prev_char);
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
                mvaddch(current_y, current_x, prev_char);
                prev_char = next_char;
                current_y++;
                current_x--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(1, width / 2 - 15, "You cannot move there mate!");
            }
            break;
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