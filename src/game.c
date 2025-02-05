#include "game.h"


void game_ui(Player* player, bool new) {
    int height, width;
    getmaxyx(stdscr, height, width);
    init_color(10, 1000, 843, 0);
    init_color(11, 0, 0, 1000);
    init_color(12, 1000, 0, 0);
    init_color(13, 0, 1000, 0);
    init_color(14, 1000, 1000, 0);
    init_color(15, 1000, 500, 0);
    init_pair(1, 11, COLOR_BLACK);
    init_pair(2, 13, COLOR_BLACK);
    init_pair(3, 10, COLOR_BLACK);
    init_pair(4, 12, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, 14, COLOR_BLACK);
    init_pair(8, COLOR_YELLOW, COLOR_BLACK);
    init_pair(9, COLOR_WHITE, 12);
    init_pair(20, 15, COLOR_BLACK);
    
    if (!strcmp(player->difficulty, "easy")) {
        player->difficulty_coeff = 1;
    }
    else if (!strcmp(player->difficulty, "medium")) {
        player->difficulty_coeff = 2;
    }
    else {
        player->difficulty_coeff = 3;
    }

    Backpack* backpack = (Backpack*) malloc(sizeof(Backpack));
    int level = player->current_level;
    int claimed_gold = 0;

    Room*** all_levels = (Room***) calloc(4, sizeof(Room**));
    char*** final_corridors = (char***) calloc(4, sizeof(char**));
    char*** corridors = (char***) calloc(4, sizeof(char**));
    int result;
    if (!new) {
        for (int i = 0; i < player->unlocked_levels; i++) {
            all_levels[i] = read_rooms(player, i + 1);
            final_corridors[i] = read_corridors(player, height, width, i + 1, true);
            corridors[i] = read_corridors(player, height, width, i + 1, false);
        }

        backpack = read_backpack(player);
        result = load_level(player, backpack, all_levels[level - 1], final_corridors[level - 1], corridors[level - 1], &claimed_gold, level);
    }
    else {
        backpack->count_weapons = 1;
        backpack->weapons = (Weapon**) calloc(5, sizeof(Weapon*));
        backpack->weapons[0] = (Weapon*) malloc(sizeof(Weapon));
        backpack->weapons[0]->type = 'M';
        backpack->weapons[0]->damage = 5;
        backpack->weapons[0]->symbol = L'\u2692';
        backpack->default_weapon = backpack->weapons[0];
        backpack->spells = (Spell**) calloc(5, sizeof(Spell*));
        backpack->count_spells = 0;
        backpack->food = (Food**) calloc(5, sizeof(Food*));
        backpack->count_food = 0;
        player->gold = 0;
        player->fast_paced = 0;
        player->hp = 100;
        player->hunger = 5;
        player->unlocked_levels = 1;
        player->current_level = 1;
        result = load_level(player, backpack, NULL, NULL, NULL, &claimed_gold, level);
    }

    save_backpack(player, backpack);

    while (result) {
        if (result == -1) {
            player->current_level--;
            level = player->current_level;
            result = load_level(player, backpack, all_levels[level - 1], final_corridors[level - 1], corridors[level - 1], &claimed_gold, level);
        }
        else {
            player->current_level++;
            level = player->current_level;
            if (level > player->unlocked_levels) {
                player->unlocked_levels++;
                result = load_level(player, backpack, NULL, NULL, NULL, &claimed_gold, level);
                all_levels[level - 1] = read_rooms(player, level);
                final_corridors[level - 1] = read_corridors(player, height, width, level, true);
                corridors[level - 1] = read_corridors(player, height, width, level, false);
            }
            else {
                result = load_level(player, backpack, all_levels[level - 1], final_corridors[level - 1], corridors[level - 1], &claimed_gold, level);
            }
        }
    }
}


