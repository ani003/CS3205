#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_IN 1000
#define MAX_MSG (MAX_IN + 100)

int main(int argc, char* argv[]) {
    char user_in[MAX_IN];
    char* token;
    char* commands[2];
    int user_flag = 0;
    char curr_user_id[100];

    while(1) {
        //Set appropriate prompt message
        if(user_flag) {
            printf("Sub-Prompt-%s> ", curr_user_id);
        }
        else {
            printf("Main-Prompt> ");
        }
        
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
        if(strcmp(commands[0], "Listusers") == 0) {
            //List all users
            printf("LSTU\n");
            
        }
        else if(strcmp(commands[0], "Adduser") == 0) {
            //Add new user
            printf("ADDU %s\n", commands[1]);
        }
        else if(strcmp(commands[0], "SetUser") == 0) {
            //Set the current user to the given user
            printf("USER %s\n", commands[1]);
            strcpy(curr_user_id, commands[1]);
            user_flag = 1;
        }
        else if(strcmp(commands[0], "Read") == 0) {
            //Read the mail pointed to by the read pointer
            printf("READM\n");
        }
        else if(strcmp(commands[0], "Delete") == 0) {
            //Delete the mail pointed to by the read pointer
            printf("DELM\n");
        }
        else if(strcmp(commands[0], "Send") == 0) {
            //Send a new mail from current user to the given user
            char msg[MAX_MSG];
            strcpy(msg, "");
            printf("Type message:\n");
            char str[MAX_IN];
            strcpy(str, "");

            //Read input from stdin and write it, until "###" is encountered
            while(!strstr(str, "###")) {
                fgets(str, MAX_IN, stdin);
                strcat(msg, str);
            }
            msg[strlen(msg) - 1] = '\0';
            printf("SEND %s %s\n", commands[1], msg);
        }
        else if(strcmp(commands[0], "Done") == 0) {
            //Reset the current user
            printf("DONEU\n");
            user_flag = 0;
            strcpy(curr_user_id, "");
        }
        else if(strcmp(commands[0], "Quit") == 0) {
            //Stop the client and server
            printf("QUIT\n");
            break;
        }
    }

    return 0;
}