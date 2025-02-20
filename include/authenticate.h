#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/stat.h>


typedef struct {
    char* username;
    char* password;
    char* email;
    char* security;
    int score;
    int gold;
    int finished;
    int exp;
    char* hero;
    char* color;
    char* difficulty;
    int difficulty_coeff;
    int fast_paced;
    int hp;
    int hunger;
    int unlocked_levels;
    int current_level;
    int x;
    int y;
} Player;


Player* authenticate(Player*);
Player* new_user(Player*, int, int);
Player* login(FILE*, Player*, int, int);
int check_username(char*, int, int);
int check_pass(char*, int, int);
void clear_pass_errors();
int check_email(char*, int, int);
void get_player_stat(Player*);
char* generate_password();