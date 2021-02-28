#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_IN 100

struct userNode {
    char* id;
    struct userNode* next;
};

typedef struct userNode userNode;

userNode* head = NULL;
userNode* tail = NULL;

int add_user(char* user_id){
    //Check if the user is already present
    userNode* curr;
    curr = head;
    while(curr != NULL) {
        if(strcmp(curr->id, user_id) == 0)
            return 1;

        curr = curr->next;
    }

    //Add the new user
    userNode* new_node;
    new_node = (userNode*)malloc(sizeof(userNode));
    new_node->id = (char*)malloc(sizeof(user_id) + 1);
    new_node->next = NULL;
    strcpy(new_node->id, user_id);

    if(head == NULL) {
        head = new_node;
        tail = new_node;
    }
    else {
        tail->next = new_node;
        tail = new_node;
    }

    //Open a new file for this user
    FILE* fp;
    char file_name[100];
    strcpy(file_name, "MAILSERVER/");
    strcat(file_name, user_id);
    fp = fopen(file_name, "w");
    fclose(fp);
    return 0;
}

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

            userNode* curr;
            curr = head;

            while (curr != NULL)
            {
                printf("%s ", curr->id);
                curr = curr->next;
            }
            printf("\n");
            
        }
        else if(strcmp(commands[0], "ADDU") == 0) {
            printf("ADDU: Add user %s\n", commands[1]);

            struct stat st = {0};
            if (stat("MAILSERVER", &st) == -1) {
                mkdir("MAILSERVER", 0777);
            }

            if(add_user(commands[1])) {
                printf("User already present\n");
            }
            else {
                printf("User added successfully\n");
            }
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