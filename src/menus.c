#include "menus.h"


char main_menu(Player* player) {
    clear();
    show_main_menu(player);
    char command;
    while((command = getch())) {
        switch (tolower(command)) {
        case 'n':
            return 'n';
            break;
        case 'l':
            return 'l';
            break;
        case 's':
            // code
            break;
        case 'p':
            show_profile(player);
            break;
        case 'a':
            // code
            break;
        case 27:
            return NULL;
            break;
        default:
            break;
        }
        clear();
        show_main_menu(player);
    }
}


void show_main_menu(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvprintw(height / 2 - 10, (width - (8 + strlen(player->username))) / 2, "Welcome %s", player->username);
    refresh();
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2 - 6, (width - 19) / 2, "Press N for NEW GAME.");
    mvprintw(height / 2 - 4, (width - 20) / 2, "Press L for LOAD GAME.");
    mvprintw(height / 2 - 2, (width - 28) / 2, "Press S to see the SCOREBOARD.");
    mvprintw(height / 2, (width - 26) / 2, "Press P to see your PROFILE.");
    mvprintw(height / 2 + 2, (width - 26) / 2, "Press A to go to SETTINGS.");
    mvprintw(height / 2 + 4, (width - 20) / 2, "Press ESC to leave.");
    refresh();
    attroff(COLOR_PAIR(2));
}


void show_profile(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    mvprintw(10, width / 4 - 2, "Press ANY KEY to go back to main menu");
    refresh();
    attron(COLOR_PAIR(1));
    mvprintw(height / 2 - 8, width / 4, "Username: %s", player->username);
    mvprintw(height / 2 - 6, width / 4, "Password: %s", player->password);
    mvprintw(height / 2 - 4, width / 4, "Email: %s", player->email);
    refresh();
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    mvprintw(height / 2 - 2, width / 4, "Points scored: %d", player->score);
    refresh();
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2, width / 4, "Gold achived: %d", player->gold);
    refresh();
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(5));
    mvprintw(height / 2 + 2, width / 4, "Finished games: %d", player->finished);
    refresh();
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(4));
    mvprintw(height / 2 + 4, width / 4, "Time played: %d", player->exp);
    refresh();
    attroff(COLOR_PAIR(4));
    getch();
}