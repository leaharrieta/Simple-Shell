/*
file: main.c
author: Leah Arrieta
email: leaha3@umbc.edu
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

#define TRUE 1
#define MAX_ARGS 256 // For parse_command

// Function prototype definitions
void user_prompt_loop();
char * get_user_command();
char ** parse_command(char * input);
void execute_command(char ** args);

// Proc file functions
void proc_cpuinfo();
void proc_loadavg();
void proc_pid_status(char * pid);
void proc_pid_environ(char * pid);
void proc_pid_sched(char * pid);

// Helper functions
void save_history(char * input);
void display_history();

int main(int argc, char **argv){
    /*
    Writes the main function that checks the number of argument passed to ensure 
    no command-line arguments are passed; if the number of argument is greater 
    than 1 then exit the shell with a message to stderr and return value of 1
    otherwise call the user_prompt_loop() function to get user input repeatedly 
    until the user enters the "exit" command.
    */

    if(argc > 1){
        fprintf(stderr, "%s\n", "Error: Command-line arguments can not be passed.");
        return 1;
    }

    // Create the .421sh file to hold command history
    FILE * file = fopen(".421sh", "w");
    if (file == NULL){
        fprintf(stderr, "Error: History file .421sh could not be made.");
    } else{
        fclose(file);
    }

    // Proceed to get user input
    user_prompt_loop();
    return 0;
}

/*
void user_prompt_loop():
    Gets the user input using a loop until the user exits, prompting the user for a command.
    Gets command and sends it to a parser, then compares the first element to the two
    different commands ("/proc", and "exit"). If it's none of the commands, 
    send it to the execute_command() function. If the user decides to exit, then exit 0 or exit 
    with the user given value. 
*/

void user_prompt_loop(){
    char * input;   // Holds the input command
    char ** parse;  // Holds the parsed command
    int i;          // Integer used in for loop

    // Loop until the user enters "exit"
    while (1){
        // Prompt the user
        printf(">> ");
        input = get_user_command();

        // If no input is entered, print error message and prompt again
        if (!input){
            fprintf(stderr, "Error: No input entered.\n");
            continue;   // exit(0);
        }

        // Save the user input from the command line
        save_history(input);

        // Parse the user command into an array
        parse = parse_command(input);
        // If the command is empty
        if (!parse || parse[0] == NULL){
            free(input);
            continue;
        }

        // Check if user entered "exit"
        else if (strncmp(parse[0], "exit", 4) == 0){
            // Free allocated memory
            free(input);
            // Iterate to free every section of parse
            for (i = 0; parse[i] != NULL; i++){
                free(parse[i]);
            }
            free(parse);
            // Exit the shell
            exit(0);
        } 
        // Check if user entered "/proc"
        else if (strncmp(parse[0], "/proc", 5) == 0){
            // save_history(input);
            if (parse[1] == NULL){
                fprintf(stderr, "Error: No argument provided for /proc.\n");
            } else{
                // Handle specific /proc queries
                if (strcmp(parse[1], "cpuinfo") == 0){
                    proc_cpuinfo();
                } else if (strcmp(parse[1], "loadavg") == 0){
                    proc_loadavg();
                } else if (parse[2] && strcmp(parse[2], "status") == 0){
                    proc_pid_status(parse[1]);
                } else if (parse[2] && strcmp(parse[2], "environ") == 0){
                    proc_pid_environ(parse[1]);
                } else if (parse[2] && strcmp(parse[2], "sched") == 0){
                    proc_pid_sched(parse[1]);
                }
            }
        } 
        // Check if the user entered "history"
        else if (strncmp(parse[0], "history", 7) == 0){
            display_history();
        }
        // Otherwise, execute the command
        else{
            // save_history(input);
            execute_command(parse);
        }
        
        // Free allocated memory
        free(input);
        for (i = 0; parse[i] != NULL; i++){
            free(parse[i]);
        }
        free(parse);
    }
}

