#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #define PATH_SEP ";"
    #define access _access
    #define EXE_EXT ".exe"
#else
    #include <unistd.h>
    #define PATH_SEP ":"
    #define EXE_EXT ""
#endif

// Function Prototypes
void handle_type(char *arg);
int is_builtin(char *cmd);

static const char *builtins[] = {"echo", "type", "exit", "quit", NULL};

int main() {
    setbuf(stdout, NULL);
    char input[1024];

    while (1) {
        printf("$ ");
        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\r\n")] = '\0'; // Handle both \r and \n

        char *cmd = strtok(input, " ");
        char *arg = strtok(NULL, ""); // Get the rest of the string as the argument

        if (!cmd) continue;

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "echo") == 0) {
            printf("%s\n", arg ? arg : "");
        } else if (strcmp(cmd, "type") == 0) {
            handle_type(arg);
        } else {
            printf("%s: command not found\n", cmd);
        }
    }
    return 0;
}

int is_builtin(char *cmd) {
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(cmd, builtins[i]) == 0) return 1;
    }
    return 0;
}

void handle_type(char *arg) {
    if (!arg) {
        printf("type: missing operand\n");
        return;
    }

    if (is_builtin(arg)) {
        printf("%s is a shell builtin\n", arg);
        return;
    }

    char *env_path = getenv("PATH");
    if (env_path) {
        char *path_copy = strdup(env_path);
        char *dir = strtok(path_copy, PATH_SEP);
        char full_path[1024];

        while (dir) {
            // Check for both the raw name and the name + .exe for Windows compatibility
            snprintf(full_path, sizeof(full_path), "%s/%s%s", dir, arg, EXE_EXT);
            if (access(full_path, 0) == 0) {
                printf("%s is %s\n", arg, full_path);
                free(path_copy);
                return;
            }
            dir = strtok(NULL, PATH_SEP);
        }
        free(path_copy);
    }
    
    printf("%s: not found\n", arg);
}
