#ifndef PARSE_COMMAND_H
#define PARSE_COMMAND_H

typedef struct ParsedCommand{
    char command[32];
    char *args[3];
    int arg_count;
}ParsedCommand;

void free_parsed_command(ParsedCommand * cmd);
ParsedCommand parse_command(const char * input);

#endif