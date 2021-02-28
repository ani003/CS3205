#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_IN 100

int main(int argc, char* argv[]) {
    printf("Server started\n");
    char user_in[MAX_IN];
    char* token;
    char* commands[2];

    while(1) {
        printf("Server-prompt> ");
        fgets(user_in, MAX_IN, stdin);  //Read user input
        token = strtok(user_in, "\n");  //Strip new line
        
        //Extract space separated tokens
        token = strtok(token, " ");
        if(token != NULL) {
            commands[0] = (char*)malloc(strlen(token) + 1);
            strcpy(commands[0], token);

            token = strtok(NULL, " ");
            if(token != NULL) {
                commands[1] = (char*)malloc(strlen(token) + 1);
                strcpy(commands[1], token);
            }
        }
        else {
            //Nothing entered
            continue;
        }

        //Check the command
        if(strcmp(commands[0], "LSTU") == 0) {
            printf("LSTU: List all users\n");
        }
        else if(strcmp(commands[0], "ADDU") == 0) {
            printf("ADDU: Add user %s\n", commands[1]);
        }
        else if(strcmp(commands[0], "USER") == 0) {
            printf("USER: Switch to user %s\n", commands[1]);
        }
        else if(strcmp(commands[0], "READM") == 0) {
            printf("READM: Read the mail\n");
        }
        else if(strcmp(commands[0], "DELM") == 0) {
            printf("DELM: Delete the mail\n");
        }
        else if(strcmp(commands[0], "SEND") == 0) {
            printf("SEND: Send a mail to %s\n", commands[1]);
        }
        else if(strcmp(commands[0], "DONEU") == 0) {
            printf("DONEU: Done with the user\n");
        }
        else if(strcmp(commands[0], "QUIT") == 0) {
            printf("QUIT: Exiting\n");
            break;
        }
    }

    return 0;
}