int load_level(Player* player, Backpack* backpack, Room** rooms, char** final_corridors, char** corridors, int* claimed_gold, int level) { 
    int height, width;
    getmaxyx(stdscr, height, width);
    bool new_level = false;
    if (rooms == NULL && final_corridors == NULL) {
        rooms = generate_map(player->difficulty, level);
        display_rooms(rooms, rooms[0]->total_rooms);
        clear(); 
        generate_corridors(rooms, rooms[0]->total_rooms);
        save_rooms(rooms, player, level);
        refresh();
        final_corridors = save_corridors();
        save_corridors_to_file(player, final_corridors, height, width, level, true);
        new_level = true;
        clear();
        display_single_room(rooms[0]);
    }
    else {
        clear();
        for (int i = 0; i < rooms[0]->total_rooms; i++) {
            if (rooms[i]->visited) {
                display_single_room(rooms[i]);
            }
        }

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (corridors[i][j] == '#' || corridors[i][j] == '+') {
                    mvprintw(i, j, "%c", corridors[i][j]);
                }
            }
        }
    }
    
    char command;
    int current_y;
    int current_x;
    char prev_char;
    if (new_level) {
        current_y = rooms[0]->corner_y + 1;
        current_x = rooms[0]->corner_x + 1;
        player->x = rooms[0]->corner_x + 1;
        player->y = rooms[0]->corner_y + 1;
        prev_char = (char) mvinch(current_y, current_x);
        move_player(player, rooms[0]->corner_y + 1, rooms[0]->corner_x + 1);
    }
    else {
        current_y = player->y;
        current_x = player->x;
        prev_char = '.';
        move_player(player, player->y, player->x);
    }
    show_game_bar(player, backpack, *claimed_gold);

    int hidden_door_x, hidden_door_y;
    bool found_hidden = false;
    for (int i = 0; i < 8; i++) {
        if (rooms[i]->door_count == 3) {
            hidden_door_x = rooms[i]->hidden_x[0];
            hidden_door_y = rooms[i]->hidden_y[0];
            break;
        }
    }

    Coin* found_coin = NULL;
    Spell* found_spell = NULL;
    Food* found_food = NULL;
    Weapon* found_weapon = NULL;
    bool trap_triggered = false;
    bool bottom_reached = false;
    bool top_reached = false;
    bool left_reached = false;
    bool right_reached = false;
    bool health_spell_used = false;
    bool speed_spell_used = false;
    bool damage_spell_used = false;
    bool special_food_used = false;
    bool magic_food_used = false;
    bool entered_battle_room = false;
    int kills = 0;
    Monster** battle_monsters = (Monster**) calloc(20, sizeof(Monster*));
    Monster** normal_monsters = (Monster**) calloc(20, sizeof(Monster*));
    int main_x;
    int main_y;
    int last_trap_x;
    int last_trap_y;

    bool game_is_running = true;
    nodelay(stdscr, TRUE);
    long last_hunger_update = get_current_time();
    long last_health_update = last_hunger_update;
    long last_ench_effect_update;
    long last_spell_effect_update = 0;
    long last_food_effect_update = 0;
    long last_monster_place_update;
    long last_monster_hit_update;
    const long hunger_interval = 60 * 1000000L;
    const long hunger_to_damage_interval = 60 * 1000000L;
    const long recover_health_interval = 5 * 1000000L;
    const long enchant_room_effect_interval = 5 * 1000000L;
    const long spell_effect_interval = 10 * 1000000L;
    const long food_effect_interval = 10 * 1000000L;
    const long monster_movement_interval = 1 * 1000000L;
    const long monster_damage_interval = 2 * 1000000L;
    Room* current_room = rooms[0];
    while (game_is_running) {
        command = getch();
        char next_char;

        if (command != ERR) {
            move(2, strlen(player->username) + 28);
            clrtoeol();
            mvaddch(2, width - 1, '|');
            move(current_y, current_x);
            show_defaults(player, backpack);
            current_room = get_current_room(rooms, current_y, current_x);
        }

        top_reached = (current_y == 0);
        bottom_reached = (current_y == height - 1);
        left_reached = (current_x == 0);
        right_reached = (current_x == width - 1);

        int num_monsters;
        if (current_room != NULL && current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
            num_monsters = 20;
        }
        else {
            num_monsters = 12;
        }

        if (current_room != NULL) {
            normal_monsters = current_room->monsters;
        }

        long current_time = get_current_time();
        if (player->hunger > 0 && current_time - last_hunger_update >= hunger_interval) {
            player->hunger--;
            last_hunger_update = current_time;
        }
        show_hunger_bar(player);

        if (player->hunger == 0 && current_time - last_health_update >= hunger_to_damage_interval) {
            player->hp -= 10;
            last_health_update = current_time;
        }
        if (player->hunger == 5 && player->hp != 100 && current_time - last_health_update >= recover_health_interval) {
            player->hp += 5 * (health_spell_used ? 2 : 1);
            last_health_update = current_time;
        }
        show_health_bar(player);

        if (health_spell_used && current_time - last_spell_effect_update >= spell_effect_interval) {
            health_spell_used = false;
        }

        if (speed_spell_used && current_time - last_spell_effect_update >= spell_effect_interval) {
            player->fast_paced = 0;
            speed_spell_used = false;
        }

        if (damage_spell_used && current_time - last_spell_effect_update >= spell_effect_interval) {
            for (int i = 0; i < backpack->count_weapons; i++) {
               backpack->weapons[i]->damage /= 2;
            }
            damage_spell_used = false;
        }

        if (special_food_used && current_time - last_food_effect_update >= food_effect_interval) {
            for (int i = 0; i < backpack->count_weapons; i++) {
                backpack->weapons[i]->damage /= 2;
            }
            special_food_used = false;
        }

        if (magic_food_used && current_time - last_food_effect_update >= food_effect_interval) {
            player->fast_paced = 0;
            magic_food_used = false;
        }

        if (entered_battle_room && current_time - last_monster_place_update >= monster_movement_interval) {
            monster_movments(battle_monsters, player, num_monsters);
            last_monster_place_update = get_current_time();
        }

        if (entered_battle_room && current_time - last_monster_hit_update >= monster_damage_interval) {
            monsters_damage(battle_monsters, player, num_monsters);
            last_monster_hit_update = get_current_time();
        }

        if (player->hp <= 0) {
            player->finished++;
            death(width);
            game_is_running = false;
            command = 'q';
            getch();
        }

        if (current_room != NULL && current_room->type == 'E' && !entered_battle_room) {
            if (current_time - last_ench_effect_update >= enchant_room_effect_interval) {
                player->hp -= 5;
                last_ench_effect_update = current_time;
            }
            attron(COLOR_PAIR(9));
            mvprintw(2, width / 2 - 18, "You are in enchant room! Escape soon!");
            attroff(COLOR_PAIR(9));
        }
        show_health_bar(player);

        if (entered_battle_room && kills == num_monsters && num_monsters != 20) {
            kills = 0;
            entered_battle_room = false;
            clear();
            for (int i = 0; i < rooms[0]->total_rooms; i++) {
                if (rooms[i]->visited) {
                    display_single_room(rooms[i]);
                }
            }

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (corridors[i][j] == '#' || corridors[i][j] == '+') {
                        mvprintw(i, j, "%c", corridors[i][j]);
                    }
                }
            }

            player->x = main_x;
            player->y = main_y;
            current_x = main_x;
            current_y = main_y;
            move_player(player, main_y, main_x);
            show_game_bar(player, backpack, *claimed_gold);
            mvprintw(last_trap_y, last_trap_x, "\U0001F571");
        }

        if (entered_battle_room && kills == num_monsters && num_monsters == 20) {
            clear();
            mvprintw(height / 2, (width - 18) / 2, "Congrats! You won!");
            mvprintw(height / 2 + 2, (width - 13) / 2, "+1000 scores!");
            player->score += 1000;
            save_player(player);
            getch();
            sleep(5);
            endwin();
            exit(0);
        }

        
        if (player->x == rooms[rooms[0]->total_rooms - 1]->corner_x + 1 && player->y == rooms[rooms[0]->total_rooms - 1]->corner_y + 1) {
            clear();
            entered_battle_room = true;
            num_monsters = 20;
            battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
            entered_battle_room = true;
            int num_monsters;
            if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                num_monsters = 20;
            }
            else {
                num_monsters = 12;
            }
            main_x = current_x;
            main_y = current_y - 1;
            last_trap_x = current_x;
            last_trap_y = current_y;
            corridors = save_corridors();
            battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
            current_y++;
            show_battle_bar(backpack, kills);
            show_game_bar(player, backpack, *claimed_gold);
        }

        switch (command) {
        case 'q':
            if (entered_battle_room) {
                entered_battle_room = false;
                clear();
                for (int i = 0; i < rooms[0]->total_rooms; i++) {
                    if (rooms[i]->visited) {
                        display_single_room(rooms[i]);
                    }
                }

                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        if (corridors[i][j] == '#' || corridors[i][j] == '+') {
                            mvprintw(i, j, "%c", corridors[i][j]);
                        }
                    }
                }

                player->x = main_x;
                player->y = main_y;
                current_x = main_x;
                current_y = main_y;
                move_player(player, main_y, main_x);
                show_game_bar(player, backpack, *claimed_gold);
                mvprintw(last_trap_y, last_trap_x, "\U0001F571");
            }
            else {
                game_is_running = false;
            }
            break;
        case '8':
            if (top_reached) {
                break;
            }
            next_char = (char) mvinch(current_y - 1, current_x);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                    entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x;
                    main_y = current_y - 1;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_y++;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;                   
                current_y--;                             
                player->y--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '6':
            if (right_reached) {
                break;
            }
            next_char = (char) mvinch(current_y, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x - 1;
                    main_y = current_y;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_x--;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_x++;
                player->x++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '2':
            if (bottom_reached) {
                break;
            }
            next_char = (char) mvinch(current_y + 1, current_x);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x;
                    main_y = current_y + 1;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_y--;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y++;
                player->y++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '4':
            if (left_reached) {
                break;
            }
            next_char = (char) mvinch(current_y, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x - 1;
                    main_y = current_y;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_x++;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_x--;
                player->x--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '7':
            if (top_reached || left_reached) {
                break;
            }
            next_char = (char) mvinch(current_y - 1, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x - 1;
                    main_y = current_y - 1;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_x++;
                    current_y++;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y--;
                current_x--;
                player->x--;
                player->y--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '9':
            if (top_reached || right_reached) {
                break;
            }
            next_char = (char) mvinch(current_y - 1, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x + 1;
                    main_y = current_y - 1;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_x--;
                    current_y++;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y--;
                current_x++;
                player->x++;
                player->y--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '3':
            if (bottom_reached || right_reached) {
                break;
            }
            next_char = (char) mvinch(current_y + 1, current_x + 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x + 1;
                    main_y = current_y + 1;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_x--;
                    current_y--;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y++;
                current_x++;
                player->y++;
                player->x++;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
            break;
        case '1':
            if (bottom_reached || left_reached) {
                break;
            }
            next_char = (char) mvinch(current_y + 1, current_x - 1);
            if (next_char != '-' && next_char != 'O' && next_char != ' ' && next_char != '|' && next_char != 'D' && next_char != 'F' && next_char != 'G' && next_char != 'S' && next_char != 'U') {
                if (!player->fast_paced) {    
                    usleep(150000);
                }
                if (trap_triggered && !entered_battle_room) {
                    mvprintw(current_y, current_x, "\U0001F571");
                    trap_triggered = false;
                     entered_battle_room = true;
                    int num_monsters;
                    if (current_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        num_monsters = 20;
                    }
                    else {
                        num_monsters = 12;
                    }
                    main_x = current_x - 1;
                    main_y = current_y + 1;
                    last_trap_x = current_x;
                    last_trap_y = current_y;
                    corridors = save_corridors();
                    battle_monsters = battle_room(player, backpack, *claimed_gold, num_monsters, &current_y, &current_x);
                    current_x++;
                    current_y--;
                    show_battle_bar(backpack, kills);
                }
                else if (found_coin && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "\u25CC");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_spell && prev_char != '.') {
                    attron(COLOR_PAIR(6));
                    mvprintw(current_y, current_x, "\U0001F70F");
                    attroff(COLOR_PAIR(6));
                }
                else if (found_food && prev_char != '.') {
                    attron(COLOR_PAIR(3));
                    mvprintw(current_y, current_x, "@");
                    attroff(COLOR_PAIR(3));
                }
                else if (found_weapon && prev_char != '.') {
                    attron(COLOR_PAIR(5));
                    mvprintw(current_y, current_x, "\u2692");
                    attroff(COLOR_PAIR(5));
                }
                else {
                    mvprintw(current_y, current_x, "%lc", prev_char); 
                }
                prev_char = next_char;
                current_y++;
                current_x--;
                player->y++;
                player->x--;
                move_player(player, current_y, current_x);
                refresh();
            }
            else {
                mvprintw(2, width / 2 - 15, "You cannot move there mate!");
            }
        case 'c':
            if (found_coin != NULL) {
                found_coin->claimed = true;
                char* new_gold = (char*) malloc(10 * sizeof(char));
                char* prev_gold = (char*) malloc(10 * sizeof(char));
                *claimed_gold += 10;
                player->gold += 10;
                prev_char = '.';
                if (!strcmp(player->difficulty, "easy")) {
                    player->score += 10;
                }
                else if (!strcmp(player->difficulty, "medium")) {
                    player->score += 20;
                }
                else {
                    player->score += 30;
                }
                new_gold[0] = '\0';
                prev_gold[0] = '\0';
                move(2, 1);
                clrtoeol();
                move(3, 1);
                clrtoeol();
                show_defaults(player, backpack);
                mvaddch(2, width - 1, '|');
                mvaddch(3, width - 1, '|');
                mvprintw(2, width / 2 - 6, "Coin claimed!");
                move(current_y, current_x);
                sprintf(new_gold, "%d", *claimed_gold);
                sprintf(prev_gold, "%d", player->gold);
                attron(COLOR_PAIR(3));
                mvprintw(2, 2, "Gold earned: %s", new_gold);
                mvprintw(3, 2, "Total gold: %s", prev_gold); 
                attroff(COLOR_PAIR(3));
                mvprintw(2, strlen(player->username) + 15, "|");
                mvprintw(3, strlen(player->username) + 15, "|");
            }

            if (found_spell != NULL) {
                found_spell->claimed = true;
                mvprintw(2, width / 2 - 6, "Spell claimed!");
                prev_char = '.';
                if (backpack->count_spells == 0) {
                    backpack->default_spell = found_spell;
                }

                bool new_spell = true;
                for (int i = 0; i < backpack->count_spells; i++) {
                    if (backpack->spells[i]->type == found_spell->type) {
                        backpack->spells[i]->amount++;
                        new_spell = false;
                    }
                }

                if (new_spell) {
                    found_spell->amount = 1;
                    backpack->spells[backpack->count_spells] = found_spell;
                    backpack->count_spells++;
                }
            }

            if (found_food != NULL) {
                found_food->claimed = true;
                mvprintw(2, width / 2 - 6, "Food claimed!");
                prev_char = '.';
                if (backpack->count_food == 0) {
                    backpack->default_food = found_food;
                }

                bool new_food = true;
                for (int i = 0; i < backpack->count_food; i++) {
                    if (backpack->food[i]->type == found_food->type) {
                        backpack->food[i]->amount++;
                        new_food = false;
                    }
                }

                if (new_food) {
                    found_food->amount = 1;
                    backpack->food[backpack->count_food] = found_food;
                    backpack->count_food++;
                }
            }

            if (found_weapon != NULL) {
                found_weapon->claimed = true;
                mvprintw(2, width / 2 - 6, "Weapon claimed!");
                prev_char = '.';
                
                bool new_weapon = true;
                for (int i = 0; i < backpack->count_weapons; i++) {
                    if (backpack->weapons[i]->type == found_weapon->type) {
                        if (found_weapon->type == 'D') {
                            backpack->weapons[i]->ammo += 10;
                            new_weapon = false;
                        }
                        else if (found_weapon->type == 'W') {
                            backpack->weapons[i]->ammo += 8;
                            new_weapon = false;
                        }
                        else if (found_weapon->type == 'A') {
                            backpack->weapons[i]->ammo += 20;
                            new_weapon = false;
                        }
                    }
                }

                if (new_weapon) {
                    backpack->weapons[backpack->count_weapons] = found_weapon;
                    if (found_weapon->type == 'D') {
                        backpack->weapons[backpack->count_weapons]->ammo = 10;
                    }
                    else if (found_weapon->type == 'W') {
                        backpack->weapons[backpack->count_weapons]->ammo = 8;
                    }
                    else if (found_weapon->type == 'A') {
                        backpack->weapons[backpack->count_weapons]->ammo = 20;
                    }
                    backpack->count_weapons++;
                }
            }
            break;
        case 'f':
            player->fast_paced = !player->fast_paced;
            if (player->fast_paced) {
                mvprintw(1, strlen(player->username) + 13, "\u26A1");
                mvprintw(2, width / 2 - 12, "Fast-paced mode enabled!");
            }
            else {
                mvprintw(1, strlen(player->username) + 13, " ");
                mvprintw(2, width / 2 - 12, "Fast-paced mode disabled!");
            }
            break;
        case 'i':
            inventory_menu(player, backpack);
            show_defaults(player, backpack);
            break;
        case 'w':
            weapon_menu(player, backpack);
            show_defaults(player, backpack);
            break;
        case 's':
            if (backpack->default_spell != NULL && !health_spell_used && !speed_spell_used && !damage_spell_used && backpack->default_spell->amount > 0) {
                if (backpack->default_spell->type == 'H') {
                    health_spell_used = true;
                    mvprintw(2, width / 2 - 9, "Health spell used!");
                }
                else if (backpack->default_spell->type == 'S') {
                    speed_spell_used = true;
                    player->fast_paced = 1;
                    mvprintw(2, width / 2 - 8, "Speed spell used!");
                }
                else {
                    damage_spell_used = true;
                    mvprintw(2, width / 2 - 9, "Damage spell used!");
                    for (int i = 0; i < backpack->count_weapons; i++) {
                        backpack->weapons[i]->damage *= 2;
                    }
                }
                backpack->default_spell->amount--;
                last_spell_effect_update = get_current_time();
            }
            break;
        case 'e':
            if (backpack->default_food != NULL && !special_food_used && !magic_food_used && backpack->default_food->amount > 0) {
                if (backpack->default_food->type == 'N') {
                    int expired_prob = rand() % 4;
                    if (expired_prob == 0) {
                        player->hp -= 20;
                        show_health_bar(player);
                        mvprintw(2, width / 2 - 6, "Expired Food!");
                    }
                    else {
                        mvprintw(2, width / 2 - 8, "Normal food used!");
                        player->hunger = 5;
                    }
                }
                else if (backpack->default_food->type == 'S') {
                    player->hunger = 5;
                    special_food_used = true;
                    mvprintw(2, width / 2 - 9, "Special food used!");
                    for (int i = 0; i < backpack->count_weapons; i++) {
                        backpack->weapons[i]->damage *= 2;
                    }
                }
                else {
                    player->hunger = 5;
                    magic_food_used = true;
                    player->fast_paced = 1;
                    mvprintw(2, width / 2 - 8, "Magic food used!");
                }
                backpack->default_food->amount--;
                last_food_effect_update = get_current_time();
                show_hunger_bar(player);
            }
            break;
        case 'm':
            if (!entered_battle_room) {
                corridors = save_corridors();
                clear();
                for (int i = 0; i < rooms[0]->total_rooms; i++) {
                    display_single_room(rooms[i]);
                }

                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        if (final_corridors[i][j] == '#' || final_corridors[i][j] == '+') {
                            mvprintw(i, j, "%c", final_corridors[i][j]);
                        }
                    }
                }
                refresh();
                sleep(3);
                clear();
                for (int i = 0; i < rooms[0]->total_rooms; i++) {
                    if (rooms[i]->visited) {
                        display_single_room(rooms[i]);
                    }
                }

                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        if (corridors[i][j] == '#' || corridors[i][j] == '+') {
                            mvprintw(i, j, "%c", corridors[i][j]);
                        }
                    }
                }

                show_game_bar(player, backpack, *claimed_gold);
                move_player(player, current_y, current_x);
                refresh();
            }
            break;
        case 32:
            if (entered_battle_room) {
                if (backpack->default_weapon->type == 'M' || backpack->default_weapon->type == 'S') {
                    mvprintw(2, width / 2 - 12, "Enter direction");
                    char direction = getch();
                    while (1) {
                        if (direction == '4') {
                            Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        else if (direction == '8') {
                            Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        else if (direction == '6') {
                            Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        else if (direction == '2') {
                            Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        direction = getch();
                    }
                }
                else {
                    if (backpack->default_weapon->ammo > 0) {
                        backpack->default_weapon->ammo--;
                        mvprintw(2, width / 2 - 12, "Enter direction");
                        char direction = getch();
                        while (1) {
                            if (direction == '4') {
                                Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            else if (direction == '8') {
                                Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            else if (direction == '6') {
                                Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            else if (direction == '2') {
                                Monster* found = find_monster(battle_monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            direction = getch();
                        }
                    }
                }
                show_battle_bar(backpack, kills);
            }
            else {
                if (backpack->default_weapon->type == 'M' || backpack->default_weapon->type == 'S') {
                    mvprintw(2, width / 2 - 12, "Enter direction");
                    char direction = getch();
                    while (1) {
                        if (direction == '4') {
                            Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        else if (direction == '8') {
                            Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        else if (direction == '6') {
                            Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        else if (direction == '2') {
                            Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                            if (found != NULL) {
                                found->hp -= backpack->default_weapon->damage;
                                if (found->hp <= 0) {
                                    found->alive = false;
                                    mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                    kills++;
                                    mvaddch(found->y, found->x, ".");
                                }
                            }
                            break;
                        }
                        direction = getch();
                    }
                }
                else if (current_room != NULL) {
                    if (backpack->default_weapon->ammo > 0) {
                        backpack->default_weapon->ammo--;
                        mvprintw(2, width / 2 - 12, "Enter direction");
                        char direction = getch();
                        while (1) {
                            if (direction == '4') {
                                Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            else if (direction == '8') {
                                Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            else if (direction == '6') {
                                Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            else if (direction == '2') {
                                Monster* found = find_monster(current_room->monsters, player, direction, num_monsters);
                                if (found != NULL) {
                                    found->hp -= backpack->default_weapon->damage;
                                    if (found->hp <= 0) {
                                        found->alive = false;
                                        mvprintw(2, width / 2 - 12, "Monster eliminated!");
                                        kills++;
                                        mvaddch(found->y, found->x, ".");
                                    }
                                }
                                break;
                            }
                            direction = getch();
                        }
                    }
                }
                show_battle_bar(backpack, kills);
            }
            break;
        case 27:
            bool is_leaving = show_pause_menu(player, width);
            if (is_leaving) {
                char** current_corridors = save_corridors();
                save_corridors_to_file(player, current_corridors, height, width, level, false);
                save_player(player);
                save_rooms(rooms, player, level);
                save_backpack(player, backpack);
                mvprintw(2, width / 2 - 12, "Your game has been saved!");
                refresh();
                getch();
                game_is_running = false;
            }
            show_game_bar(player, backpack, *claimed_gold);
        default:
            break;
        }

        if (prev_char == '+' && !entered_battle_room) {
            Room* visited_room = find_room_by_door(rooms, current_y, current_x);
            if (!visited_room->visited) {
                visited_room->visited = true;
                display_single_room(visited_room);
                if (visited_room->type == 'E') {
                    mvprintw(2, width / 2 - 18, "You have discovered the enchant room!");
                    last_ench_effect_update = get_current_time();
                }
                else {
                    mvprintw(2, width / 2 - 15, "You have discovered a new room!");
                    if (visited_room->corner_x == rooms[rooms[0]->total_rooms - 1]->corner_x) {
                        mvprintw(rooms[rooms[0]->total_rooms - 1]->corner_y + 1, rooms[rooms[0]->total_rooms - 1]->corner_x + 1, "\U0001F3C6");
                    }
                }
                move_player(player, current_y, current_x);
            }
        }

        if (!entered_battle_room && !found_hidden && found_hidden_door(current_y, current_x, hidden_door_y, hidden_door_x)) {
            found_hidden = true;
            mvaddch(hidden_door_y, hidden_door_x, '$');
            mvprintw(2, width / 2 - 15, "You have found a hidden door!");
        }

        move(current_y, current_x);

        if (prev_char == '+' || prev_char == '#' || prev_char == '$') {
            if (!right_reached && (final_corridors[current_y][current_x + 1] == '#' || final_corridors[current_y][current_x + 1] == '+')) {
                mvaddch(current_y, current_x + 1, final_corridors[current_y][current_x + 1]);
            }
            if (!left_reached && (final_corridors[current_y][current_x - 1] == '#' || final_corridors[current_y][current_x - 1] == '+')) {
                mvaddch(current_y, current_x - 1, final_corridors[current_y][current_x - 1]);
            }
            if (!top_reached && (final_corridors[current_y - 1][current_x] == '#' || final_corridors[current_y - 1][current_x] == '+')) {
                mvaddch(current_y - 1, current_x, final_corridors[current_y - 1][current_x]);
            }
            if (!bottom_reached && (final_corridors[current_y + 1][current_x] == '#' || final_corridors[current_y + 1][current_x] == '+')) {
                mvaddch(current_y + 1, current_x, final_corridors[current_y + 1][current_x]);
            }
            if (!left_reached && !top_reached && (final_corridors[current_y - 1][current_x - 1] == '#' || final_corridors[current_y - 1][current_x - 1] == '+')) {
                mvaddch(current_y - 1, current_x - 1, final_corridors[current_y - 1][current_x - 1]);
            }
            if (!bottom_reached && !left_reached && (final_corridors[current_y + 1][current_x - 1] == '#' || final_corridors[current_y + 1][current_x - 1] == '+')) {
                mvaddch(current_y + 1, current_x - 1, final_corridors[current_y + 1][current_x - 1]);
            }
            if (!top_reached && !right_reached && (final_corridors[current_y - 1][current_x + 1] == '#' || final_corridors[current_y - 1][current_x + 1] == '+')) {
                mvaddch(current_y - 1, current_x + 1, final_corridors[current_y - 1][current_x + 1]);
            }
            if (!bottom_reached && !right_reached && (final_corridors[current_y + 1][current_x + 1] == '#' || final_corridors[current_y + 1][current_x + 1] == '+')) {
                mvaddch(current_y + 1, current_x + 1, final_corridors[current_y + 1][current_x + 1]);
            }
        }

        if (!entered_battle_room) {
            trap_triggered = stepped_on_trap(rooms, player, current_y, current_x, width);
            found_coin = stepped_on_loot(rooms, current_y, current_x, width);
            found_spell = stepped_on_spell(rooms, current_y, current_x, width);
            found_food = stepped_on_food(rooms, current_y, current_x, width);
            found_weapon = stepped_on_weapon(rooms, current_y, current_x, width);
        }
        else {
            show_battle_bar(backpack, kills);
        }
        refresh();
        move(current_y, current_x);
        usleep(10000);
    }
}

void move_player(Player* player, int y, int x) {
    attron(A_BOLD);
    if (!strcmp(player->color, "blue")) {
        attron(COLOR_PAIR(1));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(1));
    }
    else if (!strcmp(player->color, "yellow")) {
        attron(COLOR_PAIR(3));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(3));
    }
    else if (!strcmp(player->color, "green")) {
        attron(COLOR_PAIR(2));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(2));
    }
    else if (!strcmp(player->color, "white")) {
        mvprintw(y, x, player->hero);
    }
    else if (!strcmp(player->color, "red")) {
        attron(COLOR_PAIR(4));
        mvprintw(y, x, player->hero);
        attroff(COLOR_PAIR(4));
    }
    attroff(A_BOLD);
}


bool found_hidden_door(int y, int x, int door_y, int door_x) {
    if (door_y == y - 1 && door_x == x) {
        return true;
    }
    else if (door_y == y + 1 && door_x == x) {
        return true;
    }
    else if (door_y == y && door_x == x - 1) {
        return true;
    }
    else if (door_y == y && door_x == x + 1) {
        return true;
    }

    return false;
}


bool stepped_on_trap(Room** rooms, Player* player, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->trap_count; j++) {
            if (rooms[i]->traps[j]->y == y && rooms[i]->traps[j]->x == x) {
                if (!rooms[i]->traps[j]->found) {
                    player->hp -= 5;
                    show_health_bar(player);
                    mvprintw(2, width / 2 - 15, "You have stepped on a trap!");
                }
                rooms[i]->traps[j]->found = true;
                return true;
            }
        }
    }

    return false;
}


Coin* stepped_on_loot(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->coin_count; j++) {
            if (!rooms[i]->coins[j]->claimed && rooms[i]->coins[j]->y == y && rooms[i]->coins[j]->x == x) {
                mvprintw(2, width / 2 - 15, "Loot found! Press C to claim it.");
                return rooms[i]->coins[j];
            }
        }
    }

    return NULL;
}


Spell* stepped_on_spell(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->spell_count; j++) {
            if (!rooms[i]->spells[j]->claimed && rooms[i]->spells[j]->y == y && rooms[i]->spells[j]->x == x) {
                char* type = (char*) malloc(10 * sizeof(char));
                if (rooms[i]->spells[j]->type == 'H') {
                    type = "Health";
                }
                else if (rooms[i]->spells[j]->type == 'S') {
                    type = "Speed";
                }
                else {
                    type = "Damage";
                }
                mvprintw(2, width / 2 - 18, "%s spell found! Press C to claim it.", type);
                return rooms[i]->spells[j];
            }
        }
    }

    return NULL;
}


Food* stepped_on_food(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->food_count; j++) {
            if (!rooms[i]->food[j]->claimed && rooms[i]->food[j]->y == y && rooms[i]->food[j]->x == x) {
                char* type = (char*) malloc(10 * sizeof(char));
                if (rooms[i]->food[j]->type == 'N') {
                    type = "Normal";
                }
                else if (rooms[i]->food[j]->type == 'S') {
                    type = "Special";
                }
                else {
                    type = "Magic";
                }
                mvprintw(2, width / 2 - 15, "%s food found! Press C to claim it.", type);
                return rooms[i]->food[j];
            }
        }
    }

    return NULL;
}


Weapon* stepped_on_weapon(Room** rooms, int y, int x, int width) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        for (int j = 0; j < rooms[i]->weapon_count; j++) {
            if (!rooms[i]->weapons[j]->claimed && rooms[i]->weapons[j]->y == y && rooms[i]->weapons[j]->x == x) {
                char* type = (char*) malloc(10 * sizeof(char));
                if (rooms[i]->weapons[j]->type == 'D') {
                    type = "Dagger";
                }
                else if (rooms[i]->weapons[j]->type == 'S') {
                    type = "Sword";
                }
                else if (rooms[i]->weapons[j]->type == 'W') {
                    type = "Magic wand";
                }
                else {
                    type = "Arrow";
                }
                mvprintw(2, (width + strlen(type)) / 2 - 15, "%s weapon found! Press C to claim it.", type);
                return rooms[i]->weapons[j];
            }
        }
    }

    return NULL;
}


