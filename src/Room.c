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

#include "../include/Server.h"
#include "../include/Room.h"

void add_room_list(Room * _room){
    Node * temp_node = server->rooms;
    Node * new_node = (Node *)malloc(sizeof(Node) * 1);
        if(new_node == NULL){
            printf("[ERROR] - Memory Allocation Error!\n");
            exit(EXIT_FAILURE);
        }
        new_node->data = (void *)_room;
        new_node->next = NULL;

        if (server->rooms == NULL)
        {
            server->rooms = new_node;
            printf("[INFO] - Room Added Server List!\n");
            return;
        }
        
        while (temp_node->next != NULL)
        {
            temp_node = temp_node->next;
        }
        temp_node->next = new_node;
        printf("[INFO] - Room Added Server List!\n");
        return;
}

Room * find_room_with_id(int room_id){
    Node * temp_node = server->rooms;
        if (temp_node == NULL)
        {
            return NULL;
        }
        Room * temp_room = NULL;
        while (temp_node != NULL)
        {
            temp_room = (Room *)temp_node->data;
            if (temp_room->room_id == room_id)
            {
                return temp_room;
            }
            temp_node = temp_node->next;
        }
    return NULL;
}

int get_last_room_id(){
    int max_id = 0;
    Node * temp_node = server->rooms;

    while (temp_node != NULL)
    {
        Room * temp_room = (Room *)temp_node->data;
        if (temp_room->room_id > max_id)
        {
            max_id = temp_room->room_id;
        }
        temp_node = temp_node->next;
    }

    return max_id;
}

int room_add_client(int room_id, Client * _client){
    Node * temp_node = NULL;
        Room * temp_room = find_room_with_id(room_id);
        if (temp_room == NULL) return -1;
        if (temp_room->room_client_count == temp_room->room_size) return -2;
        
        Node * new_node = (Node *)malloc(sizeof(Node) * 1);
            if (new_node == NULL)
            {
                printf("[ERROR] - Memory Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
            new_node->data = (void *)_client;

        temp_node = temp_room->clients;
        if (temp_node == NULL)
        {
            temp_room->clients = new_node;
            temp_room->room_client_count++;
            _client->client_current_room_id = temp_room->room_id;
            printf("[INFO] - Room Client Count : %d\n", temp_room->room_client_count);
            return 1;
        }

        while (temp_node->next != NULL)
        {
            temp_node = temp_node->next;
        }
        temp_node->next = new_node;

        _client->client_current_room_id = room_id;
        _client->client_current_room_id = temp_room->room_id;
        temp_room->room_client_count++;
        printf("[INFO] - Room Client Count : %d\n", temp_room->room_client_count);
    return 1;
}

Room * create_room(ParsedCommand * command, Client * client){
    Room * new_room = (Room *)malloc(sizeof(Room) * 1);
        if (new_room == NULL)
        {
            printf("[ERROR] - Memory Allocation Error!\n");
            exit(EXIT_FAILURE);
        }
        new_room->room_owner = client;
        new_room->room_size = atoi(command->args[1]);
        new_room->room_name = strdup(command->args[0]);
        new_room->room_id = get_last_room_id() + 1;
        new_room->clients = (Node *)malloc(sizeof(Client) * new_room->room_size);
        client->client_current_room_id = new_room->room_id;
        Node * new_node = (Node *)malloc(sizeof(Node) * 1);
            new_node->data = (void *)client;
            new_room->clients = new_node;
        return new_room;
}

int leaving_room(Client * _client){
    if (_client == NULL) return -1;
    if (_client->client_current_room_id == -1) return -1;

    Room * temp_room = find_room_with_id(_client->client_current_room_id);
    if (temp_room == NULL) return -1;

    Node * temp_node = temp_room->clients;
    Node * prev = NULL;

    while (temp_node != NULL) {
        Client * temp_client = (Client *)temp_node->data;
        if (strcmp(temp_client->user->username, _client->user->username) == 0) {
            if (prev == NULL) {
                temp_room->clients = temp_node->next;
            } else {
                prev->next = temp_node->next;
            }
            free(temp_node);
            _client->client_current_room_id = -1;
            temp_room->room_client_count--;
            return 1;
        }
        prev = temp_node;
        temp_node = temp_node->next;
    }
    return -1;
}

int destroy_room_with_id(int room_id, Client * _client){
    Room * room = find_room_with_id(room_id);
        if (room == NULL) return -1;
        if (room->room_owner != _client) return -2;

        Node * temp_node = server->rooms;
        Node * prev = NULL;
        Room * temp_room = NULL;

        while (temp_node != NULL)
        {
            temp_room = (Room *)temp_node->data;
            if (temp_room->room_id == room->room_id)
            {
                break;
            }
            prev = temp_node;
            temp_node = temp_node->next;
        }
        if (prev == NULL) {
            server->rooms = temp_node->next;
        } else {
            prev->next = temp_node->next;
        }
        free(room->room_name);
        Client * temp_client = NULL;
        Node * current = room->clients;
        char * message = "[SERVER] - Room is Destroyed by Owner!\n";
        while (current != NULL) {
            temp_client = (Client *)current->data;
            send(temp_client->client_fd, message, strlen(message), 0);
            temp_client->client_current_room_id = -1;
        
            Node * next = current->next;
            free(current);
            current = next;
        }
        free(room);
        return 1;
}
