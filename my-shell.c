#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tokenizer.h"

#define INPUT_SIZE 1024
#define MAX_TOKENS 100

pid_t childPid = 0;

void executeShell();

void writeToStdout(char *text);

void sigintHandler(int sig);

char **getCommandFromInput();

void registerSignalHandlers();

void killChildProcess();

char* trimWhitespace(char *str);

void redirectionsSTDOUTtoFile(char *filename);

void redirectionsSTDINtoFile(char *filename);

void freeArray(char **array);

int isPipe(char **array);

void executeNonPipeCommand(char **commandArray);

void executePipeCommand(char **commandArray);

char** createArrayOfTokensBeforePipe(char **array);

char** createArrayOfTokensAfterPipe(char **array);

void printArray(char **array);

int containsChar(char *string, int c);

char** redirectionsPipeWriterProcess(char** commandArray, char** args);

char** redirectionsPipeReaderProcess(char** commandArray, char** args);

char** initArrayOfMaxTokens();

int main(int argc, char **argv) {
    registerSignalHandlers();

    while (1) {
        executeShell();
    }

    return 0;
}

/* Sends SIGKILL signal to a child process.
 * Error checks for kill system call failure and exits program if
 * there is an error */
void killChildProcess() {
    if (kill(childPid, SIGKILL) == -1) {
        perror("Error in kill");
        exit(EXIT_FAILURE);
    }
}

/* Signal handler for SIGINT. Catches SIGINT signal (e.g. Ctrl + C) and
 * kills the child process if it exists and is executing. Does not
 * do anything to the parent process and its execution */
void sigintHandler(int sig) {
    if (childPid != 0) {
        // Parent
        killChildProcess();
    }
}


/* Registers SIGALRM and SIGINT handlers with corresponding functions.
 * Error checks for signal system call failure and exits program if
 * there is an error */
void registerSignalHandlers() {
    // Register SIGINT (Ctl+D)
    if (signal(SIGINT, sigintHandler) == SIG_ERR) {
        perror("Error in signal");
        exit(EXIT_FAILURE);
    }
}

/* Prints the shell prompt and waits for input from user.
 * if there is a valid command. It then creates a child process which
 * executes the command with its arguments.
 *
 * The parent process waits for the child. On unsuccessful completion,
 * it exits the shell. */
void executeShell() {
    char minishell[] = "my-shell> ";
    writeToStdout(minishell);

    // get array of command from user input
    char** commandArray = getCommandFromInput();

    // check if the command contains pipe '|'
    if (isPipe(commandArray)) {
        executePipeCommand(commandArray);
    } else {
        executeNonPipeCommand(commandArray);
    }

    // free commandArray
    freeArray(commandArray);
}

/* Writes particular text to standard output */
void writeToStdout(char *text) {
    if (write(STDOUT_FILENO, text, strlen(text)) == -1) {
        perror("Error in write");
        exit(EXIT_FAILURE);
    }
}

/* Reads input from standard input till it reaches a new line character.
 * Checks if EOF (Ctrl + D) is being read and exits penn-shredder if that is the case
 * Otherwise, it checks for a valid input and adds the characters to an input buffer.
 *
 * From this input buffer, the first 1023 characters (if more than 1023) or the whole
 * buffer are assigned to command and returned. An \0 is appended to the command so
 * that it is null terminated */