void inventory_menu(Player* player, Backpack* backpack) {
    int height, width;
    getmaxyx(stdscr, height, width);
    attron(COLOR_PAIR(4));
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "INVENTORY");
    attroff(COLOR_PAIR(4));
    for (int i = 0; i < backpack->count_spells; i++) {
        if (backpack->spells[i]->type == 'H') {
            mvprintw(1, width / 2 + i * 23 - 15, "H for Health\U0001F496 (%d)", backpack->spells[i]->amount);
        }
        else if (backpack->spells[i]->type == 'D') {
            mvprintw(1, width / 2 + i * 23 - 15, "D for Damage\U0001F9BE (%d)", backpack->spells[i]->amount);
        }
        else {
            mvprintw(1, width / 2 + i * 23 - 15, "F for Speed\u26A1 (%d)", backpack->spells[i]->amount);
        }
    }

    for (int i = 0; i < backpack->count_food; i++) {
        if (backpack->food[i]->type == 'N') {
            mvprintw(2, width / 2 + i * 23 - 15, "N for Normal\U0001F36B (%d)", backpack->food[i]->amount);
        }
        else if (backpack->food[i]->type == 'S') {
            mvprintw(2, width / 2 + i * 23 - 15, "S for Special\U0001F35F (%d)", backpack->food[i]->amount);
        }
        else {
            mvprintw(2, width / 2 + i * 23 - 15, "M for Magic\U0001F354 (%d)", backpack->food[i]->amount);
        }
    }

    char command;
    while ((command = getch()) != 'q') {
        switch (command) {
        case 'h':
            for (int i = 0; i < backpack->count_spells; i++) {
                if (backpack->spells[i]->type == 'H') {
                    backpack->default_spell = backpack->spells[i];
                    break;
                }
            }
            break;
        case 'd':
            for (int i = 0; i < backpack->count_spells; i++) {
                if (backpack->spells[i]->type == 'D') {
                    backpack->default_spell = backpack->spells[i];
                    break;
                }
            }
            break;
        case 'f':
            for (int i = 0; i < backpack->count_spells; i++) {
                if (backpack->spells[i]->type == 'S') {
                    backpack->default_spell = backpack->spells[i];
                    break;
                }
            }
            break;
        case 'n':
            for (int i = 0; i < backpack->count_food; i++) {
                if (backpack->food[i]->type == 'N') {
                    backpack->default_food = backpack->food[i];
                    break;
                }
            }
            break;
        case 's':
            for (int i = 0; i < backpack->count_food; i++) {
                if (backpack->food[i]->type == 'S') {
                    backpack->default_food = backpack->food[i];
                    break;
                }
            }
            break;
        case 'm':
            for (int i = 0; i < backpack->count_food; i++) {
                if (backpack->food[i]->type == 'M') {
                    backpack->default_food = backpack->food[i];
                    break;
                }
            }
            break;
        default:
            break;
        }
        show_defaults(player, backpack);
        usleep(10000);
    }

    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "Messages"); 
    attron(A_UNDERLINE);
    mvaddch(0, (width - 15 + strlen(player->username)) / 2 + 14 + strlen(player->username), '-');
    attroff(A_UNDERLINE);   

    move(1, width / 2 - 15);
    clrtoeol();
    move(2, width / 2 - 15);
    clrtoeol();
    mvaddch(1, width - 1, '|');
    mvaddch(2, width - 1, '|');
}