/*
get_user_command():
Take input of arbitrary size from the user and return to the user_prompt_loop()
*/
char * get_user_command(){
    // Create a temporary buffer
    char buffer[256];
    char *input;

    // Read a line from stdin
    if (fgets(buffer, sizeof(buffer), stdin) == NULL){
        return NULL;
    }

    // Remove trailing newline if present
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
    }

    // Allocate the necessary amount of space for the string
    input = malloc(len + 1);
    if (input == NULL){
        fprintf(stderr, "Error: Memory allocation for get_user_command failed\n");
        return NULL;
    }

    // Copy the input from buffer to allocated memory
    strcpy(input, buffer);

    return input;
}

/*
parse_command():
Take command grabbed from the user and parse appropriately.
*/
char **parse_command(char *input){
    // Allocate space using MAX_ARGS + 1 as the size limit
    // * sizeof(char *) is to allocate the proper amt of bytes
    char **word = malloc((MAX_ARGS + 1) * sizeof(char *));
    // Counter for the amount of tokens
    int count = 0;

    // Check if allocation worked
    if (word == NULL){
        return NULL;
    }

    // Split the input into tokens and get the first token
    char *token = strtok(input, " ");
    while (token && count < MAX_ARGS){
        // Allocate space for each token and copy it into the array
        word[count++] = strdup(token);
        // Iterate through
        token = strtok(NULL, " ");
    }

    // Make the array NULL
    word[count] = NULL;
    return word;
}

/*
execute_command():
Execute the parsed command if the commands are neither /proc nor exit;
fork a process and execute the parsed command inside the child process
*/
void execute_command(char **args){
    // Create a child process
    pid_t rc = fork();

    if (rc > 0){
        // Run in the parent process
        int status;
        waitpid(rc, &status, 0);
    } else if (rc == 0){
        // Run in the child process
        // Execute command
        execvp(args[0], args);
        fprintf(stderr, "Error: Command not found: %s\n", args[0]);
        exit(1);
    } else{
        // rc = 0, an error has occured
        fprintf(stderr, "Error: Failed to create process\n");
        return;
    }
}

/*
proc_cpuinfo();
    Gets the cpu information if the input is /proc/cpuinfo:
    - Cpu Mhz
    - Cache size
    - Cpu cores
    - Address sizes
*/
void proc_cpuinfo(){
    // Open cpuinfo in the proc filesystem to be read from
    FILE *file = fopen("/proc/cpuinfo", "r");
    // Check if the file exists/is open
    if (file == NULL){
        fprintf(stderr, "Error: Unable to open /proc/cpuinfo.");
        return;
    }

    // Allocate memory to store each line read
    char buffer[256];
    // Read the file
    // Store in char buffer, read the max length of the buffer, from the file
    if (fgets(buffer, sizeof(buffer), file) != NULL){
        // Display cpu information
        printf("%s\n", buffer);
    } else{
        fprintf(stderr, "Error: Unable to read cpu information.");
        return;
    }

    // Close the file
    fclose(file);
}

/*
proc_loadavg()
    Gets the number of currently running processes from /proc/loadavg
*/
void proc_loadavg(){
    // Open file
    FILE *file = fopen("/proc/loadavg", "r");

    if (file == NULL){
        fprintf(stderr, "Error: Unable to open /proc/loadavg.");
        return;
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) != NULL){
        printf("%s\n", buffer);
    } else{
        fprintf(stderr, "Error: Unable to get the number of current running processes.");
        return;
    }

    fclose(file);
}

