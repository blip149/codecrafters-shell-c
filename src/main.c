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

        if (!fgets(buffer, sizeof(buffer), stdin)) break;

        enable_raw_mode();
        if (strcmp(buffer, '\t')==0){
            autocomplete(buffer);
        }
        disable_raw_mode();

        parse_command(&cmd, buffer);
        
        if (!handle_command(&cmd)) {
            break; 
        }
    }


    if (cmd.cmd) free(cmd.cmd); 

    return 0;
}


