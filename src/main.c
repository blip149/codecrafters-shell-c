#include <stdio.h>
#include "command.h"



int main() {
    setbuf(stdout, NULL); 
    char buffer[1024];
    Command cmd = {0};   

    while (1) {
        printf("$ ");
        if (!fgets(buffer, sizeof(buffer), stdin)) break;

        command_parse(&cmd, buffer);
        
        if (!handle_command(&cmd)) {
            break; 
        }
    }


    if (cmd.cmd) free(cmd.cmd); 

    return 0;
}


