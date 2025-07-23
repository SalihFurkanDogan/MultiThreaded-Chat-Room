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

#include "../include/User.h"
#include "../include/Server.h"
#include "../include/Room.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENT 25

char * server_functions(){
    char * message = "----------/[Functions]\\----------\n[1] - LOGIN \"Username\" \"Password\"\n[2] - REGISTER \"Username\" \"Password\"\n[3] - MSG \"Username/ROOM\" \"Message\"\n[4] - LIST Room/Server (Listing Online Clients)\n[5] - JOIN \"Room ID\"\n[6] - LEAVE (Leave Current Room)\n[7] - EXIT (Exit Server)\n\n";
    return message;
}

void * accept_connections(void * arg){
    while (1)
    {
        int client_socket;
        client_socket = accept(server->server_fd, (struct sockaddr *)&server->address, &server->addrlen);
        if (client_socket < 0) {
            perror("[ERROR] - Server Accepting Error!");
            exit(EXIT_FAILURE);
        }
        else{
            printf("[INFO] - Client Connected Server!\n");
            send(client_socket, "hosgeldin", 10,0);
            Client * new_client = create_client(client_socket);
            client_add_list(new_client);
            pthread_t tid;
                pthread_create(&tid, NULL, listen_client_socket, (void *)new_client);
        }
    }
    return NULL;
}

