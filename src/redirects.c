#include "redirects.h"
#include <stdlib.h>
#include <stdbool.h>

void red_std(Command* cmd) {

    if(!cmd || cmd->argc == 0) return;

    for (int i = 0; i<cmd->argc; i++) {
        bool is_stdout = strcmp(cmd->args[i], ">")== 0 || strcmp(cmd->args[i], "1>")==0;
        bool is_stdout_append = strcmp(cmd->args[i],">>")==0 || strcmp(cmd->args[i],"1>>")==0;
        bool is_stderr = strcmp(cmd->args[i], "2>")==0;
        bool is_stderr_append = strcmp(cmd->args[i],"2>>")==0;

        if (is_stderr || is_stdout || is_stderr_append || is_stdout_append){
            if(i+1>cmd->argc){
                fprintf(stderr, "No file specified");
                return ;
            }

            char* file_name = cmd->args[i+1];
            char* mode = (is_stderr_append || is_stdout_append) ? "a": "w";
            FILE* stream = fopen(file_name, mode);
            if(!stream) {
                perror("fopen failed");
                return;
            }

            int file_fd = fileno(stream);

            int target_fd = (is_stdout || is_stdout_append)? STDOUT_FILENO : STDERR_FILENO;

            if(dup2(file_fd, target_fd) < 0){
                perror("dup2 failed");
                fclose(stream);
                return;
            }
            fclose(stream);

            cmd->args[i] = NULL;
            cmd->argc = i;
            break;
        }

    }
}