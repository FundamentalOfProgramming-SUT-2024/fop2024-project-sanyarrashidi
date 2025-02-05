#include "menus.h"


char main_menu(Player* player) {
    // Mix_Music* track1 = load_music("Music/blood.mp3");
    // Mix_Music* track2 = load_music("Music/champ.mp3");
    // Mix_Music* track3 = load_music("Music/damned.mp3");
    // Mix_Music* track4 = load_music("Music/doom.mp3");
    // Mix_Music* track5 = load_music("Music/glory.mp3");
    // Mix_Music* track6 = load_music("Music/life.mp3");
    // Mix_Music* track7 = load_music("Music/playground.mp3");
    // Mix_Music* track8 = load_music("Music/theme.mp3");
    // Mix_VolumeMusic(MIX_MAX_VOLUME);
    // Mix_PlayMusic(track1, -1);
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
            show_scoreboard(player);
            break;
        case 'p':
            show_profile(player);
            break;
        case 'a':
            settings(player);
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
    init_color(10, 800, 800, 800);
    init_color(11, 804, 498, 196);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, 10);
    init_pair(8, COLOR_BLACK, 11);
    attron(COLOR_PAIR(1));
    mvprintw(height / 2 - 10, (width - (8 + strlen(player->username))) / 2, "Welcome %s", player->username);
    refresh();
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2 - 6, (width - 19) / 2, "Press N for NEW GAME.");
    mvprintw(height / 2 - 4, (width - 20) / 2, "Press L for LOAD GAME.");
    mvprintw(height / 2 - 2, (width - 28) / 2, "Press S to see the SCOREBOARD.");
    mvprintw(height / 2, (width - 26) / 2, "Press P to see your PROFILE.");
    mvprintw(height / 2 + 2, (width - 24) / 2, "Press A to go to SETTINGS.");
    mvprintw(height / 2 + 4, (width - 18) / 2, "Press ESC to leave.");
    refresh();
    attroff(COLOR_PAIR(2));
}


void show_profile(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    mvprintw(height / 2 - 11, width / 4 - 2, "Press ANY KEY to go back to main menu");
    refresh();
    attron(COLOR_PAIR(1));
    mvprintw(height / 2 - 8, width / 4, "Username: %s", player->username);
    mvprintw(height / 2 - 6, width / 4, "Password: %s", player->password);
    mvprintw(height / 2 - 4, width / 4, "Email: %s", player->email);
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    mvprintw(height / 2 - 2, width / 4, "Points scored: %d", player->score);
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2, width / 4, "Gold achived: %d", player->gold);
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(5));
    mvprintw(height / 2 + 2, width / 4, "Finished games: %d", player->finished);
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(4));
    mvprintw(height / 2 + 4, width / 4, "Hero: %s", player->hero);
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(6));
    mvprintw(height / 2 + 6, width / 4, "Hero color: %s", player->color);
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2 + 8, width / 4, "Game difficulty: %s", player->difficulty);
    attroff(COLOR_PAIR(2));
    refresh();
    getch();
}


