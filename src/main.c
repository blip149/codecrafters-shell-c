#include <stdio.h>
#include "command.h"
#include "redirects.h"



int main() {
    setbuf(stdout, NULL); 
    char buffer[1024];
    Command cmd = {0};   

    while (1) {
        printf("$ ");
        if (!fgets(buffer, sizeof(buffer), stdin)) break;

        parse_command(&cmd, buffer);
        
        if (!handle_command(&cmd)) {
            break; 
        }
        red_stdout(&cmd, buffer);
    }


    if (cmd.cmd) free(cmd.cmd); 

    return 0;
}


