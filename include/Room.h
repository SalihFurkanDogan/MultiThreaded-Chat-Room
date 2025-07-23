#ifndef ROOM_H
#define ROOM_H

#include "Client.h"
#include "Node.h"

typedef struct Room{
    int room_id;
    int room_size;
    int room_client_count;
    char * room_name;
    Client * room_owner;
    Node * clients;
}Room;

void add_room_list(Room * _room);
Room * find_room_with_id(int room_id);
int get_last_room_id(void);
int room_add_client(int room_id, Client * _client);
Room * create_room(ParsedCommand * command, Client * client);
int leaving_room(Client * _client);
int destroy_room_with_id(int room_id, Client * _client);

#endif