void show_scoreboard(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    mvprintw(4, (width - 38) / 2, "Press ANY KEY to go back to main menu.");
    refresh();
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    int* player_counter = (int*) malloc(sizeof(int));
    *player_counter = 0;
    Player* players = extract_players_stats(player_counter);

    qsort(players, *player_counter, sizeof(Player), compare_players_by_score);
    mvprintw(8, (width - 68) / 2, " #|      username      |  score   |   gold   | finished |");
    for (int i = 0; i < *player_counter; i++) {
        if (i >= 10) {
            break;
        }
        mvprintw(10 + 2 * (i - 1) + 1, (width - 68) / 2, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
        refresh();
        if (!strcmp(players[i].username, player->username)) {
            attron(COLOR_PAIR(4));
            mvprintw(10 + 2 * i, (width - 68) / 2 - 2, "->%2d|%-20s|%9d |%9d |%9d ", i + 1, players[i].username, players[i].score, players[i].gold, players[i].finished);
            if (i == 0) {
                mvprintw(10 + 2 * i, (width - 78) / 2, "\U0001F3C6 ");
                attroff(COLOR_PAIR(4));
                mvprintw(10 + 2 * i, width / 2 + 23, "(The GOAT)");
            }
            else if (i == 1) {
                mvprintw(10 + 2 * i, (width - 78) / 2, "\U0001F948 ");
                attroff(COLOR_PAIR(4));
                mvprintw(10 + 2 * i, width / 2 + 23, "(The LEGEND)");
            }
            else if (i == 2) {
                mvprintw(10 + 2 * i, (width - 78) / 2, "\U0001F949 ");
                attroff(COLOR_PAIR(4));
                mvprintw(10 + 2 * i, width / 2 + 23, "(The MASTER)");
            }
            else {
                attroff(COLOR_PAIR(4));
            }
            refresh();
        }
        else if (i == 0) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(10 + 2 * i, (width - 78) / 2, "\U0001F3C6   %2d|%-20s|%9d |%9d |%9d ", i + 1, players[i].username, players[i].score, players[i].gold, players[i].finished);
            refresh();
            attroff(COLOR_PAIR(1) | A_BOLD);
            mvprintw(10 + 2 * i, width / 2 + 23, "(The GOAT)");
        }
        else if (i == 1) {
            attron(COLOR_PAIR(7) | A_BOLD);
            mvprintw(10 + 2 * i, (width - 78) / 2, "\U0001F948   %2d|%-20s|%9d |%9d |%9d ", i + 1, players[i].username, players[i].score, players[i].gold, players[i].finished);
            refresh();
            attroff(COLOR_PAIR(7) | A_BOLD);
            mvprintw(10 + 2 * i, width / 2 + 23, "(The LEGEND)");
        }
        else if (i == 2) {
            attron(COLOR_PAIR(8) | A_BOLD);
            mvprintw(10 + 2 * i, (width - 78) / 2, "\U0001F949   %2d|%-20s|%9d |%9d |%9d ", i + 1, players[i].username, players[i].score, players[i].gold, players[i].finished);
            refresh();
            attroff(COLOR_PAIR(8) | A_BOLD);
            mvprintw(10 + 2 * i, width / 2 + 23, "(The MASTER)");
        }
        else {
            mvprintw(10 + 2 * i, (width - 68) / 2, "%2d|%-20s|%9d |%9d |%9d ", i + 1, players[i].username, players[i].score, players[i].gold, players[i].finished);
            refresh();
        }
    }
    getch();
}


int compare_players_by_score(void* first, void* second) {
    Player* player_1 = (Player*) first;
    Player* player_2 = (Player*) second;
    return player_2->score - player_1->score; 
}


// put music here too
void settings(Player* player) {
    clear();
    show_settings(player);
    char command;
    while ((command = getch())) {
        if (tolower(command) == 'd') {
            change_difficulty(player);
        }
        else if (tolower(command) == 'c') {
            change_color(player);
        }
        else if (tolower(command) == 'h') {
            change_hero(player);
        }
        else if (tolower(command) == 'b') {
            break;
        }
        clear();
        show_settings(player);
    }

    int* player_counter = (int*) malloc(sizeof(int));
    *player_counter = 0;
    Player* players = extract_players_stats(player_counter);
    for (int i = 0; i < *player_counter; i++) {
        if (!strcmp(players[i].username, player->username)) {
            players[i].hero = strdup(player->hero);
            players[i].color = strdup(player->color);
            players[i].difficulty = strdup(player->difficulty);
            break;
        }
    }

    refresh();
    FILE* stats_file = fopen("data/stats.csv", "w");
    for (int i = 0; i < *player_counter; i++) {
        fprintf(stats_file, "%s,%d,%d,%d,%d,%s,%s,%s,0,100,\n", players[i].username, players[i].score, players[i].gold, players[i].finished, players[i].exp, players[i].hero, players[i].color, players[i].difficulty);
    }
    fclose(stats_file);
}


void show_settings(Player* player) {
    int height, width;
    getmaxyx(stdscr, height, width);
    mvprintw(10, (width - 33) / 2, "Press B to go back to main menu.");
    mvprintw(height / 2 - 6, (width - (44 + strlen(player->difficulty))) / 2, "Press D to change difficulty.(currently on %s)", player->difficulty);
    mvprintw(height / 2 - 2, (width - (44 + strlen(player->color))) / 2, "Press C to change hero color.(currently on %s)", player->color);
    mvprintw(height / 2 + 2, (width - 46) / 2, "Press H to change hero(currently on %s).", player->hero);
    refresh();
}


