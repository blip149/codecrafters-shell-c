#include "redirects.h"
#include <stdlib.h>

void red_stdout(Command* cmd, char* input) {
    parse_command(cmd, input);

    for (int i = 0 ; i < cmd->argc; i++){
        if (cmd->args[i] == ">"){
            char* content = cmd->args[i - 1];
            FILE* stream = fopen(cmd->args[i++], "w");

            fprintf(stream, *content);
            fclose(stream);
        }
    }
}