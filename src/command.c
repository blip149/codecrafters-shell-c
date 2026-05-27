#include "command.h"
#include <stdbool.h>
#define bool _Bool

static const char* builtins[] = {"echo", "exit", "type", "quit", "pwd"};

typedef enum{
    NORMAL_STATE,
    IN_DQUOTES,
    IN_SQUOTES,
}State;

void parse_command(Command *cmd, const char* input) {
    if (cmd->cmd != NULL) {
        free(cmd->cmd);
        cmd->cmd = NULL;
    }
    
    memset(cmd, 0, sizeof(Command));
    strncpy(cmd->raw_input, input, sizeof(cmd->raw_input) - 1);
    cmd->raw_input[strcspn(cmd->raw_input, "\r\n")] = '\0';

    char *peek_ptr = cmd->raw_input;
    
    while (*peek_ptr == ' ') peek_ptr++;
    if (*peek_ptr == '\0') return;

    char *s_ptr = peek_ptr;
    char *token_start = s_ptr;
    
    State state = NORMAL_STATE;
    bool escaped = false;

    while (*peek_ptr != '\0') {
        if (escaped) {
            if (state == IN_DQUOTES) {
                if (*peek_ptr == '\\' || *peek_ptr == '$' || *peek_ptr == '"') {
                    *s_ptr++ = *peek_ptr;
                } else {
                    *s_ptr++ = '\\';
                    *s_ptr++ = *peek_ptr;
                }
            } else {
                *s_ptr++ = *peek_ptr;
            }
            escaped = false;
            peek_ptr++;
        } 
        else if (state != IN_SQUOTES && *peek_ptr == '\\') {
            escaped = true;
            peek_ptr++;
        } 
        else if (*peek_ptr == '\'' && state != IN_DQUOTES) {
            state = (state == IN_SQUOTES) ? NORMAL_STATE : IN_SQUOTES;
            peek_ptr++;
        } 
        else if (*peek_ptr == '"' && state != IN_SQUOTES) {
            state = (state == IN_DQUOTES) ? NORMAL_STATE : IN_DQUOTES;
            peek_ptr++; 
        } 
        else if (*peek_ptr == ' ' && state == NORMAL_STATE) {
            *s_ptr++ = '\0'; 

            if (cmd->cmd == NULL) {
                cmd->cmd = _strdup(token_start);
            } else if (cmd->argc < LIMIT - 1) {
                cmd->args[cmd->argc] = token_start;
                cmd->argc++;
            }

            while (*peek_ptr == ' ') {
                peek_ptr++;
            }
            
            token_start = s_ptr; 
        } 
        else {
            *s_ptr++ = *peek_ptr++;
        }
    }

    if (s_ptr != token_start) {
        *s_ptr = '\0';
        if (cmd->cmd == NULL) {
            cmd->cmd = _strdup(token_start);
        } else if (cmd->argc < LIMIT - 1) {
            cmd->args[cmd->argc] = token_start;
            cmd->argc++;
        }
    }
    
    cmd->args[cmd->argc] = NULL; 
}

int handle_command(Command* cmd) {
    if (!cmd->cmd) return 1;

    if (strcmp(cmd->cmd, "exit")==0 || strcmp(cmd->cmd, "quit")==0){
        return 0;
    }else if (strcmp(cmd->cmd, "echo")==0){
        for (int i = 0; i<cmd->argc; i++){
            printf("%s%s", cmd->args[i], (i == cmd->argc - 1) ? "":" ");
        }
        printf("\n");
        
    }else if (strcmp(cmd->cmd, "type")==0){
        find_path(cmd->args[0]);
    }else if (strcmp(cmd->cmd, "pwd")==0){
        gwd();
    }else if (strcmp(cmd->cmd, "cd")==0){
        cd(cmd->argc > 0 ? cmd->args[0] : NULL);
    }
    else{
        run_external_program(cmd);
    }
    return 1;
    
}


int gwd() {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd))!=NULL){
        printf("%s\n", cwd);
    }else{
        perror("Error");
        return 1;
    }
    return 0;
}

void cd(const char* arg){
    char* home = getenv("HOME");

    if (arg==NULL || strcmp(arg, "~") == 0){
        chdir(home);
    }else{
        if (chdir(arg)!= 0){
            if (errno==ENOENT){
                printf("cd: %s: No such file or directory\n", arg);
            }else{
                perror("error");
            }
        }
    }
}


int is_builtin(const char *cmd) {
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(cmd, builtins[i]) == 0) return 1;
    }
    return 0;
}

void run_external_program(Command* cmd) {
    char* path_copy = NULL;
    int found = 0;

    char* env_path = getenv("PATH");
    if (!env_path) goto cleanup;

    path_copy = _strdup(env_path);
    char *dir = strtok(path_copy, PATH_SEP);
    char full_path[1024];

    while(dir) {
        snprintf(full_path, sizeof(full_path), "%s/%s%s", dir, cmd->cmd, EXE_EXT);
        if (access(full_path, X_OK) == 0) {
            found = 1;
            #ifdef _WIN32
                STARTUPINFO si;
                PROCESS_INFORMATION pi;

                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                char cmd_line[2048];
                int offset = snprintf(cmd_line, sizeof(cmd_line), "\"%s\"", full_path);
                for (int i = 0; i < cmd->argc; i++) {
                    offset += snprintf(cmd_line + offset, sizeof(cmd_line) - offset, " %s", cmd->args[i]);
                }

                if (CreateProcess(NULL, cmd_line, NULL, NULL, 0, FALSE, NULL, NULL, &si, &pi)) {
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                } else {
                    printf("failed to execute(%lu).\n", GetLastError());
                }
            #else
                pid_t pid = fork();
                if (pid < 0) {
                    perror("fork failed");
                } else if (pid == 0) {
                    char *argv[LIMIT];
                    int local_argc = 0;

                    // Element 0 must be the command name or full path path
                    argv[local_argc++] = (char*)cmd->cmd;

                    // Directly pass our pre-split arguments with zero strtok tampering!
                    for (int i = 0; i < cmd->argc && local_argc < (LIMIT - 1); i++) {
                        argv[local_argc++] = cmd->args[i];
                    }
                    argv[local_argc] = NULL;

                    execv(full_path, argv);
                        
                    perror("execution failed");
                    exit(1);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                }
            #endif
            goto cleanup;
        }
        dir = strtok(NULL, PATH_SEP);
    }

cleanup:
    if (!found) {
        printf("%s: command not found\n", cmd->cmd);
    }
    if (path_copy) free(path_copy);
}


void find_path(const char *args) {
    char *path_copy = NULL;
    int found = 0;

    if (!args) {
        printf("type: missing operand\n");
        return;
    }

    if (is_builtin(args)) {
        printf("%s is a shell builtin\n", args);
        return;
    }


    char *env_path = getenv("PATH");
    if (!env_path) goto cleanup; 

    path_copy = _strdup(env_path);
    char *dir = strtok(path_copy, PATH_SEP);
    char full_path[1024];

    while (dir) {
        snprintf(full_path, sizeof(full_path), "%s/%s%s", dir, args, EXE_EXT);
        if (access(full_path, X_OK) == 0) {
            printf("%s is %s\n", args, full_path);
            found = 1;
            goto cleanup;
        }
        dir = strtok(NULL, PATH_SEP);
    }

cleanup:
    if (!found) {
        printf("%s: not found\n", args);
    }
    if (path_copy) free(path_copy);
}