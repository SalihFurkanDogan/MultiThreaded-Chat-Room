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

#include "../include/Parse_Command.h"


ParsedCommand parse_command(const char * input){
    ParsedCommand result = {0};
    char buffer[1024];
    strncpy(buffer, input, 1024);
    buffer[1024 - 1] = '\0';

    char * token = strtok(buffer, " ");
    if (!token) return result;

    strncpy(result.command, token, sizeof(result.command) - 1);

    int index = 0;
    char * rest = strtok(NULL, "");
    if(!strcmp(result.command, "MSG")){
        token = strtok(rest, " ");
        if (token) {
            result.args[0] = strdup(token);
            result.arg_count = 1;
            const char * room_pos = strstr(input, token);
            if (room_pos) {
                const char * msg_start = room_pos + strlen(token);
                while (* msg_start == ' ') msg_start++;
                if (* msg_start != '\0') {
                    result.args[1] = strdup(msg_start);
                    result.arg_count++;
                }
            }
        }
        return result;
        
    }
    if (strcmp(result.command, "LOGIN") == 0 ||
        strcmp(result.command, "REGISTER") == 0 ||
        strcmp(result.command, "JOIN") == 0 ||
        strcmp(result.command, "CREATE") == 0 || 
        strcmp(result.command, "DESTROY") == 0 ||
        strcmp(result.command, "LIST") == 0) 
        {
        token = strtok(rest, " ");
        while (token && index < 3 - 1) {
            result.args[index++] = strdup(token);
            rest += strlen(token) + 1;
            token = strtok(NULL, " ");
        }
        if (rest && * rest != '\0') {
            result.args[index++] = strdup(rest);
        }
    }
    else {
        result.arg_count = 0;
        return result;
    }

    result.arg_count = index;
    return result;
}

void free_parsed_command(ParsedCommand * cmd){
    for (int i = 0; i < cmd->arg_count; i++) {
        free(cmd->args[i]);
    }
}