void weapon_menu(Player* player, Backpack* backpack) {
    int height, width;
    getmaxyx(stdscr, height, width);
    attron(COLOR_PAIR(4));
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "WEAPONS");
    attroff(COLOR_PAIR(4));
    attron(A_UNDERLINE);
    printw("-");
    attroff(A_UNDERLINE);
    for (int i = 0; i < backpack->count_weapons; i++) {
        if (i <= 2) {
            if (backpack->weapons[i]->type == 'M') {
                mvprintw(1, width / 2 + i * 25 - 15, "M for Mace\u2692");
            }
            else if (backpack->weapons[i]->type == 'S') {
                mvprintw(1, width / 2 + i * 25 - 15, "S for Sword\u2694");
            }
            else if (backpack->weapons[i]->type == 'A') {
                mvprintw(1, width / 2 + i * 25 - 15, "A for Arrow\U0001F3F9 (%d)", backpack->weapons[i]->ammo);
            }
            else if (backpack->weapons[i]->type == 'W') {
                mvprintw(1, width / 2 + i * 25 - 15, "W for Magic wand\U0001FA84 (%d)", backpack->weapons[i]->ammo);
            }
            else {
                mvprintw(1, width / 2 + i * 25 - 15, "D for Dagger\U0001F5E1 (%d)", backpack->weapons[i]->ammo);
            }
        }
        else {
            if (backpack->weapons[i]->type == 'M') {
                mvprintw(2, width / 2 + (i - 3) * 25 - 15, "M for Mace\u2692");
            }
            else if (backpack->weapons[i]->type == 'S') {
                mvprintw(2, width / 2 + (i - 3) * 25 - 15, "S for Sword\u2694");
            }
            else if (backpack->weapons[i]->type == 'A') {
                mvprintw(2, width / 2 + (i - 3) * 25 - 15, "A for Arrow\U0001F3F9 (%d)", backpack->weapons[i]->ammo);
            }
            else if (backpack->weapons[i]->type == 'W') {
                mvprintw(2, width / 2 + (i - 3) * 25 - 15, "W for Magic wand\U0001FA84 (%d)", backpack->weapons[i]->ammo);
            }
            else {
                mvprintw(2, width / 2 + (i - 3) * 25 - 15, "D for Dagger\U0001F5E1 (%d)", backpack->weapons[i]->ammo);
            }
        }
    }

    char command;
    while ((command = getch()) != 'q') {
        switch (command) {
        case 'm':
            for (int i = 0; i < backpack->count_weapons; i++) {
                if (backpack->weapons[i]->type == 'M') {
                    backpack->default_weapon = backpack->weapons[i];
                    break;
                }
            }
            break;
        case 's':
            for (int i = 0; i < backpack->count_weapons; i++) {
                if (backpack->weapons[i]->type == 'S') {
                    backpack->default_weapon = backpack->weapons[i];
                    break;
                }
            }
            break;
        case 'a':
            for (int i = 0; i < backpack->count_weapons; i++) {
                if (backpack->weapons[i]->type == 'A') {
                    backpack->default_weapon = backpack->weapons[i];
                    break;
                }
            }
            break;
        case 'w':
            for (int i = 0; i < backpack->count_weapons; i++) {
                if (backpack->weapons[i]->type == 'W') {
                    backpack->default_weapon = backpack->weapons[i];
                    break;
                }
            }
            break;
        case 'd':
            for (int i = 0; i < backpack->count_weapons; i++) {
                if (backpack->weapons[i]->type == 'D') {
                    backpack->default_weapon = backpack->weapons[i];
                    break;
                }
            }
            break;
        default:
            break;
        }
        show_defaults(player, backpack);
        usleep(10000);
    }

    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "Messages");    
    move(1, width / 2 - 15);
    clrtoeol();
    move(2, width / 2 - 15);
    clrtoeol();
    mvaddch(1, width - 1, '|');
    mvaddch(2, width - 1, '|');
}


