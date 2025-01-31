#include "authenticate.h"


Player* authenticate(Player* player) {
    int height, width;
    getmaxyx(stdscr, height, width);
    noecho();
    curs_set(0);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    FILE* data_file = fopen("data/players.csv", "r");
    fseek(data_file, 0, SEEK_END);
    if (!ftell(data_file)) {
        attron(COLOR_PAIR(3));
        mvprintw(height / 2, (width - 25) / 2, "You are the first player!");
        refresh();
        attroff(COLOR_PAIR(3));
        mvprintw(height / 2 + 1, (width - 36) / 2, "Press ANY KEY to create your profile.");
        mvprintw(height / 2 + 2, (width - 20) / 2, "Press ESC to leave.");
        refresh();
        char command = getch();
        if (command == 27)
            return NULL;
        else {
            clear();
            echo();
            curs_set(1);
            new_user(player, height, width);
        }
    }
    else {
        attron(COLOR_PAIR(1));
        mvprintw(height / 2 - 8, (width - 8) / 2, "Welcome!");
        refresh();
        attroff(COLOR_PAIR(1));
        mvprintw(height / 2 - 6, (width - 32) / 2, "Press L to log into your account.");
        mvprintw(height / 2 - 4, (width - 32) / 2, "Press N to create a new account.");
        mvprintw(height / 2 - 2, (width - 26) / 2, "Press G to play as guest.");
        mvprintw(height / 2, (width - 20) / 2, "Press ESC to leave.");
        refresh();
        char command;
        rewind(data_file);
        while((command = getch())) {
            if (tolower(command) == 'n') {
                clear();
                echo();
                curs_set(1);
                return new_user(player, height, width);
            }
            else if (tolower(command) == 'l') {
                clear();
                echo();
                curs_set(1);
                return login(data_file, player, height, width);
            }
            else if (tolower(command) == 'g') {
                player->username = ".guest";
                return player;
            }
            // ESC key entered
            else if (command == 27)
                return NULL;
        }
    }
    fclose(data_file);
    return NULL;
}


Player* new_user(Player* new_player, int height, int width) {
    new_player->username = (char*) calloc(50, sizeof(char));
    new_player->password = (char*) calloc(50, sizeof(char));
    new_player->email = (char*) calloc(50, sizeof(char));

    int username_is_valid = 0;
    while (!username_is_valid) {
        mvprintw(height / 2 - 10, (width - 20) / 2, "Enter your username:");
        move(height / 2 - 8, (width - 18) / 2);
        refresh();
        getstr(new_player->username);
        username_is_valid = check_username(new_player->username, height, width);
    }

    int pass_is_valid = 0;
    while (!pass_is_valid) {
        mvprintw(height / 2 - 4, (width - 20) / 2, "Enter your password:");
        move(height / 2 - 2, (width - 18) / 2);
        refresh();
        getstr(new_player->password);
        pass_is_valid = check_pass(new_player->password, height, width);
    }

    int email_is_valid = 0;
    while (!email_is_valid) {
        mvprintw(height / 2 + 2, (width - 20) / 2, "Enter your email:");
        move(height / 2 + 4, (width - 18) / 2);
        refresh();
        getstr(new_player->email);
        email_is_valid = check_email(new_player->email, height, width);
    }

    FILE* data_file = fopen("data/players.csv", "a");
    fprintf(data_file, "%s,%s,%s\n", new_player->username, new_player->password, new_player->email);
    fclose(data_file);
    FILE* stat_file = fopen("data/stats.csv", "a");
    fprintf(stat_file, "%s,0,0,0,0,\u265C,white,medium,0,\n", new_player->username);
    fclose(stat_file);
    return new_player;
}


