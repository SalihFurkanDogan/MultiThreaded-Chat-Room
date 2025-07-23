#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Node.h"
#include "User.h"
#include "Parse_Command.h"
#include "Client.h"
#include "Room.h"

typedef struct Server{
    int opt;
    int server_fd;
    char * server_name;
    socklen_t addrlen;
    struct sockaddr_in address;
    Node * first_client;
    Node * first_user;
    Node * rooms;
}Server;
extern Server * server;

void * accept_connections(void * arg);
void * listen_client_socket(void * client_socket);
int sending_message(ParsedCommand * command, Client * _client);
void * load_users_from_file(void * arg);
void save_users_to_file();
void handle_exit(int sig);
char * server_functions();

#endif