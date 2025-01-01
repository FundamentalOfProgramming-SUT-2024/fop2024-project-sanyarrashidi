#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>


typedef struct {
    char* username;
    char* password;
    char* email;
} Player;


void handle_login();
void new_user(int, int);
void login();
int check_pass(char*, int, int);
void clear_pass_errors();
int check_email(char*, int, int);