Player* login(FILE* data_file, Player* selected_player, int height, int width) {
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
        player_inf = strtok(NULL, ",");
        players[player_counter].email = strdup(player_inf);
        player_counter++;
    }
    
    noecho();
    curs_set(0);
    // converts ascii to digits
    int player_num = getch() - 48;
    while(player_num - 1 >= player_counter) {
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(5, (width - 14) / 2, "Invalid digit!");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        player_num = getch() - 48;
    }

    curs_set(1);
    echo();
    clear();
    selected_player = players + player_num - 1;
    char* entered_pass = (char*) calloc(100, sizeof(char));
    int attempts = 3;
    while (attempts > 0) {
        if (attempts < 3) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(5, (width - 20) / 2, "Invalid password!");
            refresh();
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
        move(height / 2 - 8, 0);
        clrtoeol();
        mvprintw(height / 2 - 10, (width - (40 + strlen(selected_player->username))) / 2, "Enter password for \"%s\":(%d attempts left)", selected_player->username, attempts);
        refresh();
        move(height / 2 - 8, (width - 18) / 2);
        getstr(entered_pass);
        if (!strcmp(entered_pass ,selected_player->password))
            break;
        else
            attempts--;
    }

    if (attempts) {
        return selected_player;
    }
    else {
        clear();
        attron(COLOR_PAIR(2));
        mvprintw(height / 2 - 10, (width - 28) / 2, "Too many attempts!");
        refresh();
        attroff(COLOR_PAIR(2));
        getch();
        return NULL;
    }
}


int check_username(char* username, int height, int width) {
    move(5, 0);
    clrtoeol();
    refresh();
    attron(A_BOLD | COLOR_PAIR(2));
    char* read_playar = (char*) calloc(200, sizeof(char));
    FILE* data_file = fopen("data/players.csv", "r");
    while (fgets(read_playar, 200, data_file)) {
        char* saved_username = strtok(read_playar, ",");
        if (!strcmp(saved_username, username)) {
            mvprintw(5, (width - 23) / 2, "Username already taken!");
            refresh();
            move(height / 2 - 8, 0);
            clrtoeol();
            refresh();
            attroff(A_BOLD | COLOR_PAIR(2));
            return 0;
        }
    }
    attroff(A_BOLD | COLOR_PAIR(2));
    fclose(data_file);
    return 1;
}


int check_pass(char* password, int height, int width) {
    clear_pass_errors();
    attron(A_BOLD | COLOR_PAIR(2));
    if (strlen(password) < 7) {
        mvprintw(5, (width - 50) / 2, "Your password should contain at least 7 characters!");
        refresh();
        move(height / 2 - 2, 0);
        clrtoeol();
        refresh();
        attroff(A_BOLD | COLOR_PAIR(2));
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
            attroff(A_BOLD | COLOR_PAIR(2));
            return 1;
        }
    }

    move(height / 2 - 2, (width - 18) / 2);
    clrtoeol();
    refresh();

    if (!upper_flag) {
        mvprintw(4, (width - 60) / 2, "Your password should contain at least 1 uppercase character!");
        refresh();
    }
    if (!lower_flag) {
        mvprintw(5, (width - 60) / 2, "Your password should contain at least 1 lowercase character!");
        refresh();
    }
    if (!digit_flag) {
        mvprintw(6, (width - 46) / 2, "Your password should contain at least 1 digit!");
        refresh();
    }

    attroff(A_BOLD | COLOR_PAIR(2));
    return 0;
}


void clear_pass_errors() {
    move(4, 0);
    clrtoeol();
    refresh();
    move(5, 0);
    clrtoeol();
    refresh();
    move(6, 0);
    clrtoeol();
    refresh();
}


int check_email(char* email, int height, int width) {
    move(5, 0);
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
        attron(A_BOLD | COLOR_PAIR(2));
        mvprintw(5, (width - 24) / 2, "Email format is invalid!");
        attroff(A_BOLD | COLOR_PAIR(2));
        move(height / 2 + 4, 0);
        clrtoeol();
        refresh();
        return 0;
    }

    return 1;
}


void get_player_stat(Player* player) {
    FILE* stat_file = fopen("data/stats.csv", "r");
    char* read_stat = (char*) calloc(200, sizeof(char));
    fgets(read_stat, 200, stat_file);
    char* stats = strtok(read_stat, ",");
    while(strcmp(stats, player->username)) {
        fgets(read_stat, 200, stat_file);
        stats = strtok(read_stat, ",");
    }
    stats = strtok(NULL, ",");
    player->score = atoi(stats);
    stats = strtok(NULL, ",");
    player->gold = atoi(stats);
    stats = strtok(NULL, ",");
    player->finished = atoi(stats);
    stats = strtok(NULL, ",");
    player->exp = atoi(stats);
    stats = strtok(NULL, ",");
    player->hero = stats;
    stats = strtok(NULL, ",");
    player->color = stats;
    stats = strtok(NULL, ",");
    player->difficulty = stats;
    stats = strtok(NULL, ",");
    player->fast_paced = atoi(stats);
    fclose(stat_file);
}