void show_defaults(Player* player, Backpack* backpack) {
    if (backpack->default_weapon->type == 'M') {
        mvprintw(2, 16 + strlen(player->username), "\u2692");
    }
    else if (backpack->default_weapon->type == 'S') {
        mvprintw(2, 16 + strlen(player->username), "\u2694");
    }
    else if (backpack->default_weapon->type == 'A') {
        mvprintw(2, 16 + strlen(player->username), "\U0001F3F9");
    }
    else if (backpack->default_weapon->type == 'W') {
        mvprintw(2, 16 + strlen(player->username), "\U0001FA84");
    }
    else {
        mvprintw(2, 16 + strlen(player->username), "\U0001F5E1");
    }

    if (backpack->count_food != 0 && backpack->count_spells != 0) {
        if (backpack->default_spell->type == 'H') {
            mvprintw(2, 18 + strlen(player->username), "\U0001F496");
        }
        else if (backpack->default_spell->type == 'D') {
            mvprintw(2, 18 + strlen(player->username), "\U0001F9BE");
        }
        else {
            mvprintw(2, 18 + strlen(player->username), "\u26A1");
        }

        if (backpack->default_food->type == 'N') {
            mvprintw(2, 20 + strlen(player->username), "\U0001F36B");
        }
        else if (backpack->default_food->type == 'S') {
            mvprintw(2, 20 + strlen(player->username), "\U0001F35F");
        }
        else {
            mvprintw(2, 20 + strlen(player->username), "\U0001F354");
        }
    }
    else if (backpack->count_food == 0 && backpack->count_spells != 0) {
        if (backpack->default_spell->type == 'H') {
            mvprintw(2, 18 + strlen(player->username), "\U0001F496");
        }
        else if (backpack->default_spell->type == 'D') {
            mvprintw(2, 18 + strlen(player->username), "\U0001F9BE");
        }
        else {
            mvprintw(2, 18 + strlen(player->username), "\u26A1");
        }
    }
    else if (backpack->count_food != 0 && backpack->count_spells == 0) {
        if (backpack->default_food->type == 'N') {
            mvprintw(2, 20 + strlen(player->username), "\U0001F36B");
        }
        else if (backpack->default_food->type == 'S') {
            mvprintw(2, 20 + strlen(player->username), "\U0001F35F");
        }
        else {
            mvprintw(2, 20 + strlen(player->username), "\U0001F354");
        }
    }
}


