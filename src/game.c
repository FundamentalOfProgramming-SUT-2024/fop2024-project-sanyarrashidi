#include "game.h"


void game_ui(Player* player) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    Room** rooms = generate_map();
    char** map = save_map();
    refresh(); // temporary for testing
    move_player(player, rooms[0]->corner_y + 1, rooms[0]->corner_x + 1);
    char prev_char = '.';
    char command;
    while ((command = getch()) != 'q') {
        int current_y, current_x;
        getyx(stdscr, current_y, current_x);
        current_x--;
        char next_char;
        switch (command) {
        case '8':
            next_char = (char) mvinch(current_y - 1, current_x);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                mvaddch(current_y, current_x, prev_char);
                prev_char = next_char;
                move_player(player, current_y - 1, current_x);
            }
            break;
        case '6':
            next_char = (char) mvinch(current_y, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                mvaddch(current_y, current_x, prev_char);
                prev_char = next_char;
                move_player(player, current_y, current_x + 1);
            }
            break;
        case '2':
            next_char = (char) mvinch(current_y + 1, current_x);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                mvaddch(current_y, current_x, prev_char);
                prev_char = next_char;
                move_player(player, current_y + 1, current_x);
            }
            break;
        case '4':
            next_char = (char) mvinch(current_y, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|') {
                mvaddch(current_y, current_x, prev_char);
                prev_char = next_char;
                move_player(player, current_y, current_x - 1);
            }
            break;
        }
    }
}

void move_player(Player* player, int y, int x) {
    attron(A_BOLD);
    if (!strcmp(player->color, "blue")) {
        attron(COLOR_PAIR(1));
        mvaddch(y, x, '@');
        attroff(COLOR_PAIR(1));
    }
    else if (!strcmp(player->color, "yellow")) {
        attron(COLOR_PAIR(3));
        mvaddch(y, x, '@');
        attroff(COLOR_PAIR(3));
    }
    else if (!strcmp(player->color, "green")) {
        attron(COLOR_PAIR(2));
        mvaddch(y, x, '@');
        attroff(COLOR_PAIR(2));
    }
    else if (!strcmp(player->color, "white")) {
        mvaddch(y, x, '@');
    }
    else if (!strcmp(player->color, "red")) {
        attron(COLOR_PAIR(4));
        mvaddch(y, x, '@');
        attroff(COLOR_PAIR(4));
    }
    attroff(A_BOLD);
}