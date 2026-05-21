#ifndef COMMAND_H
#define COMMAND_H

#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #define PATH_SEP ";"
    #define access _access
    #define EXE_EXT ".exe"
    #define X_OK 0
    #define getcwd _getcwd
    #define chdir  _chdir
    #include <windows.h>
#else
    #include <unistd.h>
    #define PATH_SEP ":"
    #define EXE_EXT ""
    #include <sys/types.h>
    #include <sys/wait.h>
#endif
#define LIMIT 64



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>



typedef struct{
    char raw_input[1024];
    char* cmd;
    char* args;
}Command;

void parse_command(Command* cmd, const char *input);
void find_path(const char* args);
void run_external_program(const char* cmd_name, const char* args);
int handle_command(Command* cmd);
int is_builtin(const char* cmd);
void cd(const char* arg);
int gwd();

#endif

