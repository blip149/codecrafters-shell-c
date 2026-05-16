#ifndef COMMAND_H
#define COMMAND_H

#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #define PATH_SEP ";"
    #define access _access
    #define EXE_EXT ".exe"
    #define EXIST 0
#else
    #include <unistd.h>
    #define PATH_SEP ":"
    #define EXE_EXT ""
    #define EXIST X_OK
#endif




#include <stdio.h>
#include <string.h>
#include <stdlib.h>




typedef struct{
    char raw_input[1024];
    char* cmd;
    char* args;
}Command;

void parse_command(Command* cmd, const char *input);
int execute_cmd(Command* cmd);
void handle_cmd(const char* args);
int is_builtin(const char* cmd);

#endif

