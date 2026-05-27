#include "redirects.h"
#include <stdlib.h>

void red_stdout(Command* cmd) {

    if(!cmd || cmd->argc == 0) return;

    for (int i = 0; i<cmd->argc; i++) {
        if (strcmp(cmd->args[i], ">" || strcmp(cmd->args[i], "1>")) == 0){

            if (i + 1 >= cmd->argc) {
                fprintf(stderr, "Shell error: no file specified");
                return;
            }

            char* file_name = cmd->args[i+1];

            FILE* stream = fopen(file_name, "w");
            if (!stream){
                perror("fopen failed");
                return;
            }

            int file_fd = fileno(stream);
            #ifdef _WIN32
                _write(file_fd, file_name, 14);
            #else
                if (dup2(file_fd, STDOUT_FILENO) < 0){
                    perror("dup2 failed");
                    fclose(stream);
                    return;
                }
                fclose(stream);

                cmd->args[i] = NULL;
                cmd->argc = i;

            #endif
            break;
        }
    }
}