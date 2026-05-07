#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    setbuf(stdout, NULL);
    char command[1024];
    char builtins[4][8] = {"echo", "type", "exit", "quit"};

    while(1) {
        printf("$ ");

        if (fgets(command, sizeof(command), stdin) == NULL) break;

        command[strcspn(command, "\n")] = '\0';


        char *cmd = strtok(command, " ");
        char *arg = strtok(NULL, ""); 

        if (cmd == NULL) continue; 

        
        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            break;
        }

        
        else if (strcmp(cmd, "echo") == 0) {
            if (arg) printf("%s\n", arg);
            else printf("\n");
        }

      
        else if (strcmp(cmd, "type") == 0) {
            if (arg == NULL) {
                printf("type: missing operand\n");
            } else {
                int found = 0;
                for (int i = 0; i < 4; i++) {
                    if (strcmp(arg, builtins[i]) == 0) {
                        printf("%s is a shell builtin\n", arg);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s: not found\n", arg);
                }
            }
        }

        else {
            printf("%s: command not found\n", cmd);
        }
    }
    return 0;
}


