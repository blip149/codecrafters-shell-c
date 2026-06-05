#include "autocomplete.h"
#include "builtins.h"
#include "command.h"


void enable_raw_mode(){
    #ifdef _WIN32
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD cooked_mode = 0;

        GetConsoleMode(hInput, &cooked_mode);
        DWORD raw_mode = cooked_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hInput, raw_mode);

    #else
        struct termios cooked_mode;
        tcgetattr(STDIN_FILENO, &cooked_mode);

        struct termios raw_mode = cooked_mode;
        raw_mode.c_lflag &= ~ (ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode);
    #endif
}

void disable_raw_mode(){
    #ifdef _WIN32
        HANDLE hinput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD cooked_mode = 0;

        SetConsoleMode(hinput, cooked_mode);
    #else
        struct termios cooked_mode;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &cooked_mode);
    #endif

}

void autocomplete(char* line) {
    size_t len = strlen(line);
    if(len == 0) return;

    const char* match = NULL;
    int count = 0;

    for(int i = 0 ; builtins[i]; i++){
        if(strncmp(builtins[i], line, len) == 0){
            match = builtins[i];
            count++;
        }
    }

    if(count == 1 && match){
        const char* complete = match + len;
        size_t length = strlen(complete);
        printf("%s ", complete);
        fflush(stdout);
        strncat(line, complete, length);
        strcat(line, " ");
    }
}

