#include<stdio.h>
#include<ncurses.h>
#include "game.h"


int main() {
    initscr();
    keypad(stdscr, TRUE);
    start_color();
    Player* player = (Player*) malloc(sizeof(Player));
    player = authenticate(player);
    if (player == NULL) {
        endwin();
        return 0;
    }
    else if (!strcmp(player->username, ".guest")) {
        // just new game
        return 0;
    }
    get_player_stat(player);
    noecho();
    curs_set(0);
    char result = main_menu(player);
    if (result == 'n')
        game_ui(player);
    echo();
    curs_set(1);
    endwin();
    return 0;
}