void show_health_bar(Player* player) {
    mvprintw(1, 16 + strlen(player->username), "\u2764");
    if (player->hp <= 30) {
        attron(COLOR_PAIR(4));
    }
    else if (player->hp <= 70) {
        attron(COLOR_PAIR(3));
    }
    else {
        attron(COLOR_PAIR(2));
    }
    mvprintw(1, 18 + strlen(player->username), "╠");
    for (int i = 0; i < player->hp / 10; i++) {
        mvprintw(1, 19 + strlen(player->username) + i, "▌");
    }

    for (int i = player->hp / 10; i < 10; i++) {
        mvprintw(1, 19 + strlen(player->username) + i, "-"); 
    }

    mvprintw(1, 29 + strlen(player->username), "╣");
    mvprintw(1, 31 + strlen(player->username), "%d", player->hp);
    if (player->hp != 100) {
        mvprintw(1, 33 + strlen(player->username), " ");
    }
    
    if (player->hp < 10) {
        mvprintw(1, 32 + strlen(player->username), " ");
    }

    if (player->hp <= 30) {
        attroff(COLOR_PAIR(4));
    }
    else if (player->hp <= 70) {
        attroff(COLOR_PAIR(3));
    }
    else {
        attroff(COLOR_PAIR(2));
    }
    refresh();
}


void show_hunger_bar(Player* player) {
    mvprintw(3, 16 + strlen(player->username), "\U0001F357");
    attron(COLOR_PAIR(20));
    mvprintw(3, 18 + strlen(player->username), "╠");
    for (int i = 0; i < player->hunger; i++) {
        mvprintw(3, 19 + strlen(player->username) + i, "▌");
    }

    for (int i = player->hunger; i < 5; i++) {
        mvprintw(3, 19 + strlen(player->username) + i, "-"); 
    }

    mvprintw(3, 24 + strlen(player->username), "╣");
    mvprintw(3, 26 + strlen(player->username), "%d", player->hunger);
    attroff(COLOR_PAIR(20));
    refresh();
}


long get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000L + tv.tv_usec;
}


void death(int width) {
    attron(COLOR_PAIR(9));
    mvprintw(2, width / 2 - 16, "You died! Press ANY KEY to quit.");
    attroff(COLOR_PAIR(9));
    getch();
}


Room* get_current_room(Room** rooms, int y, int x) {
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        if (x > rooms[i]->corner_x && x < rooms[i]->corner_x + rooms[i]->width - 1 && y > rooms[i]->corner_y && y < rooms[i]->corner_y + rooms[i]->height - 1) {
            rooms[i]->visited = true;
            return rooms[i];
        }
    }

    return NULL;
}


void show_game_bar(Player* player, Backpack* backpack, int claimed_gold) {
    move(1, 34 + strlen(player->username));
    clrtoeol();
    move(2, 34 + strlen(player->username));
    clrtoeol();
    move(3, 34 + strlen(player->username));
    clrtoeol();

    int height, width;
    getmaxyx(stdscr, height, width);
    for (int i = 1; i < 4; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, width - 1, "|");
    }

    for (int i = 0; i < width; i++) {
        attron(A_UNDERLINE);
        mvaddch(0, i, '-');
        mvaddch(4, i, '-');
        attroff(A_UNDERLINE);
    }

    for (int i = 1; i < 4; i++) {
        mvprintw(i, 15 + strlen(player->username), "|");
    }

    mvprintw(0, strlen(player->username) + 15 / 2 - 5, "Stats");
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "Messages");

    char* new_gold = (char*) malloc(10 * sizeof(char));
    char* prev_gold = (char*) malloc(10 * sizeof(char));
    sprintf(prev_gold, "%d", player->gold);
    sprintf(new_gold, "%d", claimed_gold);
    mvprintw(1, 2, "Your name: %s", player->username);
    attron(COLOR_PAIR(3));
    mvprintw(2, 2, "Gold earned: %s", new_gold);
    mvprintw(3, 2, "Total gold: %s", prev_gold); 
    attroff(COLOR_PAIR(3));

    if (player->fast_paced) {
        mvprintw(1, strlen(player->username) + 13, "\u26A1");
    }
    show_defaults(player, backpack);  
    show_health_bar(player);
    show_hunger_bar(player); 
}


