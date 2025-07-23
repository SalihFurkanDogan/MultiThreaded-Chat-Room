#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>

#include "../include/Client.h"
#include "../include/Node.h"
#include "../include/Server.h"

void client_add_list(Client * _client){
    if (_client == NULL) {
        printf("[WARN] - Invalid client or client name.\n");
        return;
    }
    Node * new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("[ERROR] - Memory Allocation Error!\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = (void *)_client;
    new_node->next = NULL;

    if (server->first_client == NULL) {
        server->first_client = new_node;
    } else {
        Node * temp = server->first_client;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

void delete_client(Client * _client){
    if (_client == NULL)
    {
        printf("[ERROR] - Client is NULL!\n");
        return;
    }
    
    Node * temp_node = server->first_client;
    Node * prev = NULL;
        if (temp_node == NULL)
        {
            printf("[ERROR] - No One is Online This Server!\n");
            return;
        }
        Client * temp_client = NULL;
        while (temp_node != NULL)
        {
            temp_client = (Client *)temp_node->data;
            prev = temp_node;
            if (temp_client == _client)
            {   
                break;
            }
            temp_node = temp_node->next;
        }
        prev->next = temp_node->next;
        printf("[SERVER] - Client %s Disconnect Server!\n", _client->user->username);
        close(_client->client_fd);
        free(_client->client_name);
        free(_client);
        return;
}

Client * create_client(int client_socket){
    Client * new_client = (Client *)malloc(sizeof(Client) * 1);
        if (new_client == NULL)
        {
            printf("[ERROR] - Memory Allocation Error!\n");
            exit(EXIT_FAILURE);
        }
        new_client->auth = 0;
        new_client->client_current_room_id = -1;
        new_client->client_fd = client_socket;
        new_client->client_name = NULL;
        new_client->user = malloc(sizeof(User *));
        return new_client;
}

Client * check_online_user(char * _username){
    if (_username == NULL)
    {
        return NULL;
    }
        
    Node * temp_node = server->first_client;
        if (temp_node == NULL)
        {
            return NULL;
        }
        Client * temp_client = NULL;
        while (temp_node != NULL)
        {
            temp_client = (Client *)temp_node->data;
            if (!strcmp(temp_client->client_name, _username))
            {
                printf("Checked Online USER : %s\n", temp_client->user->username);
                return temp_client;
            }
            temp_node = temp_node->next;
        }
    return NULL;
}