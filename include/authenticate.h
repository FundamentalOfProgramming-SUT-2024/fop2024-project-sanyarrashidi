#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<ctype.h>


typedef struct {
    char* username;
    char* password;
    char* email;
    int score;
    int gold;
    int finished;
    int exp;
} Player;


Player* authenticate(Player*);
Player* new_user(Player*, int, int);
Player* login(FILE*, Player*, int, int);
int check_username(char*, int, int);
int check_pass(char*, int, int);
void clear_pass_errors();
int check_email(char*, int, int);
void get_player_stat(Player*);