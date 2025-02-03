#include "database.h"


void save_rooms(Room** rooms, Player* player, int level) {
    chdir("data");
    chdir(player->username);
    char* name = (char*) calloc(10, sizeof(char));
    sprintf(name, "room%d.txt", level);
    FILE* to_write_file = fopen(name, "w");
    fprintf(to_write_file, "%d\n", rooms[0]->total_rooms);
    for (int i = 0; i < rooms[0]->total_rooms; i++) {
        fprintf(to_write_file, "%d,%d,%d,%d,\n", rooms[i]->corner_x, rooms[i]->corner_y, rooms[i]->height, rooms[i]->width);
        
        fprintf(to_write_file, "%d\n", rooms[i]->door_count);
        for (int j = 0; j < rooms[i]->door_count; j++) {
            fprintf(to_write_file, "%d,%d,\n", rooms[i]->doors_x[j], rooms[i]->doors_y[j]);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->pillar_count);
        for (int j = 0; j < rooms[i]->pillar_count; j++) {
            fprintf(to_write_file, "%d,%d,\n", rooms[i]->pillars_x[j], rooms[i]->pillars_y[j]);
        }
    
        if (rooms[i]->door_count == 3) {
            fprintf(to_write_file, "h\n%d,%d,\n", rooms[i]->hidden_x[0], rooms[i]->hidden_y[0]);
        }

        fprintf(to_write_file, "%c\n", rooms[i]->type);

        fprintf(to_write_file, "%d\n", rooms[i]->trap_count);
        for (int j = 0; j < rooms[i]->trap_count; j++) {
            fprintf(to_write_file, "%d,%d,%d,\n", rooms[i]->traps[j]->x, rooms[i]->traps[j]->y, rooms[i]->traps[j]->found ? 1 : 0);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->coin_count);
        for (int j = 0; j < rooms[i]->coin_count; j++) {
            fprintf(to_write_file, "%d,%d,%d,\n", rooms[i]->coins[j]->x, rooms[i]->coins[j]->y, rooms[i]->coins[j]->claimed ? 1 : 0);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->weapon_count);
        for (int j = 0; j < rooms[i]->weapon_count; j++) {
            fprintf(to_write_file, "%c,%d,%d,%d,%d,%d,\n", rooms[i]->weapons[j]->type, rooms[i]->weapons[j]->damage, rooms[i]->weapons[j]->ammo, rooms[i]->weapons[j]->x, rooms[i]->weapons[j]->y, rooms[i]->weapons[j]->claimed ? 1 : 0);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->spell_count);
        for (int j = 0; j < rooms[i]->spell_count; j++) {
            fprintf(to_write_file, "%c,%d,%d,%d,%d,\n", rooms[i]->spells[j]->type, rooms[i]->spells[j]->amount, rooms[i]->spells[j]->x, rooms[i]->spells[j]->y, rooms[i]->spells[j]->claimed ? 1 : 0);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->food_count);
        for (int j = 0; j < rooms[i]->food_count; j++) {
            fprintf(to_write_file, "%c,%d,%d,%d,%d,\n", rooms[i]->food[j]->type, rooms[i]->food[j]->amount, rooms[i]->food[j]->x, rooms[i]->food[j]->y, rooms[i]->food[j]->claimed ? 1 : 0);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->monster_count);
        for (int j = 0; j < rooms[i]->monster_count; j++) {
            fprintf(to_write_file, "%c,%d,%d,%d,%d,%d,\n", rooms[i]->monsters[j]->type, rooms[i]->monsters[j]->x, rooms[i]->monsters[j]->y, rooms[i]->monsters[j]->hp, rooms[i]->monsters[j]->damage, rooms[i]->monsters[j]->alive ? 1 : 0);
        }

        fprintf(to_write_file, "%d\n", rooms[i]->visited ? 1 : 0);
    }

    fclose(to_write_file);
    chdir("..");
    chdir("..");
}


