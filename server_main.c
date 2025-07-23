#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "include/Node.h"
#include "include/User.h"
#include "include/Parse_Command.h"
#include "include/Client.h"
#include "include/Room.h"
#include "include/Server.h"

#define PORT 8080
#define MAX_CLIENT 25

Server * server = NULL;

int main(int argc, char const *argv[])
{
    if (argc < 2 || argv == NULL)
    {
        printf("\t-----------[INFO]-----------\n");
        printf("./Server \"Server Name\" \"Port Number\"\n");
        return 0;
    }

    server = (Server *)malloc(sizeof(Server) * 1);
    if (server == NULL)
    {
        printf("------------[ERROR]----------\n[-] Memory Allocation Error!\n");
        return 0;
    }
    else{
        server->server_name = (char *)malloc(sizeof(char) * 15);
        if (server->server_name == NULL)
        {
            printf("------------[ERROR]----------\n[-] Memory Allocation Error!\n");
            return 0;
        }
        strcpy(server->server_name, argv[1]);
        server->opt = 1;

        if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("[ERROR] - Socket Creation Error!");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &server->opt, sizeof(server->opt)) < 0) {
            perror("setsockopt(SO_REUSEADDR) başarısız");
            exit(EXIT_FAILURE);
        }

        #ifdef SO_REUSEPORT
        if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEPORT, &server->opt, sizeof(server->opt)) < 0) {
            perror("setsockopt(SO_REUSEPORT) başarısız");
            exit(EXIT_FAILURE);
        }
        #endif

        #ifdef SO_KEEPALIVE
        if (setsockopt(server->server_fd, SOL_SOCKET, SO_KEEPALIVE, &server->opt, sizeof(server->opt)) < 0)
        {
            perror("setsockopt(SO_KEEPALIVE) başarısız");
            exit(EXIT_FAILURE);
        }
        
        #endif
        server->address.sin_family = AF_INET;
        server->address.sin_addr.s_addr = INADDR_ANY;
        server->address.sin_port = htons(PORT);
        server->first_client = NULL;
        server->rooms = NULL;
        signal(SIGINT, handle_exit);
        
        if (bind(server->server_fd, (struct sockaddr *)&server->address,
                sizeof(server->address)) < 0) 
        {
            perror("[ERROR] - Bind Failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server->server_fd, MAX_CLIENT) < 0) {
            perror("[ERROR] - Listen Failed!");
            exit(EXIT_FAILURE);
        }
    }
    pthread_t file_th;
        pthread_create(&file_th, NULL, load_users_from_file, NULL);

    pthread_join(file_th, NULL);
    pthread_t tid;
        pthread_create(&tid, NULL, accept_connections, NULL);

    while(1);
    
    save_users_to_file();
    return 0;
}