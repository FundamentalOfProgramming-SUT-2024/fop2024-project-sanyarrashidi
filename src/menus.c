#include "menus.h"


void handle_login() {
    initscr();
    int height, width;
    getmaxyx(stdscr, height, width);
    keypad(stdscr, TRUE);
    FILE* data_file = fopen("data/players.csv", "r");
    fseek(data_file, 0, SEEK_END);
    if (!ftell(data_file)) {
        mvprintw(height / 2, (width - 25) / 2, "You are the first player!");
        mvprintw(height / 2 + 1, (width - 36) / 2, "Press ANY KEY to create your profile.");
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
        mvprintw(height / 2 - 8, (width - 8) / 2, "Welcome!");
        mvprintw(height / 2 - 6, (width - 32) / 2, "Press L to log into your account.");
        mvprintw(height / 2 - 4, (width - 32) / 2, "Press N to create a new account.");
        mvprintw(height / 2 - 2, (width - 26) / 2, "Press G to play as guest.");
        mvprintw(height / 2, (width - 20) / 2, "Press ESC to leave.");
        refresh();
        char command = getch();
        clear();
        rewind(data_file);
        if (tolower(command) == 'n')
            new_user(height, width);
        else if (tolower(command) == 'l')
            login(data_file, height, width);
        else if (tolower(command) == 'g') {}
            //start function
        else if (command == 27)
            endwin();
        else    
            endwin();
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

    FILE* data_file = fopen("data/players.csv", "a");
    fprintf(data_file, "%s,%s,%s\n", new_player.username, new_player.password, new_player.email);
    fclose(data_file);
}


void login(FILE* data_file, int height, int width) {
    attron(A_UNDERLINE);
    mvprintw(10, (width - 60) / 2, "Enter the number of player you want to log into:");
    refresh();
    attroff(A_UNDERLINE);
    Player* players = (Player*) calloc(100, sizeof(Player));
    char* read_player = (char*) calloc(1000, sizeof(char));
    int player_counter = 0;
    while(fgets(read_player, 1000, data_file)) {
        players[player_counter].username = (char*) calloc(1000, sizeof(char));
        players[player_counter].password = (char*) calloc(1000, sizeof(char));
        char* player_inf = strtok(read_player, ",");
        players[player_counter].username = strdup(player_inf);
        mvprintw(14 + player_counter, (width - 48) / 2, "%d. %s\n", player_counter + 1, players[player_counter].username);
        refresh();
        player_inf = strtok(NULL, ",");
        players[player_counter].password = strdup(player_inf);
        player_counter++;
    }
    
    noecho();
    curs_set(0);
    int player_num = getch() - 48;
    while(player_num - 1 >= player_counter) {
        attron(A_BOLD);
        mvprintw(7, (width - 14) / 2, "Invalid digit!");
        refresh();
        attroff(A_BOLD);
        player_num = getch();
    }

    curs_set(1);
    echo();
    clear();
    Player selected_player = *(players + player_num - 1);
    char* entered_pass = (char*) calloc(100, sizeof(char));
    int attempts = 3;
    while (attempts > 0) {
        if (attempts < 3) {
            attron(A_BOLD);
            mvprintw(7, (width - 20) / 2, "Invalid password!");
            refresh();
            attroff(A_BOLD);
        }
        move(height / 2 - 8, 0);
        clrtoeol();
        mvprintw(height / 2 - 10, (width - (40 + strlen(selected_player.username))) / 2, "Enter password for \"%s\":(%d attempts left)", selected_player.username, attempts);
        refresh();
        move(height / 2 - 8, (width - 18) / 2);
        getstr(entered_pass);
        if (!strcmp(entered_pass ,selected_player.password))
            break;
        else
            attempts--;
    }

    if (attempts) {
        // user entered
    }
    else {
        clear();
        mvprintw(height / 2 - 10, (width - 28) / 2, "Too many attempts!");
        refresh();
        getch();
        endwin();
    }
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

    move(height / 2 - 2, (width - 18) / 2);
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