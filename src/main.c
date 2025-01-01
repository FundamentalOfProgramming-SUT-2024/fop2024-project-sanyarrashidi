#include<stdio.h>
#include<ncurses.h>
#include"authenticate.h"


int main() {
    initscr();
    Player* a = (Player*) calloc(1, sizeof(Player));
    a = authenticate(a);
    endwin();
    return 0;
}