char **getCommandFromInput() {
    char buffer[INPUT_SIZE];
    ssize_t bytesRead;

    // Initialize the buffer with null characters
    memset(buffer, 0, sizeof(buffer));

    bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer)-1);
    if (bytesRead < 0) {
        // Error reading input
        return NULL;

    // Check for a EOF (Contrl-D) without text
    } else if (bytesRead == 0) {
        // Add a newline to the console to format
        write(STDOUT_FILENO, "\n", 1);
        exit(0);
    }

    // Check for a newline character (Enter key)
    char * ptr;
    ptr = strchr(buffer, '\n');
    if (ptr != NULL) {
        *ptr = '\0';
    } else {
        // User entered EOF after some text
        // Add a newline to the console to format
        write(STDOUT_FILENO, "\n", 1);
    }

    // Trim whitespace
    char* string = trimWhitespace(buffer);

    // Initialize tokenizer
    TOKENIZER *tokenizer = init_tokenizer(string);

    if (tokenizer == NULL) {
        perror("error: empty tokenizer");
    }

    // Allocate memory
    char **commandArray = initArrayOfMaxTokens();

    if (commandArray == NULL) {
        perror("error: memory allocation failed.");
    }

    int tokenIndex = 0;
    char *token;

    // Get each token
    while ( (token = get_next_token(tokenizer)) != NULL) {

        // Store each token into commandArray
        commandArray[tokenIndex] = token;
        tokenIndex++;
        
        if (tokenIndex >= MAX_TOKENS) {
            perror("max number of tokens.");
            exit(1);
        }
    }

    // Add Null at the end of the array
    commandArray[tokenIndex] = NULL;

    // free allocated memory
    free_tokenizer(tokenizer);

    return commandArray;
}

/*
Remove leading and trailing whitespace from the given string
*/
char* trimWhitespace(char *str) {
    char *end;

    // Trim leading space
    while(*str == ' ') str++;

    // If it's all whitespace, return
    if(*str == '\0') 
    return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && *end == ' ') end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

/*
Redirect STDOUT of the current process to the given file
*/
void redirectionsSTDOUTtoFile(char *filename){
    // Open the file for writing (create if it doesn't exist, truncate if it exists)
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Invalid standard output redirect: ");
        exit(1);
    }

    // Redirect stdout to the file
    dup2(fd, STDOUT_FILENO);

    // Close the original file descriptor
    close(fd);
}

/*
Redirect STDIN of the current process to the given file
*/
void redirectionsSTDINtoFile(char *filename){
    // Open the file for reading
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Invalid standard input redirect: ");
        exit(1);
    }

    // Redirect stdin to the file
    dup2(fd, STDIN_FILENO);

    // Close the original file descriptor
    close(fd);
}

/*
Given an array of string, free all the allocated elements
*/
void freeArray(char **array) {
    int i = 0;
    while (array[i] != NULL) {
        free(array[i]);
        i++;
    }
    free(array);
}

/*
Given an array of string, return 1 if there is a pipe "|", otherwise returns 0.
*/
int isPipe(char **array) {
    int i = 0;
    while (array[i] != NULL) {
        char * ptr;
        int pipe = '|';
        ptr = strchr(array[i], pipe);
        if (ptr != NULL) {
            return 1;
        }
        i++;
    }
    return 0;
}

/*
Check for redirections (>, <) in the given commandArray, redirects STDIN / STDOUT to files using dup2(),
and execute commands
*/
void executeNonPipeCommand(char **commandArray) {
    char *command; // store the first command
    // flag to indicate if we already have an output redirection
    int hasOutRedirection = 0;
    int hasInRedirection = 0;
    
    if (commandArray[0] != NULL) {
        // Get the first command
        command = commandArray[0];

        // Create a child process for executing command
        childPid = fork();

        if (childPid < 0) {
            perror("error in creating child process");
            freeArray(commandArray);
            exit(EXIT_FAILURE);
        }

        if (childPid == 0) {
            // Child

            // Ignore SIGINT in child process
            signal(SIGINT, SIG_IGN);

            // Initialize the index of command array
            int m = 1;

            // Initialize args array
            char* args[MAX_TOKENS] = {NULL};
            args[0] = command;
            int argc = 1;

            while (commandArray[m] != NULL) {
                if (*commandArray[m] == '>') {
                    if (hasOutRedirection == 1) {
                        perror("Invalid: Multiple standard output redirects");
                        freeArray(commandArray);
                        exit(1);
                    }
                    hasOutRedirection = 1;
                    m++;
                    if (commandArray[m] == NULL) {
                        perror("Invalid: No file name for output redirect");
                        freeArray(commandArray);
                        exit(1);
                    }
                    char *nextToken = commandArray[m];
                    redirectionsSTDOUTtoFile(nextToken);
                } else if (*commandArray[m] == '<') {
                    if (hasInRedirection == 1) {
                        perror("Invalid: Multiple standard input redirects or redirect in invalid location");
                        freeArray(commandArray);
                        exit(1);
                    }
                    hasInRedirection = 1;
                    m++;
                    if (commandArray[m] == NULL) {
                        perror("Invalid: No file name for input redirect");
                        freeArray(commandArray);
                        exit(1);
                    }
                    char *nextToken = commandArray[m];
                    redirectionsSTDINtoFile(nextToken);
                } else {
                    // Populate args array
                    args[argc] = commandArray[m];
                    argc++;
                }
                m++;
            }

            // execute command
            if (execvp(args[0], args) == -1) {
                perror("Error in execve");
                freeArray(commandArray);
                exit(EXIT_FAILURE);
            }

        } else {
            // Parent

            // wait
            int status;
            do {
                if (wait(&status) == -1) {
                    perror("Error in child process termination");
                    freeArray(commandArray);
                    exit(EXIT_FAILURE);
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));

            // Set childPid to 0 because there is no child running
            childPid = 0;
        }
    }
}