Room** read_rooms(Player* player, int level) {
    chdir("data");
    chdir(player->username);
    char* name = (char*) calloc(10, sizeof(char));
    sprintf(name, "room%d.txt", level);
    FILE* data_file = fopen(name, "r");
    char* read_line = (char*) calloc(100, sizeof(char));
    fscanf(data_file, "%s", read_line);
    int total_rooms = atoi(read_line);
    Room** rooms = (Room**) calloc(total_rooms, sizeof(Room*));
    for (int i = 0; i < total_rooms; i++) {
        rooms[i] = (Room*) calloc(1, sizeof(Room));
        rooms[i]->total_rooms = total_rooms;
        fscanf(data_file, "%s", read_line);
        sscanf(read_line, "%d,%d,%d,%d,", &rooms[i]->corner_x, &rooms[i]->corner_y, &rooms[i]->height, &rooms[i]->width);
    
        fscanf(data_file, "%s", read_line);
        rooms[i]->door_count = atoi(read_line);
        rooms[i]->doors_x = (int*) calloc(rooms[i]->door_count, sizeof(int));
        rooms[i]->doors_y = (int*) calloc(rooms[i]->door_count, sizeof(int));
        for (int j = 0; j < rooms[i]->door_count; j++) {
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%d,%d,", &rooms[i]->doors_x[j], &rooms[i]->doors_y[j]);
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->pillar_count = atoi(read_line);
        rooms[i]->pillars_x = (int*) calloc(rooms[i]->pillar_count, sizeof(int));
        rooms[i]->pillars_y = (int*) calloc(rooms[i]->pillar_count, sizeof(int));
        for (int j = 0; j < rooms[i]->pillar_count; j++) {
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%d,%d,", &rooms[i]->pillars_x[j], &rooms[i]->pillars_y[j]);
        }

        fscanf(data_file, "%s", read_line);
        if (read_line[0] == 'h') {
            fscanf(data_file, "%s", read_line);
            rooms[i]->hidden_x = (int*) calloc(1, sizeof(int));
            rooms[i]->hidden_y = (int*) calloc(1, sizeof(int));
            sscanf(read_line, "%d,%d,", &rooms[i]->hidden_x[0], &rooms[i]->hidden_y[0]);
            fscanf(data_file, "%s", read_line);
            rooms[i]->type = read_line[0];
        }
        else {
            rooms[i]->type = read_line[0];
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->trap_count = atoi(read_line);
        rooms[i]->traps = (Trap**) calloc(rooms[i]->trap_count, sizeof(Trap*));
        for (int j = 0; j < rooms[i]->trap_count; j++) {
            int int_to_bool;
            rooms[i]->traps[j] = (Trap*) calloc(1, sizeof(Trap));
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%d,%d,%d,", &rooms[i]->traps[j]->x, &rooms[i]->traps[j]->y, &int_to_bool);
            rooms[i]->traps[j]->found = int_to_bool ? true : false;
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->coin_count = atoi(read_line);
        rooms[i]->coins = (Coin**) calloc(rooms[i]->coin_count, sizeof(Coin*));
        for (int j = 0; j < rooms[i]->coin_count; j++) {
            int int_to_bool;
            rooms[i]->coins[j] = (Coin*) calloc(1, sizeof(Coin));
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%d,%d,%d,", &rooms[i]->coins[j]->x, &rooms[i]->coins[j]->y, &int_to_bool);
            rooms[i]->coins[j]->claimed = int_to_bool ? true : false;
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->weapon_count = atoi(read_line);
        rooms[i]->weapons = (Weapon**) calloc(rooms[i]->weapon_count, sizeof(Weapon*));
        for (int j = 0; j < rooms[i]->weapon_count; j++) {
            int int_to_bool;
            rooms[i]->weapons[j] = (Weapon*) calloc(1, sizeof(Weapon));
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%c,%d,%d,%d,%d,%d,", &rooms[i]->weapons[j]->type, &rooms[i]->weapons[j]->damage, &rooms[i]->weapons[j]->ammo, &rooms[i]->weapons[j]->x, &rooms[i]->weapons[j]->y, &int_to_bool);
            rooms[i]->weapons[j]->claimed = int_to_bool ? true : false;
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->spell_count = atoi(read_line);
        rooms[i]->spells = (Spell**) calloc(rooms[i]->spell_count, sizeof(Spell*));
        for (int j = 0; j < rooms[i]->spell_count; j++) {
            int int_to_bool;
            rooms[i]->spells[j] = (Spell*) calloc(1, sizeof(Spell));
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%c,%d,%d,%d,%d,", &rooms[i]->spells[j]->type, &rooms[i]->spells[j]->amount, &rooms[i]->spells[j]->x, &rooms[i]->spells[j]->y, &int_to_bool);
            rooms[i]->spells[j]->claimed = int_to_bool ? true : false;
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->food_count = atoi(read_line);
        rooms[i]->food = (Food**) calloc(rooms[i]->food_count, sizeof(Food*));
        for (int j = 0; j < rooms[i]->food_count; j++) {
            int int_to_bool;
            rooms[i]->food[j] = (Food*) calloc(1, sizeof(Food));
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%c,%d,%d,%d,%d,", &rooms[i]->food[j]->type, &rooms[i]->food[j]->amount, &rooms[i]->food[j]->x, &rooms[i]->food[j]->y, &int_to_bool);
            rooms[i]->food[j]->claimed = int_to_bool ? true : false;
        }

        fscanf(data_file, "%s", read_line);
        rooms[i]->monster_count = atoi(read_line);
        if (rooms[i]->monster_count > 0) {
            rooms[i]->monsters = (Monster**) calloc(rooms[i]->monster_count, sizeof(Monster*));
        }
        for (int j = 0; j < rooms[i]->monster_count; j++) {
            int int_to_bool;
            rooms[i]->monsters[j] = (Monster*) calloc(1, sizeof(Monster));
            fscanf(data_file, "%s", read_line);
            sscanf(read_line, "%c,%d,%d,%d,%d,%d,", &rooms[i]->monsters[j]->type, &rooms[i]->monsters[j]->x, &rooms[i]->monsters[j]->y, &rooms[i]->monsters[j]->hp, &rooms[i]->monsters[j]->damage, &int_to_bool);
            rooms[i]->monsters[j]->alive = int_to_bool ? true : false;
        }

        fscanf(data_file, "%s", read_line);
        int int_to_bool = atoi(read_line);
        rooms[i]->visited = int_to_bool ? true : false;
    }

    fclose(data_file);
    chdir("..");
    chdir("..");
    return rooms;
}


void save_corridors_to_file(Player* player, char** map, int height, int width, int level, bool final) {
    chdir("data");
    chdir(player->username);
    char* name = (char*) calloc(10, sizeof(char));
    if (final) {
        sprintf(name, "final%d.txt", level);
    }
    else {
        sprintf(name, "corridors%d.txt", level);
    }
    FILE* to_write_file = fopen(name, "w");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(to_write_file, "%c", map[i][j]);
        }
        fprintf(to_write_file, "\n");
    }

    fclose(to_write_file);
    chdir("..");
    chdir("..");
}


char** read_corridors(Player* player, int height, int width, int level, bool final) {
    chdir("data");
    chdir(player->username);
    char* name = (char*) calloc(20, sizeof(char));
    if (final) {
        sprintf(name, "final%d.txt", level);
    }
    else {
        sprintf(name, "corridors%d.txt", level);
    }
    FILE* data_file = fopen(name, "r");
    char** corridors = (char**) calloc(height, sizeof(char*));
    char* read_line = (char*) calloc(width + 2, sizeof(char));
    for (int i = 0; i < height; i++) {
        corridors[i] = (char*) calloc(width, sizeof(char));
        fgets(read_line, width + 2, data_file);
        read_line[strcspn(read_line, "\n")] = '\0';
        for (int j = 0; j < width; j++) {
            corridors[i][j] = read_line[j];
        }
    }

    fclose(data_file);
    chdir("..");
    chdir("..");
    return corridors;
}


void save_player(Player* player) {
    int num_players = 0;
    Player* players = extract_players_stats(&num_players);
    FILE* to_write_file = fopen("data/stats.csv", "w");
    for (int i = 0; i < num_players; i++) {
        if (!strcmp(players[i].username, player->username)) {
            players[i].score = player->score;
            players[i].gold = player->gold;
            players[i].finished = player->finished;
            players[i].hero = player->hero;
            players[i].color = player->color;
            players[i].difficulty = player->difficulty;
            players[i].fast_paced = player->fast_paced;
            players[i].hp = player->hp;
            players[i].hunger = player->hunger;
            players[i].unlocked_levels = player->unlocked_levels;
            players[i].current_level = player->current_level;
            players[i].x = player->x;
            players[i].y = player->y;
        }

        fprintf(to_write_file, "%s,%d,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,\n", 
            players[i].username,
            players[i].score,
            players[i].gold,
            players[i].finished,
            players[i].exp,
            players[i].hero,
            players[i].color,
            players[i].difficulty,
            players[i].fast_paced,
            players[i].hp,
            players[i].hunger,
            players[i].unlocked_levels,
            players[i].current_level,
            players[i].x,
            players[i].y);
        
    }

    fclose(to_write_file);
}