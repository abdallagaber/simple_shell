#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 64

/**
 * main - Simple shell program entry point
 *
 * Return: Always 0 on success, -1 on failure
 */
int main(void)
{
    char buffer[BUFFER_SIZE];
    ssize_t read_chars;
    int status;

    while (1)
    {
        printf("$ ");
        read_chars = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        if (read_chars == -1)
        {
            perror("read");
            return (-1);
        }
        if (read_chars == 0)
        {
            printf("\n");
            return (0);
        }
        buffer[read_chars - 1] = '\0';
        char *args[MAX_ARGS];
        char *arg;
        int i = 0;
        for (arg = strtok(buffer, " "); arg != NULL; arg = strtok(NULL, " "))
        {
            args[i++] = arg;
        }
        args[i] = NULL;

        // Check for built-in commands
        if (strcmp(args[0], "exit") == 0)
        {
            return (0);
        }
        else if (strcmp(args[0], "env") == 0)
        {
            char **env;
            for (env = environ; *env != NULL; env++)
            {
                printf("%s\n", *env);
            }
            continue;
        }

        // Check if command exists
        char *path = getenv("PATH");
        char *dir;
        struct stat st;
        int found = 0;
        for (dir = strtok(path, ":"); dir != NULL; dir = strtok(NULL, ":"))
        {
            char cmd[BUFFER_SIZE];
            snprintf(cmd, BUFFER_SIZE, "%s/%s", dir, args[0]);
            if (stat(cmd, &st) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            fprintf(stderr, "%s: command not found\n", args[0]);
            continue;
        }

        // Fork and execute command
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return (-1);
        }
        else if (pid == 0)
        {
            if (execv(dir, args) == -1)
            {
                perror("execv");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            wait(&status);
        }
    }

    return (0);
}