void executePipeCommand(char **commandArray) {
    // Divide the commandArray by pipe '|' and create two arrays
    char** commandArray1 = createArrayOfTokensBeforePipe(commandArray);
    char** commandArray2 = createArrayOfTokensAfterPipe(commandArray);

    // Create pipes for communication between child processes
    int fd[2];
    pipe(fd);

    // Create one child process for the command before pipe
    pid_t childPid1 = fork();
    if (childPid < 0) {
            perror("error in creating child process");
            freeArray(commandArray);
            freeArray(commandArray1);
            freeArray(commandArray2);
            exit(EXIT_FAILURE);
        }

    if (childPid1 == 0) {
        // Allocate memory for args1
        char** args1 = initArrayOfMaxTokens();

        redirectionsPipeWriterProcess(commandArray1, args1);

        // Close read end
        close(fd[0]);

        // Pipe output redirection
        dup2(fd[1], STDOUT_FILENO);

        // Close duplicate write end
        close(fd[1]);

        // execute command stored in args1
        execvp(args1[0], args1);

        free(commandArray1);
        free(args1);
    }

    // Create another child process for the command after pipe
    pid_t childPid2 = fork();
    if (childPid < 0) {
            perror("error in creating child process");
            freeArray(commandArray);
            freeArray(commandArray1);
            freeArray(commandArray2);
            exit(EXIT_FAILURE);
        }

    if (childPid2 == 0) {
        // Allocate memory for args2
        char** args2 = initArrayOfMaxTokens();

        redirectionsPipeReaderProcess(commandArray2, args2);

        // Close write end
        close(fd[1]);

        // Pipe input redirection
        dup2(fd[0], STDIN_FILENO);

        // Close duplicate read end
        close(fd[0]);

        // execute command stored in args2
        execvp(args2[0], args2);

        free(commandArray2);
        free(args2);
    }

    // Parent

    // close both pipe ends in parent process
    close(fd[0]);
    close(fd[1]);

    // int status1, status2 = 0;

    // // Wait for both child processes
    // waitpid(childPid1, &status1, 0);
    // waitpid(childPid2, &status2, 0);

    // // Check if child 1 terminated
    // if (WIFEXITED(status1)) {
    //     // printf("Child 1 (PID: %d) terminated.\n", childPid1);
    //     childPid1 = 0;
    // }

    // // Check if child 2 terminated
    // if (WIFEXITED(status2)) {
    //     // printf("Child 2 (PID: %d) terminated.\n", childPid2);
    //     childPid2 = 0;
    // }

    // wait for all child processes to finish
    int status;
    while (waitpid(-1, &status, 0) > 0) {
        // do nothing
    }
    childPid1 = 0;
    childPid2 = 0;
}

/*
Given an array of string, create an array that contains the subarray of string before pipe '|'.
Don't forget to free the returned array of string after use.
*/
char** createArrayOfTokensBeforePipe(char **array) {
    // Allocate memory
    char **newArray = initArrayOfMaxTokens();

    // copy the array to newArray until we reach pipe
    int i = 0;
    while (array[i] != NULL) {
        if (containsChar(array[i], '|')){
            break;
        }
        newArray[i] = array[i];
        i++;
    }
    return newArray;
}

