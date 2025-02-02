#include<stdio.h>
#include<ncurses.h>
#include<locale.h>
#include<wchar.h>
#include "game.h"


int main() {
    setlocale(LC_ALL, "");
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
        player->username = "Guest";
        game_ui(player, true);
        return 0;
    }
    get_player_stat(player);
    noecho();
    curs_set(0);
    char result = main_menu(player);
    if (result == 'n')
        game_ui(player, true);
    else if (result == 'l')
        game_ui(player, false);
    echo();
    curs_set(1);
    endwin();
    return 0;
}