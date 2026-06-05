#include <stdio.h>
#include "command.h"
#include "autocomplete.h"



int main() {
    setbuf(stdout, NULL); 
    char buffer[1024];
    Command cmd = {0};   

    while (1) {
        char cwd[PATH_MAX];

        if(getcwd(cwd, sizeof(cwd)) != NULL){
            char* home = getenv("HOME");
            if(home && strcmp(cwd, home)==0){
                printf("$ ");
            }else{
                printf("%s\n $ ", cwd);
            }
        }else{
            printf("$ "); 
        }

        memset(buffer, 0, sizeof(buffer));
        int buf_len = 0;

        enable_raw_mode();

        while (buf_len < sizeof(buffer) - 1) {
            char c;
            
            #if defined(_WIN32)
                DWORD read;
                ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), &c, 1, &read, NULL);
            #else
                read(STDIN_FILENO, &ch, 1);
            #endif

            if (c == '\t') {
                autocomplete(buffer); 
                continue;
            }


            if (c == '\n' || c == '\r') {
                printf("\n");
                break;
            }

            if (c == 127 || c == '\b') {
                if (buf_len > 0) {
                    buf_len--;
                    buffer[buf_len] = '\0';
                    printf("\b \b"); 
                }
                continue;
            }

            buffer[buf_len++] = c;
            printf("%c", c);
        }

        disable_raw_mode();

        if (buf_len == 0) continue;

        parse_command(&cmd, buffer);
        
        if (!handle_command(&cmd)) {
            break; 
        }
    }

    if (cmd.cmd) free(cmd.cmd); 
    return 0;
}



