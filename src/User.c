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

int register_user(ParsedCommand * command){
    char * _username = strdup(command->args[0]);
    char * _password = strdup(command->args[1]);
    if (check_username(_username) == -1)
    {
        return -1;
    }
    
    User * new_user = (User *)malloc(sizeof(User) * 1);
        if (new_user == NULL)
        {
            printf("[ERROR] - Memory Allocation Error!\n");
            exit(EXIT_FAILURE);
        }
        new_user->username = (char *)malloc(sizeof(char) * strlen(_username) + 1);
        new_user->password = (char *)malloc(sizeof(char) * strlen(_password) + 1);
            if (new_user->password == NULL || new_user->username == NULL)
            {
                printf("[ERROR] - Memory Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
            strcpy(new_user->username, _username);
            strcpy(new_user->password, _password);
            printf("USERNAME : %s, PASSWORD : %s\n", new_user->username, new_user->password);
            Node * new_node = (Node *)malloc(sizeof(Node) * 1);
                if (new_node == NULL)
                {
                    printf("[ERROR] - Memory Allocation Error!\n");
                    exit(EXIT_FAILURE);
                }
                new_node->data = (void *)new_user;
                new_node->next = NULL;
                
                Node * temp_node = server->first_user;
                while (temp_node->next != NULL)
                {
                    temp_node = temp_node->next;
                }
                temp_node->next = new_node;
                free(_username);
                free(_password);
    return 1;
}

User * login_user(ParsedCommand * command){
    if (server->first_user == NULL)
    {
        return NULL;
    }
    Node * temp_node = server->first_user;
    User * temp_user = NULL;
    while (temp_node != NULL)
    {
        temp_user = (User *)temp_node->data;
        if (!strcmp(temp_user->username, command->args[0]) && !strcmp(temp_user->password, command->args[1]))
        {
            return temp_user;
        }
        temp_node = temp_node->next;
    }
    return NULL;
}

int check_username(char * username){
        Node * temp_node = server->first_user;
        User * temp_user = NULL;
        while (temp_node != NULL)
        {
            temp_user = (User *)temp_node->data;
            if (!strcmp(username, temp_user->username))
            {
                return -1;
            }
            temp_node = temp_node->next;
        }
    return 1;
}