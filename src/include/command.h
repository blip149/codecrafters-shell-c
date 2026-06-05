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
    #define PATH_MAX 4096
    #define STDOUT_FILENO 1
    #define STDERR_FILENO 2
    #define fileno _fileno
    #define dup _dup
    #define dup2 _dup2
    #define close _close

#else
    #include <unistd.h>
    #define PATH_SEP ":"
    #define EXE_EXT ""
    #include <sys/types.h>
    #include <sys/wait.h>
    #define _strdup strdup
    #include <termios.h>
#endif
#define LIMIT 64



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>



typedef struct{
    char raw_input[1024];
    char* cmd;
    char* args[LIMIT];
    int argc;
}Command;

void parse_command(Command* cmd, const char *input);
void find_path(const char* args);
void run_external_program(Command *cmd);
int handle_command(Command* cmd);
int is_builtin(const char* cmd);
void cd(const char* arg);
int gwd();

#endif

