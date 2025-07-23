#ifndef USER_H
#define USER_H

#include "Node.h"
#include "Parse_Command.h"

typedef struct User{
    char * username;
    char * password;
}User;

int check_username(char * username);
int register_user(ParsedCommand * command);
User * login_user(ParsedCommand * command);

#endif