bool show_pause_menu(Player* player, int width) {
    attron(A_UNDERLINE);
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "-");
    attroff(A_UNDERLINE);
    attron(COLOR_PAIR(4));
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 7 + strlen(player->username), "PAUSE");
    attroff(COLOR_PAIR(4));
    attron(A_UNDERLINE);
    printw("-");
    printw("-");
    attroff(A_UNDERLINE);
    mvprintw(1, width / 2 - 8, "Press R to resume");
    mvprintw(2, width / 2 - 8, "Press S to save and leave");
    mvprintw(3, width / 2 - 8, "Press O to open settings");

    char command;
    while ((command = getch()) != 'r') {
        switch (command) {
        case 's':
            return true;
            break;
        case 'o':
            show_options_menu(player, width);
            break;
        default:
            break;
        }
        usleep(10000);
    }

    return false;
}


void show_options_menu(Player* player, int width) {
    attron(COLOR_PAIR(4));
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "OPTIONS");
    attroff(COLOR_PAIR(4));
    attron(A_UNDERLINE);
    printw("-");
    printw("-");
    attroff(A_UNDERLINE);
    mvprintw(1, width / 2 - 8, "Difficulty:\tE for Easy | M for Medium | H for Hard");
    mvprintw(2, width / 2 - 8, "Hero:      \t1.\u265F | 2.\u265E | 3.\u265D | 4.\u265C");
    mvprintw(3, width / 2 - 8, "Hero color:\tW:\u26AA | B:\U0001F535 | R:\U0001F534 | G:\U0001F7E2 | Y:\U0001F7E1");

    char command;
    while ((command = getch()) != 'q') {
        switch (command) {
        case 'e':
            player->difficulty = "easy";
            player->difficulty_coeff = 1;
            break;
        case 'm':
            player->difficulty = "medium";
            player->difficulty_coeff = 2;
            break;
        case 'h':
            player->difficulty = "hard";
            player->difficulty_coeff = 3;
            break;
        case '1':
            player->hero = "\u265F";
            mvprintw(player->y, player->x, "%s", player->hero);
            break;
        case '2':
            player->hero = "\u265E";
            mvprintw(player->y, player->x, "%s", player->hero);
            break;
        case '3':
            player->hero = "\u265D";
            mvprintw(player->y, player->x, "%s", player->hero);
            break;
        case '4':
            player->hero = "\u265C";
            mvprintw(player->y, player->x, "%s", player->hero);
            break;
        case 'w':
            player->color = "white";
            mvprintw(player->y, player->x, "%s", player->hero);
            break;
        case 'b':
            player->color = "blue";
            attron(COLOR_PAIR(1));
            mvprintw(player->y, player->x, "%s", player->hero);
            attroff(COLOR_PAIR(1));
            break;
        case 'r':
            player->color = "red";
            attron(COLOR_PAIR(4));
            mvprintw(player->y, player->x, "%s", player->hero);
            attroff(COLOR_PAIR(4));
            break;
        case 'g':
            player->color = "green";
            attron(COLOR_PAIR(2));
            mvprintw(player->y, player->x, "%s", player->hero);
            attroff(COLOR_PAIR(2));
            break;
        case 'y':
            player->color = "yellow";
            attron(COLOR_PAIR(3));
            mvprintw(player->y, player->x, "%s", player->hero);
            attroff(COLOR_PAIR(3));
            break;
        default:
            break;
        }
        usleep(10000);
    }
    move(1, width / 2 - 8);
    clrtoeol();
    move(2, width / 2 - 8);
    clrtoeol();
    move(3, width / 2 - 8);
    clrtoeol();
    mvaddch(1, width - 1, '|');
    mvaddch(2, width - 1, '|');
    mvaddch(3, width - 1, '|');
    
    attron(A_UNDERLINE);
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 6 + strlen(player->username), "-");
    attroff(A_UNDERLINE);
    attron(COLOR_PAIR(4));
    mvprintw(0, (width - 15 + strlen(player->username)) / 2 + 7 + strlen(player->username), "PAUSE");
    attroff(COLOR_PAIR(4));
    attron(A_UNDERLINE);
    printw("-");
    printw("-");
    attroff(A_UNDERLINE);
    mvprintw(1, width / 2 - 8, "Press R to resume");
    mvprintw(2, width / 2 - 8, "Press S to save and leave");
    mvprintw(3, width / 2 - 8, "Press O to open settings");
}


Monster** battle_room(Player* player, Backpack* backpack, int claimed_gold, int num_monsters, int* current_y, int* current_x) {
    clear();
    show_game_bar(player, backpack, claimed_gold);
    int room_corner_y = 8;
    int room_corner_x = 10;
    int room_height = 18;
    int room_width = 60;
    for (int i = 1; i < room_height; i++) {
        mvprintw(room_corner_y + i, room_corner_x, "|");
        mvprintw(room_corner_y + i, room_corner_x + room_width, "|");
    }
    for (int i = 0; i <= room_width; i++) {
        attron(COLOR_PAIR(4) | A_UNDERLINE);
        mvprintw(room_corner_y, room_corner_x + i, "-");
        mvprintw(room_corner_y + room_height, room_corner_x + i, "-");
        attroff(COLOR_PAIR(4) | A_UNDERLINE);
    }
    for (int i = 1; i < room_height; i++) {
        for (int j = 1; j < room_width; j++) {
            mvprintw(room_corner_y + i, room_corner_x + j, ".");
        }
    }
    move_player(player, room_corner_y + 1, room_corner_x + 1);
    player->y = room_corner_y + 1;
    player->x = room_corner_x + 1;
    *current_x = room_corner_x + 1;
    *current_y = room_corner_y + 1;

    Monster** monsters = (Monster**) calloc(num_monsters, sizeof(Monster*));
    bool valid_position = false;
    for (int i = 0; i < num_monsters; i++) {
        monsters[i] = (Monster*) malloc(sizeof(Monster));
        monsters[i]->y = room_corner_y + 2 + rand() % (room_height - 4);
        monsters[i]->x = room_corner_x + 2 + rand() % (room_width - 4);
        int prob = rand() % 10 + 1;
        if (prob <= 1) {
            monsters[i]->type = 'D';
            monsters[i]->damage = 2;
            monsters[i]->hp = 5;
            monsters[i]->alive = true;
            monsters[i]->range = 1;
        }
        else if (prob <= 3) {
            monsters[i]->type = 'F';
            monsters[i]->damage = 4;
            monsters[i]->hp = 10;
            monsters[i]->alive = true;
            monsters[i]->range = 1;
        }
        else if (prob <= 6) {
            monsters[i]->type = 'G';
            monsters[i]->damage = 6;
            monsters[i]->hp = 15;
            monsters[i]->alive = true;
            monsters[i]->range = 5;
        }
        else if (prob <= 8) {
            monsters[i]->type = 'S';
            monsters[i]->damage = 8;
            monsters[i]->hp = 20;
            monsters[i]->alive = true;
            monsters[i]->range = 1000;
        }
        else {
            monsters[i]->type = 'U';
            monsters[i]->damage = 10;
            monsters[i]->hp = 30;
            monsters[i]->alive = true;
            monsters[i]->range = 5;
        }

        attron(COLOR_PAIR(4));
        mvaddch(monsters[i]->y, monsters[i]->x, monsters[i]->type);
        attroff(COLOR_PAIR(4));
    }

    return monsters;
}