/*
Given an array of string, create an array that contains the subarray of string after pipe '|'.
Don't forget to free the returned array of string after use.
*/
char** createArrayOfTokensAfterPipe(char **array) {
    // Allocate memory
    char **newArray = initArrayOfMaxTokens();

    int i = 0;
    
    // increment i until we find pipe '|'
    while (array[i] != NULL && !containsChar(array[i], '|')) {
        i++;
    }
    i++;

    // copy the array after pipe to newArray
    int j = 0;
    while (array[i] != NULL) {
        newArray[j] = array[i];
        i++;
        j++;
    }
    return newArray;
}

/*
Given an array of string, prints all the contents to the console.
*/
void printArray(char **array) {
    int i = 0;
    while (array[i] != NULL) {
        char* text = array[i];
        if (write(STDOUT_FILENO, text, strlen(text)) == -1) {
            perror("Error in write");
            exit(EXIT_FAILURE);
        }
        writeToStdout(" ");
        i++;
    }
}

/*
Given a string and a char, return 1 if string contains char, return 0 otherwise.
*/
int containsChar(char *string, int c) {
    char * ptr;
    ptr = strchr(string, c);
    if (ptr != NULL) {
        return 1;
    } else {
        return 0;
    }
}

/*
Given commandArray, an array of command before pipe '|',
check for input redirection (<) and redirect using dup2().
Return args, an array of command without redirection command
*/
char** redirectionsPipeWriterProcess(char** commandArray, char** args) {
    int m = 0; // index for commandArray
    int i = 0; // index for args
    int hasInRedirection = 0; // flag

    while (commandArray[m] != NULL) {
        if (*commandArray[m] == '>') {
            perror("Invalid: Output redirects before pipe");
            freeArray(commandArray);
            exit(1);
        } else if (*commandArray[m] == '<') {
            if (hasInRedirection == 1) {
                perror("Invalid: Multiple standard input redirects or redirect in invalid location");
                freeArray(commandArray);
                exit(1);
            }
            hasInRedirection = 1;
            m++;
            if (commandArray[m] == NULL) {
                perror("Invalid: No file name for input redirect");
                freeArray(commandArray);
                exit(1);
            }
            char *nextToken = commandArray[m];
            redirectionsSTDINtoFile(nextToken);
        } else {
            // Populate args array
            args[i] = commandArray[m];
            i++;
        }
        m++;
    }
    return args;
}

/*
Given commandArray, an array of command after pipe '|',
check for output redirection (>) and redirect using dup2().
Return args, an array of command without redirection command
*/
char** redirectionsPipeReaderProcess(char** commandArray, char** args) {
    int m = 0; // index for commandArray
    int i = 0; // index for args
    int hasOutRedirection = 0; // flag

    while (commandArray[m] != NULL) {
        if (*commandArray[m] == '>') {
            if (hasOutRedirection == 1) {
                perror("Invalid: Multiple standard output redirects");
                freeArray(commandArray);
                exit(1);
            }
            hasOutRedirection = 1;
            m++;
            if (commandArray[m] == NULL) {
                perror("Invalid: No file name for output redirect");
                freeArray(commandArray);
                exit(1);
            }
            char *nextToken = commandArray[m];
            redirectionsSTDOUTtoFile(nextToken);
        } else if (*commandArray[m] == '<') {
            perror("Invalid: output redirects after pipe");
            freeArray(commandArray);
            exit(1);
        } else {
            // Populate args array
            args[i] = commandArray[m];
            i++;
        }
        m++;
    }
    return args;
}

/*
Allocate memory for an array of string of length MAX_TOKENS
Return the pointer to the array of string
Don't forget to free after use
*/
char** initArrayOfMaxTokens(){
    // Allocate memory for array
    char** array = malloc(MAX_TOKENS * sizeof(char*));

    if (array == NULL) {
        perror("malloc failed");
        exit(1);
    }

    // Initialize with NULL
    for (int i = 0; i < MAX_TOKENS; i++) {
        array[i] = NULL;
    }
    return array;
}