void * listen_client_socket(void * client_socket){
    Client * client = (Client *)client_socket;
    char buffer[1024] = { 0 };
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client->client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        ParsedCommand parsed = parse_command(buffer);
        if(!strcmp(parsed.command, "LOGIN")){
            client->user = login_user(&parsed);
            if (client->user == NULL)
            {
                char * error_login_message = "[ERROR] - Invalid Username Or Password!\n";
                send(client->client_fd, error_login_message, strlen(error_login_message), 0);
            }
            else{
                char * success_login_message = "[SERVER] - Logged In Successfully!\n";
                client->client_name = strdup(client->user->username);
                send(client->client_fd, success_login_message, strlen(success_login_message), 0);
                client->auth = 1;
            }
        }else if(!strcmp(parsed.command, "REGISTER")){
            if (client->auth == 1)
            {
                char * error_login_message = "[ERROR] - You Are Already Logged In!\n";
                send(client->client_fd, error_login_message, strlen(error_login_message), 0);
            }
            else{
                int result = register_user(&parsed);
                if (result == 1)
                {
                    char * success_register_message = "[SERVER] - You Are Successfully Registered Server Please Login Server!\n";
                    send(client->client_fd, success_register_message, strlen(success_register_message), 0);
                }
                else if(result == -1){
                    char * success_register_message = "[SERVER] - This Username Already Have Using!!\n";
                    send(client->client_fd, success_register_message, strlen(success_register_message), 0);
                }
                else{
                    char * error_register_message = "[ERROR] - You Are Not Registered Server Try Again!\n";
                    send(client->client_fd, error_register_message, strlen(error_register_message), 0);
                }
            }
        
        }else if(!strcmp(parsed.command, "MSG")){
            if (client->auth == 1)
            {
                int result = sending_message(&parsed, client);
                if (result == -1)
                {
                    char * error_message_sending = "[ERROR] - This User Is Not Online!\n";
                    send(client->client_fd, error_message_sending, strlen(error_message_sending), 0);
                }
            }
            else{
                char * error_create_message = "[SERVER] - Login Server First!\n";
                send(client->client_fd, error_create_message, strlen(error_create_message), 0);
            }
            
        }else if(!strcmp(parsed.command, "LIST")){
            if (!strcmp(parsed.args[0], "SERVER"))
            {
                char * new_buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
                Node * temp_node = server->first_client;
                if (temp_node == NULL)
                {
                    char * msg = "[SERVER] - No one is Online!\n";
                    send(client->client_fd, msg, strlen(msg), 0);
                }
                else{
                    Client * temp_client = NULL;
                    while (temp_node != NULL)
                    {
                        temp_client = (Client *)temp_node->data;
                        sprintf(new_buffer,"[SERVER] - Online User : %s\n", temp_client->client_name);
                        printf("ONLİNE USER : %s\n", temp_client->client_name);
                        send(client->client_fd, new_buffer, strlen(new_buffer), 0);
                        memset(new_buffer, 0, BUFFER_SIZE);
                        temp_node = temp_node->next;
                    }
                }
            }
            else if(!strcmp(parsed.args[0], "ROOM")){
                if (client->auth == 1)
                {
                    if (parsed.args[1] == NULL)
                    {
                        char * message = "[SERVER] - Missing Args!\n\tLIST ROOM \"Room ID\"";
                        send(client->client_fd, message, strlen(message), 0);
                    }
                    else{
                        int get_room_id = atoi(parsed.args[1]);
                        Node * temp_node = server->rooms;
                        Room * temp_room = find_room_with_id(get_room_id);

                        if (temp_node == NULL || temp_room == NULL)
                        {
                            char * error_message = "[ERROR] - Room Not Found!\n";
                            send(client->client_fd, error_message, strlen(error_message), 0);
                            continue;
                        }

                        if (temp_room->clients == NULL)
                        {
                            char * empty_msg = "[INFO] - Room is Empty!\n";
                            send(client->client_fd, empty_msg, strlen(empty_msg), 0);
                            continue;
                        }
                        char * list_buffer = malloc(BUFFER_SIZE);
                        int i = 0;
                        Node * client_node = temp_room->clients;

                        while (client_node != NULL)
                        {
                            Client * temp_client = (Client *)client_node->data;
                            Node * next = client_node->next;
                        
                            if (temp_client == NULL || temp_client->user == NULL) {
                                client_node = next;
                                continue;
                            }
                        
                            if (temp_client == client) {
                                client_node = next;
                                continue;
                            }
                        
                            memset(list_buffer, 0, BUFFER_SIZE);
                        
                            if (temp_client == temp_room->room_owner)
                                sprintf(list_buffer, "[Room Owner %d] Name : %s\n", i, temp_client->user->username);
                            else
                                sprintf(list_buffer, "[Client %d] Name : %s\n", i, temp_client->user->username);
                        
                            send(client->client_fd, list_buffer, strlen(list_buffer), 0);
                            i++;
                        
                            client_node = next;
                        }
                    }
                }
                else{
                    char * error_create_message = "[SERVER] - Login Server First!\n";
                    send(client->client_fd, error_create_message, strlen(error_create_message), 0);
                }
            }
        }else if(!strcmp(parsed.command, "JOIN")){
            if (client->auth == 1 && client->client_current_room_id == -1)
            {    
                int get_room_id = atoi(parsed.args[0]);
                int result = room_add_client(get_room_id, client);
                if (result == 1)
                {
                    char * success_message = (char *)malloc(sizeof(char) * 35);
                    sprintf(success_message, "[INFO] - You Are Joined Room : %d\n", get_room_id);
                    send(client->client_fd, success_message, strlen(success_message), 0);
                    free(success_message);
                }
                else if (result == -2)
                {
                    char * success_message = (char *)malloc(sizeof(char) * 35);
                    sprintf(success_message, "[INFO] - You Are Not Joined Room : %d\nRoom is FULL!\n", get_room_id);
                    send(client->client_fd, success_message, strlen(success_message), 0);
                    free(success_message);
                }
                else{
                    char * error_message = (char *)malloc(sizeof(char) * 47);;
                    sprintf(error_message, "[ERROR] - Room %d Not Finded!\n", get_room_id);
                    send(client->client_fd, error_message, strlen(error_message), 0);
                    free(error_message);
                }
                
            }
            else if(client->auth == 1 && client->client_current_room_id != -1){
                char * message = "[ERROR] - Leave First Current Room And Try Again!\n";
                send(client->client_fd, message, strlen(message), 0);
            }
            else{
                char * error_create_message = "[SERVER] - Login Server First!\n";
                send(client->client_fd, error_create_message, strlen(error_create_message), 0);
            }
        }else if(!strcmp(parsed.command, "SHOW")){
            char * message = (char *)malloc(sizeof(char) * BUFFER_SIZE);
                if (message == NULL)
                {
                    printf("Memory Allocation Error!\n");
                    exit(EXIT_FAILURE);
                }
                sprintf(message, "[INFO] - Username : %s\n\tPassword : %s\n\tCurrent Room ID : %d\n", client->user->username, client->user->password, client->client_current_room_id);
                send(client->client_fd, message, strlen(message), 0);
                free(message);
        
        }else if(!strcmp(parsed.command, "EXIT")){
            //main.c Dosyasında düzenleme yapmak lazım ona göre burayı tekrar inşa et
            if (client != NULL && client->auth == 1)
            {
                delete_client(client);
            }
            
        }else if(!strcmp(parsed.command, "LEAVE")){
            if (client->client_current_room_id != -1 && client->auth == 1)
            {
                if (leaving_room(client) == 1)
                {
                    printf("[INFO] - %s Client Leaving Room!\n", client->user->username);
                    char * message = "[SERVER] - You are Leaved Room!\n";
                    send(client->client_fd, message, strlen(message), 0);
                }
                else{
                    printf("[INFO] - Can't Leaving Room!\n");
                    char * message = "[ERROR] - You are Not Leaved Room!\n";
                    send(client->client_fd, message, strlen(message), 0);
                }
            }
            
        }else if(!strcmp(parsed.command, "CREATE")){
            if (client->auth == 1)
            {
                Room * new_room = create_room(&parsed, client);
                if (new_room == NULL)
                {
                    char * error_create_message = "[ERROR] - Room Creation Error!\n";
                    send(client->client_fd, error_create_message, strlen(error_create_message), 0);
                    free(new_room);
                    free(error_create_message);
                }
                else{
                    char * success_create_message = (char *)malloc(sizeof(char) * 48);
                    sprintf(success_create_message, "[INFO] - Room Creation Success! \n\tRoom ID = %d\n", new_room->room_id);
                    add_room_list(new_room);
                    send(client->client_fd, success_create_message, strlen(success_create_message), 0);
                    free(success_create_message);
                }
            }
            else{
                char * error_create_message = "[SERVER] - Login Server First!\n";
                send(client->client_fd, error_create_message, strlen(error_create_message), 0);
            }
        }else if(!strcmp(parsed.command, "DESTROY")){
            if (client->auth == 1 && parsed.args[1] != NULL)
            {
                int room_id = atoi(parsed.args[1]);
                int result = destroy_room_with_id(room_id, client);
                printf("--------------------\n");
                if (result == 1)
                {
                    char * message = "[SERVER] - Room is Successfully Destroyed!\n";
                    send(client->client_fd, message, strlen(message), 0);
                }
                if (result == -1)
                {
                    char * message = "[SERVER] - Room is Not Finded!\n";
                    send(client->client_fd, message, strlen(message), 0);
                }
                if (result == -2)
                {
                    char * message = "[SERVER] - Room Owner is Not You!\n";
                    send(client->client_fd, message, strlen(message), 0);
                }
            }
            else{
                char * message = "[ERROR] - DESTROY ROOM \"Room ID\"\n";
                send(client->client_fd, message, strlen(message), 0);
            }
            
        }else{
            send(client->client_fd, "[INFO] - Unknown Command!\n", 27, 0);
        }
    }
    return NULL;
}

