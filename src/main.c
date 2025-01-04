#include<stdio.h>
#include<ncurses.h>
#include"menus.h"
#include"map.h"


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
        generate_map();
    echo();
    curs_set(1);
    endwin();
    return 0;
}