void show_battle_bar(Backpack* backpack, int kills) {
    int height, width;
    getmaxyx(stdscr, height, width);
    for (int i = 5; i < height - 4; i++) {
        mvprintw(i, width - 20, "|");
        mvprintw(i, width - 1, "|");
    }
    for (int i = 20; i > 0; i--) {
        attron(A_UNDERLINE);
        mvprintw(height - 4, width - i, "-");
        mvprintw(14, width - i, "-");
        mvprintw(19, width - i, "-");
        attroff(A_UNDERLINE);
    }

    attron(COLOR_PAIR(4));
    mvprintw(6, width - 18, "Total kills: %d", kills);
    attroff(COLOR_PAIR(4));

    for (int i = 0; i < backpack->count_weapons; i++) {
        if (backpack->weapons[i]->type == 'M') {
            mvprintw(8 + i, width - 18, "%d.\u2692 Mace", i + 1);
        }
        else if (backpack->weapons[i]->type == 'S') {
            mvprintw(8 + i, width - 18, "%d.\u2694 Sword", i + 1);
        }
        else if (backpack->weapons[i]->type == 'A') {
            mvprintw(8 + i, width - 18, "%d.\U0001F3F9 Arrow (%d)", i + 1, backpack->weapons[i]->ammo);
        }
        else if (backpack->weapons[i]->type == 'D') {
            mvprintw(8 + i, width - 18, "%d.\U0001F5E1 Dagger (%d)", i + 1, backpack->weapons[i]->ammo);
        }
        else if (backpack->weapons[i]->type == 'W') {
            mvprintw(8 + i, width - 18, "%d.\U0001FA84 Magic Wand (%d)", i + 1, backpack->weapons[i]->ammo);
        }
    }

    for (int i = 0; i < backpack->count_spells; i++) {
        if (backpack->spells[i]->type == 'H') {
            mvprintw(16 + i, width - 18, "%d.\U0001F496 Health spell (%d)", i + 1, backpack->spells[i]->amount);
        }
        else if (backpack->spells[i]->type == 'S') {
            mvprintw(16 + i, width - 18, "%d.\u26A1 Speed spell (%d)", i + 1, backpack->spells[i]->amount);
        }
        else {
            mvprintw(16 + i, width - 18, "%d.\u2620 Damage spell (%d)", i + 1, backpack->spells[i]->amount);
        }
    }

    for (int i = 0; i < backpack->count_food; i++) {
        if (backpack->food[i]->type == 'N') {
            mvprintw(21 + i, width - 18, "%d.\U0001F36B Normal food (%d)", i + 1, backpack->food[i]->amount);
        }
        else if (backpack->food[i]->type == 'S') {
            mvprintw(21 + i, width - 18, "%d.\U0001F35F Special food (%d)", i + 1, backpack->food[i]->amount);
        }
        else {
            mvprintw(21 + i, width - 18, "%d.\U0001F354 Magic food (%d)", i + 1, backpack->food[i]->amount);
        }
    }
}


void monster_movments(Monster** monsters, Player* player, int num_monsters) {
    static long updates = 0;
    for (int i = 0; i < num_monsters; i++) {
        if (monsters[i]->type == 'G') {
            if (distance(player->x, player->y, monsters[i]->x, monsters[i]->y) <= monsters[i]->range) {
                if (player->x == monsters[i]->x) {
                    if (player->y > monsters[i]->y && mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y++;
                    }
                    else if (player->y < monsters[i]->y && mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y--;
                    }
                }
                else if (player->y == monsters[i]->y) {
                    if (player->x > monsters[i]->x && mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->x++;
                    }
                    else if (player->x < monsters[i]->x && mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y--;
                    }
                }
                else {
                    if (player->y > monsters[i]->y && player->x > monsters[i]->x) {
                        if (mvinch(monsters[i]->y + 1, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                        }
                    }
                    else if (player->y < monsters[i]->y && player->x > monsters[i]->x) {
                        if (mvinch(monsters[i]->y - 1, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                        }
                    }
                    else if (player->y > monsters[i]->y && player->x < monsters[i]->x) {
                        if (mvinch(monsters[i]->y + 1, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                        }
                    }
                    else {
                        if (mvinch(monsters[i]->y - 1, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                        }
                    }
                }
            }
        }
        else if (monsters[i]->type == 'S') {
            if (!(updates % 2) && distance(player->x, player->y, monsters[i]->x, monsters[i]->y) <= monsters[i]->range) {
                if (player->x == monsters[i]->x) {
                    if (player->y > monsters[i]->y && mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y++;
                    }
                    else if (player->y < monsters[i]->y && mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y--;
                    }
                }
                else if (player->y == monsters[i]->y) {
                    if (player->x > monsters[i]->x && mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->x++;
                    }
                    else if (player->x < monsters[i]->x && mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y--;
                    }
                }
                else {
                    if (player->y > monsters[i]->y && player->x > monsters[i]->x) {
                        if (mvinch(monsters[i]->y + 1, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                        }
                    }
                    else if (player->y < monsters[i]->y && player->x > monsters[i]->x) {
                        if (mvinch(monsters[i]->y - 1, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                        }
                    }
                    else if (player->y > monsters[i]->y && player->x < monsters[i]->x) {
                        if (mvinch(monsters[i]->y + 1, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                        }
                    }
                    else {
                        if (mvinch(monsters[i]->y - 1, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                        }
                    }
                }
            }
        }
        else if (monsters[i]->type == 'U') {
            if (distance(player->x, player->y, monsters[i]->x, monsters[i]->y) <= monsters[i]->range) {
                if (player->x == monsters[i]->x) {
                    if (player->y > monsters[i]->y && mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y++;
                    }
                    else if (player->y < monsters[i]->y && mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y--;
                    }
                }
                else if (player->y == monsters[i]->y) {
                    if (player->x > monsters[i]->x && mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->x++;
                    }
                    else if (player->x < monsters[i]->x && mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                        mvaddch(monsters[i]->y, monsters[i]->x, '.');
                        attron(COLOR_PAIR(4));
                        mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                        attroff(COLOR_PAIR(4));
                        monsters[i]->y--;
                    }
                }
                else {
                    if (player->y > monsters[i]->y && player->x > monsters[i]->x) {
                        if (mvinch(monsters[i]->y + 1, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                        }
                    }
                    else if (player->y < monsters[i]->y && player->x > monsters[i]->x) {
                        if (mvinch(monsters[i]->y - 1, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x + 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x + 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x++;
                        }
                        else if (mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                        }
                    }
                    else if (player->y > monsters[i]->y && player->x < monsters[i]->x) {
                        if (mvinch(monsters[i]->y + 1, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y + 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y + 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y++;
                        }
                    }
                    else {
                        if (mvinch(monsters[i]->y - 1, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y, monsters[i]->x - 1) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y, monsters[i]->x - 1, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->x--;
                        }
                        else if (mvinch(monsters[i]->y - 1, monsters[i]->x) == '.') {
                            mvaddch(monsters[i]->y, monsters[i]->x, '.');
                            attron(COLOR_PAIR(4));
                            mvaddch(monsters[i]->y - 1, monsters[i]->x, monsters[i]->type);
                            attroff(COLOR_PAIR(4));
                            monsters[i]->y--;
                        }
                    }
                }
            }
        }
    }

    updates++;
}


int distance(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}


void monsters_damage(Monster** monsters, Player* player, int num_monsters) {
    int height, width;
    getmaxyx(stdscr, height, width);
    for (int i = 0; i < num_monsters; i++) {
        if (distance(player->x, player->y, monsters[i]->x, monsters[i]->y) <= 1) {
            player->hp -= monsters[i]->damage * player->difficulty_coeff;
            mvprintw(2, width / 2 - 15, "Run! Monsters are eating you!");
        }
    }
}


Monster* find_monster(Monster** monsters, Player* player, char direction, int num_monsters) {
    if (monsters == NULL) {
        return NULL;
    }
    bool found_monster = false;
    Monster* found = (Monster*) malloc(sizeof(Monster));
    int min_distance = 100000;
    if (direction == '4') {
        for (int i = 0; i < num_monsters; i++) {
            if (monsters[i]->x < player->x && distance(player->x, player->y, monsters[i]->x, monsters[i]->y) < min_distance) {
                min_distance = distance(player->x, player->y, monsters[i]->x, monsters[i]->y);
                found = monsters[i];
                found_monster = true;
            } 
        }
    }
    else if (direction == '8') {
        for (int i = 0; i < num_monsters; i++) {
            if (monsters[i]->y < player->y && distance(player->x, player->y, monsters[i]->x, monsters[i]->y) < min_distance) {
                min_distance = distance(player->x, player->y, monsters[i]->x, monsters[i]->y);
                found = monsters[i];
                found_monster = true;
            } 
        }
    }
    else if (direction == '6') {
        for (int i = 0; i < num_monsters; i++) {
            if (monsters[i]->x > player->x && distance(player->x, player->y, monsters[i]->x, monsters[i]->y) < min_distance) {
                min_distance = distance(player->x, player->y, monsters[i]->x, monsters[i]->y);
                found = monsters[i];
                found_monster = true;
            } 
        }
    }
    else if (direction == '2') {
        for (int i = 0; i < num_monsters; i++) {
            if (monsters[i]->y > player->y && distance(player->x, player->y, monsters[i]->x, monsters[i]->y) < min_distance) {
                min_distance = distance(player->x, player->y, monsters[i]->x, monsters[i]->y);
                found = monsters[i];
                found_monster = true;
            } 
        }
    }

    if (found_monster)
        return found;

    return NULL;
}