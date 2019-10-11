#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 16 pipe segments
#define MAX_PIPE_SEGMENTS 16

// Assume that each segment has at most 256 characters (including NULL)
#define MAX_SEGMENT_LENGTH 256

#define DELIMITER "|"

/*
  Function  Prototypes
 */
void show_prompt();
int get_cmd_line(char *cmdline);
void process_cmd(char *cmdline);
char **tokenize(char *cmdline, char *delimiter, int size);
// void **tokenize(char **argv, char *line, int *numTokens, char *token);


/* The main function implementation */
int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    printf("The shell program (pid=%d) starts\n", getpid());
    while (1)
    {
        show_prompt();
        if (get_cmd_line(cmdline) == -1)
            continue; /* empty line handling */

        process_cmd(cmdline);
    }
    return 0;
}

/* 
    Explanation of implementation of process_cmd

    Step 1: split the input line cmdline with the delimiter "|" to different parts of commands(I modify the split function)
    Step 2: check if the first command is "exit"(here I suggest that there won't be commands with "exit" in pipes)
        Step 2.1: print the shell program information
        Step 2.2: call the exit function to terminate process
    Step 3: count the number of different parts of commands by iteration
    Step 4: copy the origin in and out file descriptor
    Step 5: iterations of execute different parts of commands
        Step 5.1: split the current part with the delimiter " " to get arguments of the command
        Step 5.2: judge if now is the final command to be excuted
            Step 5.2.1 if it is the last one, make the out back to the origin out
            Step 5.2.2 if it is not the last one, open a pipe to connect the first part stdout to the second part stdin
        Step 5.3: fork a child process to excute commands
    Step 6: reset the stdin and stdout to the original status
    Step 7: wait for all processes to end

 */
void process_cmd(char *cmdline)
{
    char **multi_args = tokenize(cmdline, "|", MAX_PIPE_SEGMENTS);
    
    if (strcmp(multi_args[0], "exit") == 0)
    {
        printf("The shell program (pid=%d) ends\n", getpid());
        exit(EXIT_SUCCESS);
    }

    int length = 0;
    while (multi_args[length] != NULL)
    {
        length++;
    }

    pid_t pid, wpid;
    char **args;
    int status;
    int origin_in = dup(0);
    int origin_out = dup(1);
    int fin = dup(0);
    int fout = dup(1);
    int i;
    for (i = 0; i < length; i++)
    {
        args = tokenize(multi_args[i], " ", MAX_CMDLINE_LEN);
        dup2(fin, 0);
        close(fin);
        if (i == length - 1)
        {
            fout = dup(origin_out);
        }
        else
        {
            int pfds[2];
            pipe(pfds);
            fin = pfds[0];
            fout = pfds[1];
        }
        dup2(fout, 1);
        close(fout);

        pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args);
        }    
    }
    dup2(origin_in, 0);
    dup2(origin_out, 1);
    close(origin_in);
    close(origin_out);
    do 
    {
        wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}


void show_prompt()
{
    printf("$> ");
}

int get_cmd_line(char *cmdline)
{
    int i;
    int n;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ') {
        ++i;
    }
    if (i == n) {
        // Empty command
        return -1;
    }
    return 0;
}


char **tokenize(char *cmdline, char *delimiter, int size)
{
    int argc = 0;
    char **argv = malloc(size * sizeof(char *));
    char *token = strtok(cmdline, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    argv[argc++] = NULL;
    return argv;
}

// void **tokenize(char **argv, char *line, int *numTokens, char *delimiter)
// {
//     int argc = 0;
//     char *token = strtok(line, delimiter);
//     while (token != NULL)
//     {
//         argv[argc++] = token;
//         token = strtok(NULL, delimiter);
//     }
//     argv[argc++] = NULL;
//     *numTokens = argc - 1;
// }