/*
proc_pid_status()
    Gets the following information from /proc/process_id_no/status
    - the vm size of the virtual memory allocated the vbox 
    - the most memory used vmpeak 
    - the process state
    - the parent pid
    - the number of threads
    - number of voluntary context switches
    - number of involuntary context switches
*/
void proc_pid_status(char * pid){
    // Make a buffer for pid to store the path of the opened file
    char pid_buff[256];
    // Create the filepath, /proc/[pid]/status
    snprintf(pid_buff, sizeof(pid_buff), "/proc/%s/status", pid);

    char buffer[256];
    // Open the file
    FILE *file = fopen(pid_buff, "r");
    if (fgets(buffer, sizeof(buffer), file) != NULL){
        printf("%s", buffer);
    } else{
        fprintf(stderr, "Error: Unable to open /proc/uptime.");
        return;
    }

    fclose(file);
}

/*
proc_pid_eviron()
    Displays the list of environment variables from /proc/process_id_no/environ
*/
void proc_pid_environ(char * pid){
    // Make a buffer for pid to store the path of the opened file
    char pid_buff[256];
    // Create the filepath, /proc/[pid]/environ
    snprintf(pid_buff, sizeof(pid_buff), "/proc/%s/environ", pid);

    char buffer[256];
    // Open the file
    FILE *file = fopen(pid_buff, "r");
    if (fgets(buffer, sizeof(buffer), file) != NULL){   // fgetc to read by character
        printf("%s", buffer);
    } else{
        fprintf(stderr, "Error: Unable to open %s./n", pid_buff);
        return;
    }

    int file_info = fgetc(file);
    // Iterate until the end of file
    if (file_info != EOF){
        // If theres a null character, replace it with a newline
        if (file_info == '\0'){
            printf("/n");
        } else{
            printf("%c", file_info);
        }
    }

    fclose(file);
}

/*
proc_pid_sched()
    Displays the performance information if the user input is /proc/process_id_no/sched
*/
void proc_pid_sched(char * pid){
    char pid_buff[256];
    snprintf(pid_buff, sizeof(pid_buff), "/proc/%s/sched", pid);
    
    char buffer[256];
    FILE *file = fopen(pid_buff, "r");
    if (fgets(buffer, sizeof(buffer), file) != NULL){
        printf("%s", buffer);
    } else{
        fprintf(stderr, "Error: Unable to open %s./n", pid_buff);
        return;
    }

    fclose(file);
}

/*
void save_history(char * input);
    If the first command is not "history", then append the string into the .421sh file
*/
void save_history(char * input){
    // If the command is "history" then don't save to the file and return
    if (strncmp(input, "history", 7) == 0){
        return;
    }

    // Open file to append into it
    FILE * file = fopen(".421sh", "a");
    // Check if opened
    if (file == NULL){
        fprintf(stderr, "Error: Unable to append to history file .421sh.");
    } else{
        // Place input into file
        fprintf(file, "%s\n", input);
    }
    
    fclose(file);
}

/*
void display_history()
    A helper function to read and print up to the last ten commands from user input
*/
void display_history() {
    char *lines[10];  // Array to store ten lines
    int count = 0;    // Lines read
    char buffer[256]; // Temporary buffer to read lines

    // Initialize array to NULL
    for (int i = 0; i < 10; i++){
        lines[i] = NULL;
    }

    // Open .421sh file to read
    FILE *file = fopen(".421sh", "r");
    // Error if not opened
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to read from history file .421sh.\n");
        return;
    }

    // Read each line from the file
    while (fgets(buffer, sizeof(buffer), file)){
        // Allocate space
        char *line = malloc(strlen(buffer) + 1);

        // If empty, error out
        if (line == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            fclose(file);
            return;
        }

        // Copy the information from the buffer
        strcpy(line, buffer);

        // If there's less than 10 entries, input line
        if (count < 10){
            lines[count++] = line;
        } else{
            // If 10 or more, free the oldest line and shift all history
            free(lines[0]);
            for (int i = 0; i < 9; i++){
                lines[i] = lines[i + 1];
            }
            // Add the new input to the end 
            lines[9] = line;
        }
    }

    fclose(file);

    // Print the command history
    for (int i = 0; i < count; i++){
        printf("%s\n", lines[i]);
        free(lines[i]);
    }
}
