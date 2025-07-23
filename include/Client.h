#ifndef CLIENT_H
#define CLIENT_H

#include "User.h"

typedef struct Client{
    User * user;
    int auth;
    int client_fd;
    int client_current_room_id;
    char * client_name;
}Client;

void client_add_list(Client * _client);
void delete_client(Client * _client);
Client * create_client(int client_socket);
Client * check_online_user(char * _username);

#endif