int sending_message(ParsedCommand * command, Client * _client){
        if (!strcmp(command->args[0], "ROOM"))
        {
            Room * room = find_room_with_id(_client->client_current_room_id);
                if (room == NULL)
                {
                    printf("[ERROR] - Room Not finded!\n");
                    return -1;
                }
                Node * temp_node = room->clients;
                    if (temp_node == NULL)
                    {
                        printf("[ERROR] - Room Not Have Client!\n");
                        return -1;
                    }
                    Client * temp_client = NULL;
                    char * message = (char *)malloc(sizeof(char) * BUFFER_SIZE);
                    sprintf(message, "[R - %s] -> %s", _client->user->username, command->args[1]);
                    while (temp_node != NULL)
                    {
                        temp_client = (Client *)temp_node->data;
                        if (temp_client != _client)
                        {
                            send(temp_client->client_fd, message, strlen(message), 0);
                        }
                        temp_node = temp_node->next;
                    }
                    return 1;
        }
        else{
            Client * online_client = check_online_user(command->args[0]);
            if (online_client != NULL)
            {
                char * message = (char *)malloc(sizeof(char) * BUFFER_SIZE);
                sprintf(message, "[P - %s] -> %s", _client->user->username, command->args[1]);
                send(online_client->client_fd, message, strlen(message), 0);
                return 1;
            }
            else{
                return -1;
            }
        }
    return 0;
}

void * load_users_from_file(void * arg){
    FILE * f = fopen("client.txt", "r");
    if (f == NULL)
    {
        printf("[ERROR] - File Reading Error!\n");
        exit(EXIT_FAILURE);
    }
    
    char buffer[1024] = { 0 };
    while ((fgets(buffer, 1024, f)) != NULL)
    {
        User * temp_user = (User *)malloc(sizeof(User) * 1);
            if (temp_user == NULL)
            {
                printf("[ERROR] - Memory Allocation Error!");
                exit(EXIT_FAILURE);
            }
            temp_user->username = (char *)malloc(sizeof(char) * 15);
            temp_user->password = (char *)malloc(sizeof(char) * 15);
            if (temp_user->username == NULL || temp_user->password == NULL)
            {
                printf("[ERROR] - Memory Allocation Error!");
                exit(EXIT_FAILURE);
            }
        
            char * token = strtok(buffer, ",");
            temp_user->username = strdup(token);
            token = strtok(NULL, "\n");
            temp_user->password = strdup(token);
        
        Node * user_node = (Node *)malloc(sizeof(Node) * 1);
            if (user_node == NULL)
            {
                printf("[ERROR] - Memory Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
            user_node->next = NULL;
            user_node->data = (void *)temp_user;
        
        Node * temp_node = server->first_user;
            if (temp_node == NULL)
            {
                server->first_user = user_node;
            }
            else{
                while (temp_node->next != NULL)
                {
                    temp_node = temp_node->next;
                }
                temp_node->next = user_node;
            }
    }

    Node * reader_node = server->first_user;
    User * temp_user = NULL;
    while (reader_node != NULL)
    {
        temp_user = (User *)reader_node->data;
        reader_node = reader_node->next;
    }
    return NULL;
}

void save_users_to_file(){
    FILE * f = fopen("client.txt", "w");
    if (!f) {
        perror("[ERROR] - Failed to open file for writing");
        return;
    }

    Node *temp = server->first_user;
    while (temp) {
        User *user = (User *)temp->data;
        fprintf(f, "%s,%s\n", user->username, user->password);
        temp = temp->next;
    }

    fclose(f);
    printf("[INFO] - Users saved to file successfully.\n");
}

void handle_exit(int sig) {
    printf("\n[INFO] - Signal received, saving users and exiting...\n");
    save_users_to_file();
    exit(0);
}