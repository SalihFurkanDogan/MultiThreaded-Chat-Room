#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 8080

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void * read_messages_from_server(void * arg){
    pthread_mutex_lock(&print_lock);
    char buffer[1024] = { 0 };
    ssize_t valread;
    while (1)
    {
        memset(buffer, 0, 1024);
        valread = read(*(int *)arg, buffer, 1024 - 1); 
        if (valread <= 0)
        {
            continue;
        }
        buffer[valread] = '\0';
        printf("%s\n", buffer);
    }
    pthread_mutex_unlock(&print_lock);
}

int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status
         = connect(client_fd, (struct sockaddr *)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    pthread_t tid;
        pthread_create(&tid, NULL, read_messages_from_server, (void *)&client_fd);
    
    char buffer[1024] = { 0 };
    while (1) {
    memset(buffer, 0, 1024 - 1); // buffer'ı sıfırla
    if (fgets(buffer, 1024 - 1, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) > 0) {
            send(client_fd, buffer, strlen(buffer), 0);
        }
    }
}
    return 0;
}