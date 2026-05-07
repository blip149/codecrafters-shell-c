#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>


static char* builtins[] = {"echo", "type", "exit", "quit", NULL};

int handle_cmd(char *cmd);

int main() {
    setbuf(stdout, NULL);
    char command[1024];

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
            } else{
                handle_cmd(arg);
            }
        }
        else {
            printf("%s: command not found\n", cmd);
        }
    }
    return 0;
}

int handle_cmd(char* cmd) {
    for (int i = 0; builtins[i] != NULL; i++){
        if (strcmp(cmd, builtins[i]) == 0){
            printf("%s is a shell builtin\n", cmd);
            return 0;
        }
    }

    char* env_path = getenv("PATH");
    if (env_path == NULL) {
        printf("%s: not found (PATH empty)\n", cmd);
        return 1;
    }

    char* path_copy = strdup(env_path);
    char* dir = strtok(path_copy, ";");
    static char result_path[1024];

    while (dir != NULL) {
        snprintf(result_path, sizeof(result_path), "%s\\%s.exe", dir, cmd);
        
        if(_access(result_path, 0) == 0){
            printf("%s is %s\n", cmd, result_path);
            free(path_copy);
            return 0;
        }
        dir = strtok(NULL, ";");
    }

    free(path_copy);
    printf("%s: not found\n", cmd);
    return 0;
}
