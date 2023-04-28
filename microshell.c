#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#define STDERR(msg) write(2, msg, strlen(msg))

void error_fatal() {
    STDERR("error: fatal\n");
    exit(1);
}

int cd_builtin(char **args) {
    if (!args[1] || args[2]) {
        STDERR("error: cd: bad arguments\n");
        return (1);
    }
    if (chdir(args[1])) {
        STDERR("error: cd: cannot change directory to ");
        STDERR(args[1]);
        STDERR("\n");
        return (1);
    }
    return (0);
}

int exec_cmd(char **args, char **env) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) error_fatal();
    if (pid == 0) {
        if (execve(args[0], args, env) < 0) {
            STDERR("error: cannot execute ");
            STDERR(args[0]);
            STDERR("\n");
            exit(1);
        }
    } else {
        if (waitpid(pid, &status, 0) < 0) error_fatal();
    }
    return (0);
}

int parse_execute(char **args, char **env) {
    if (!strcmp(args[0], "cd")) return cd_builtin(args);
    return exec_cmd(args, env);
}

int main(int argc, char **argv, char **env) {
    int i, j;
    char **args;

    if (argc <= 1) return (0);
    args = (char **)malloc(sizeof(char *) * (argc + 1));
    if (!args) error_fatal();
    i = 1;
    while (i < argc) {
        j = 0;
        while (i < argc && strcmp(argv[i], ";") && strcmp(argv[i], "|")) {
            args[j++] = argv[i++];
        }
        args[j] = NULL;
        if (!j) {
            STDERR("error: syntax error\n");
            return (1);
        }
        parse_execute(args, env);
        if (i < argc && !strcmp(argv[i], "|")) {
            STDERR("error: not implemented\n");
            return (1);
        }
        ++i;
    }
    free(args);
    return (0);
}
