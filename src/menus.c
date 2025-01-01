#include "menus.h"


void handle_login() {
    initscr();
    int height, width;
    getmaxyx(stdscr, height, width);
    FILE* data_file = fopen("data/users.csv", "r");
    fseek(data_file, 0, SEEK_END);
    if (!ftell(data_file)) {
        mvprintw(height / 2, (width - 25) / 2, "You are the first player!");
        refresh();
        mvprintw(height / 2 + 1, (width - 36) / 2, "Press ANY KEY to create your profile.");
        refresh();
        mvprintw(height / 2 + 2, (width - 20) / 2, "Press ESC to leave.");
        refresh();
        char command = getch();
        if (command == 27)
            endwin();
        else {
            clear();
            new_user(height, width);
        }
    }
    else {

    }
    fclose(data_file);
    endwin();
}


void new_user(int height, int width) {
    Player new_player;
    new_player.username = (char*) calloc(50, sizeof(char));
    new_player.password = (char*) calloc(50, sizeof(char));
    new_player.email = (char*) calloc(50, sizeof(char));

    mvprintw(height / 2 - 10, (width - 20) / 2, "Enter your username:");
    move(height / 2 - 8, (width - 18) / 2);
    refresh();
    getstr(new_player.username);
    int pass_is_valid = 0;
    while (!pass_is_valid) {
        mvprintw(height / 2 - 4, (width - 20) / 2, "Enter your password:");
        move(height / 2 - 2, (width - 18) / 2);
        refresh();
        getstr(new_player.password);
        pass_is_valid = check_pass(new_player.password, height, width);
    }

    int email_is_valid = 0;
    while (!email_is_valid) {
        mvprintw(height / 2 + 2, (width - 20) / 2, "Enter your email:");
        move(height / 2 + 4, (width - 18) / 2);
        refresh();
        getstr(new_player.email);
        email_is_valid = check_email(new_player.email, height, width);
    }


}


void login() {

}


int check_pass(char* password, int height, int width) {
    clear_pass_errors();
    attron(A_BOLD);
    if (strlen(password) < 7) {
        mvprintw(7, (width - 50) / 2, "Your password should contain at least 7 characters!");
        refresh();
        move(height / 2 - 2, 0);
        clrtoeol();
        refresh();
        attroff(A_BOLD);
        return 0;
    }

    int upper_flag = 0;
    int lower_flag = 0;
    int digit_flag = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (*(password + i) >= 'A' && *(password + i) <= 'Z')
            upper_flag = 1;
        else if (*(password + i) >= 'a' && *(password + i) <= 'z')
            lower_flag = 1;
        else if (*(password + i) >= '0' && *(password + i) <= '9')
            digit_flag = 1;
        
        if (upper_flag && lower_flag && digit_flag) {
            attroff(A_BOLD);
            return 1;
        }
    }

    move(height / 2 + 4, (width - 18) / 2);
    clrtoeol();
    refresh();

    if (!upper_flag) {
        mvprintw(6, (width - 60) / 2, "Your password should contain at least 1 uppercase character!");
        refresh();
    }
    if (!lower_flag) {
        mvprintw(7, (width - 60) / 2, "Your password should contain at least 1 lowercase character!");
        refresh();
    }
    if (!digit_flag) {
        mvprintw(8, (width - 46) / 2, "Your password should contain at least 1 digit!");
        refresh();
    }

    attroff(A_BOLD);
    return 0;
}


void clear_pass_errors() {
    move(6, 0);
    clrtoeol();
    refresh();
    move(7, 0);
    clrtoeol();
    refresh();
    move(8, 0);
    clrtoeol();
    refresh();
}


int check_email(char* email, int height, int width) {
    move(7, 0);
    clrtoeol();
    refresh();
    int atsign_index = 0, dot_index = 0;
    for (int i = 0; i < strlen(email); i++) {
        if (*(email + i) == '@')
            atsign_index = i;
        else if (*(email + i) == '.')
            dot_index = i;
    }

    if (!atsign_index || !dot_index || dot_index + 1 <= atsign_index || dot_index == strlen(email) - 1) {
        attron(A_BOLD);
        mvprintw(7, (width - 24) / 2, "Email format is invalid!");
        attroff(A_BOLD);
        move(height / 2 + 4, 0);
        clrtoeol();
        refresh();
        return 0;
    }

    return 1;
}