void change_difficulty(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    attron(COLOR_PAIR(4));
    mvprintw(height / 2 - 6, (width - 37) / 2, "Press E to change difficulty to easy.");
    refresh();
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2 - 2, (width - 39) / 2, "Press M to change difficulty to medium.");
    refresh();
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(height / 2 + 2, (width - 37) / 2, "Press H to change difficulty to hard.");
    refresh();
    attroff(COLOR_PAIR(3));
    char difficulty;
    while ((difficulty = getch())) {
        if (tolower(difficulty) == 'e') {
            player->difficulty = "easy";
            break;
        }
        else if (tolower(difficulty) == 'm') {
            player->difficulty = "medium";
            break;
        }
        else if (tolower(difficulty) == 'h') {
            player->difficulty = "hard";
            break;
        }
    }
}


void change_color(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    attron(COLOR_PAIR(4));
    mvprintw(height / 2 - 8, (width - 33) / 2, "Press G to change color to green.");
    refresh();
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(2));
    mvprintw(height / 2 - 4, (width - 34) / 2, "Press Y to change color to yellow.");
    refresh();
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(height / 2, (width - 31) / 2, "Press R to change color to red.");
    refresh();
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(1));
    mvprintw(height / 2 + 4, (width - 32) / 2, "Press B to change color to blue.");
    refresh();
    attroff(COLOR_PAIR(1));
    mvprintw(height / 2 + 8, (width - 33) / 2, "Press W to change color to white.");
    refresh();
    char color;
    while ((color = getch())) {
        if (tolower(color) == 'g') {
            player->color = "green";
            break;
        }
        else if (tolower(color) == 'y') {
            player->color = "yellow";
            break;
        }
        else if (tolower(color) == 'r') {
            player->color = "red";
            break;
        }
        else if (tolower(color) == 'b') {
            player->color = "blue";
            break;
        }
        else if (tolower(color) == 'w') {
            player->color = "white";
            break;
        }
    }
}


void change_hero(Player* player) {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);
    mvprintw(10, (width - 28) / 2, "Press P to change hero to \u265F.");
    mvprintw(height / 2 - 6, (width - 28) / 2, "Press K to change hero to \u265E.");
    mvprintw(height / 2 - 2, (width - 28) / 2, "Press B to change hero to \u265D.");
    mvprintw(height / 2 + 2, (width - 28) / 2, "Press R to change hero to \u265C.");
    refresh();
    char hero;
    while ((hero = getch())) {
        if (tolower(hero) == 'p') {
            player->hero = "\u265F";
            break;
        }
        else if (tolower(hero) == 'k') {
            player->hero = "\u265E";
            break;
        }
        else if (tolower(hero) == 'b') {
            player->hero = "\u265D";
            break;
        }
        else if (tolower(hero) == 'r') {
            player->hero = "\u265C";
            break;
        }
    }
}


Player* extract_players_stats(int* num_of_players) {
    Player* players = (Player*) calloc(100, sizeof(Player));
    FILE* stats_file = fopen("data/stats.csv", "r");
    char* read_stats = (char*) calloc(200, sizeof(char));
    char* player_inf = (char*) calloc(100, sizeof(char));
    while(fgets(read_stats, 200, stats_file)) {
        player_inf = strtok(read_stats, ",");
        players[*num_of_players].username = strdup(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].score = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].gold = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].finished = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].exp = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].hero = strdup(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].color = strdup(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].difficulty = strdup(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].fast_paced = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].hp = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].hunger = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].unlocked_levels = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].current_level = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].x = atoi(player_inf);
        player_inf = strtok(NULL, ",");
        players[*num_of_players].y = atoi(player_inf);
        (*num_of_players)++;
    }

    return players;
}


// void music_menu() {

// }


// void extract_tracks(Mix_Music** tracks) {
//     tracks[0] = load_music("Music/blood.mp3");
//     tracks[1] = load_music("Music/champ.mp3");
//     tracks[2] = load_music("Music/damned.mp3");
//     tracks[3] = load_music("Music/doom.mp3");
//     tracks[4] = load_music("Music/glory.mp3");
//     tracks[5] = load_music("Music/life.mp3");
//     tracks[6] = load_music("Music/playground.mp3");
//     tracks[7] = load_music("Music/theme.mp3");
// }