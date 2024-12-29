#include "menus.h"


typedef struct {
    char* message;
    int x;
    int y;
} Button;


void handle_login() {
    initscr();
    int height, width;
    getmaxyx(stdscr, height, width);
    FILE* data_file = fopen("data/users.txt", "r");
    fseek(data_file, 0, SEEK_END);
    if (!ftell(data_file)) {
        mvprintw(height / 2, (width - 25) / 2, "You are the first player!");
        refresh();
        mvprintw(height / 2 + 1, (width - 36) / 2, "Press ANY KEY to create your profile.");
        refresh();
        getchar();
        clear();
        new_user();
    }
    fclose(data_file);
    endwin();
}


void new_user() {
    printw("NEW USER!");
    refresh();
    getchar();
}


void login() {

}