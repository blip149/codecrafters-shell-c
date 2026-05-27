#include "command.h"
#include <stdbool.h>
#define bool _Bool

static const char* builtins[] = {"echo", "exit", "type", "quit", "pwd"};

typedef enum{
    NORMAL_STATE,
    IN_DQUOTES,
    IN_SQUOTES,
}State;

static void parse(char* peek_ptr,const char* start){
    if (!peek_ptr) return;

    char* s_ptr = peek_ptr;
    State state = NORMAL_STATE;
    bool escaped = false;

    while(*peek_ptr != '\0'){
        if (escaped){
            if(state == IN_DQUOTES){
                if (*peek_ptr == '\\' || *peek_ptr =='$'|| *peek_ptr == '"'){
                    *s_ptr++ = *peek_ptr;
                }else{
                    *s_ptr++ = '\\';
                    *s_ptr++ = *peek_ptr;
                }
            }else{
                *s_ptr++ = *peek_ptr;
            }
            escaped = false;
            peek_ptr++;
        }else if (state != IN_SQUOTES && *peek_ptr =='\\'){
            escaped = true;
            peek_ptr++;
        }else if(*peek_ptr =='\'' && state != IN_DQUOTES){
            state = (state == IN_SQUOTES) ? NORMAL_STATE:IN_SQUOTES;
            peek_ptr++;
        }else if (*peek_ptr == '"' && state != IN_SQUOTES) {
            state = (state == IN_DQUOTES) ? NORMAL_STATE:IN_DQUOTES;
            peek_ptr++;
        }else {
            if (*peek_ptr == ' ' && state == NORMAL_STATE){
                if (s_ptr != start && *(s_ptr -1) != ' '){
                    *s_ptr++ = *peek_ptr;
                }
                peek_ptr++;
            }else{
            *s_ptr++ = *peek_ptr++;
            }
        }
    }
    *s_ptr = '\0';
}

void parse_command(Command *cmd, const char* input) {
    if (cmd->cmd != NULL) {
        free(cmd->cmd);
        cmd->cmd = NULL;
    }

    strncpy(cmd->raw_input, input, sizeof(cmd->raw_input) - 1);
    cmd->raw_input[strcspn(cmd->raw_input, "\r\n")] = '\0';

    char *first_space = strchr(cmd->raw_input, ' ');

    if (first_space) {
        size_t len = first_space - cmd->raw_input;
        cmd->cmd = malloc(len + 1);
        strncpy(cmd->cmd, cmd->raw_input, len);
        cmd->cmd[len] = '\0';

        char *arg_ptr = first_space + 1;
        while (*arg_ptr == ' ') {
            arg_ptr++;
        }
        cmd->args = (*arg_ptr != '\0') ? arg_ptr : NULL;
    } else {
        cmd->cmd = _strdup(cmd->raw_input);
        cmd->args = NULL;
    }


    parse(cmd->cmd, cmd->cmd);
    parse(cmd->args, cmd->args);
}

int handle_command(Command* cmd) {
    if (!cmd->cmd) return 1;

    if (strcmp(cmd->cmd, "exit")==0 || strcmp(cmd->cmd, "quit")==0){
        return 0;
    }else if (strcmp(cmd->cmd, "echo")==0){
        printf("%s\n", (cmd->cmd)? cmd->args:"");
    }else if (strcmp(cmd->cmd, "type")==0){
        find_path(cmd->args);
    }else if (strcmp(cmd->cmd, "pwd")==0){
        gwd();
    }else if (strcmp(cmd->cmd, "cd")==0){
        cd(cmd->args);
    }
    else{
        run_external_program(cmd->cmd, cmd->args);
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
void run_external_program(const char* cmd, const char* args){
    char* path_copy = NULL;
    int found = 0;

    char* env_path = getenv("PATH");
    if (!env_path) goto cleanup;

    path_copy = _strdup(env_path);
    char *dir = strtok(path_copy, PATH_SEP);
    char full_path[1024];

    while(dir) {
        snprintf(full_path, sizeof(full_path), "%s/%s%s", dir, cmd, EXE_EXT);
        if (access(full_path, X_OK) == 0){
            found = 1;
            #ifdef _WIN32
                STARTUPINFO si;
                PROCESS_INFORMATION pi;

                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                char cmd_line[2048];
                if (args && strlen(args)> 0){
                    snprintf(cmd_line, sizeof(cmd_line), "\"%s\" %s", full_path, args);
                }else{
                    snprintf(cmd_line, sizeof(cmd_line), "\"%s\"", full_path);
                }
                if (CreateProcess(NULL, cmd_line, NULL, NULL, 0,FALSE,NULL, NULL,&si, &pi )){
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }else{
                    printf("failed to execute(%lu).\n", GetLastError());
                }
            #else
                pid_t pid = fork();
                if (pid < 0){
                    perror("fork failed");
                }else if (pid == 0){
                    char *cmd_line[LIMIT];

                    if (args && strlen(args)> 0){
                        snprintf(cmd_line, sizeof(cmd_line), "\"%s\" %s", full_path, args);
                    }else{
                        snprintf(cmd_line, sizeof(cmd_line), "\"%s\"", full_path);
                    }

                    execlp("/bin/sh", "sh", "-c", exe_cmd, (char *)NULL);
                    perror("execution failed");
                    exit(1);
                    // int argc = 0;

                    // argv[argc++] = (char*)cmd;

                    // if (args && strlen(args)>0){
                    //     char *args_copy = _strdup(args);
                    //     char *tokens = strtok(args_copy, " ");

                    //     while (tokens && argc < (LIMIT -1)){
                    //        argv[argc++] = tokens;
                    //        tokens = strtok(NULL," ");
                    //     }
                    // }

                    // printf("Debug: Executing command [%s] with args [%s]\n", cmd, args);
                    // argv[argc] = NULL;
                    // execv(full_path, argv);
                        
                    // perror("execution failed");
                    // exit(1);
                }else{
                    int status;
                    waitpid(pid, &status, 0);
                    }
            #endif
            goto cleanup;
        }
        dir = strtok(NULL, PATH_SEP);
    }
    cleanup:
        if (!found){
            printf("%s: command not found\n", cmd);
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