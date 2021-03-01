#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_IN 1000
#define MAX_MSG (MAX_IN + 100)

struct userNode {
    char* id;
    int num_messages;
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
    new_node->num_messages = 0;
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

userNode* exists_user(char* user) {
    userNode* curr;
    curr = head;
    while(curr != NULL) {
        if(strcmp(curr->id, user) == 0) {
            return curr;
        }

        curr = curr->next;
    }
    
    return NULL;
}

void send_message(char* sender, userNode* receiver) {
    char* msg;
    msg = (char*)malloc(MAX_MSG);
    strcpy(msg, "From: ");
    strcat(msg, sender);
    strcat(msg, "\nTo: ");
    strcat(msg, receiver->id);
    strcat(msg, "\nMessage:\n");

    printf("Type your message:\n");
    char str[MAX_IN];
    char* end_msg;

    while(1) {
        fgets(str, MAX_IN, stdin);
        if((end_msg = strstr(str, "###"))) {
            int msg_len = strlen(msg);
            for(int i = 0; (str + i) != end_msg; i++){
                msg[msg_len + i] = str[i];
                msg[msg_len + i + 1] = '\0';
            }
            
            msg_len = strlen(msg);
            if(msg[msg_len - 1] != '\n') {
                msg[msg_len] = '\n';
                msg[msg_len + 1] = '\0';
            }

            strcat(msg, "###\n");
            break;
        }
        else {
            strcat(msg, str);
        }
    }

    FILE* fp;
    char file_name[100];
    strcpy(file_name, "MAILSERVER/");
    strcat(file_name, receiver->id);
    fp = fopen(file_name, "a");
    fputs(msg, fp);
    fclose(fp);
    receiver->num_messages++;
    return;
}

FILE* read_mail(userNode* user, FILE* read_fp) {
    if(user->num_messages == 0) {
        printf("No more mail\n");
        return read_fp;
    }

    if(read_fp == NULL) {
        char file_name[100];
        strcpy(file_name, "MAILSERVER/");
        strcat(file_name, user->id);
        read_fp = fopen(file_name, "r");
    }

    char msg[MAX_MSG], tmp[MAX_IN];
    strcpy(msg, "");
    fgets(tmp, MAX_MSG, read_fp);
    while(!strstr(tmp, "###")) {
        strcat(msg, tmp);
        fgets(tmp, MAX_MSG, read_fp);
    }

    printf("%s", msg);
    return read_fp;
}

FILE* delete_mail(userNode* user, int msg_count) {
    if(user->num_messages == 0) {
        printf("No more mail\n");
        return NULL;
    }

    FILE *read_fp, *write_fp;

    char file_name[100];
    strcpy(file_name, "MAILSERVER/");
    strcat(file_name, user->id);
    read_fp = fopen(file_name, "r");

    char temp_file_name[100];
    strcpy(temp_file_name, "MAILSERVER/.");
    strcat(temp_file_name, user->id);
    write_fp = fopen(temp_file_name, "w");

    //Navigate to the message
    int count = 0;
    char tmp[MAX_IN];
    while(count < msg_count) {
        fgets(tmp, MAX_MSG, read_fp);
        while(!strstr(tmp, "###")) {
            fprintf(write_fp, "%s", tmp);
            fgets(tmp, MAX_MSG, read_fp);
        }
        fprintf(write_fp, "%s", tmp);
        count++;
    }
    
    //Delete the message
    fgets(tmp, MAX_MSG, read_fp);
    while(!strstr(tmp, "###")) {
        fgets(tmp, MAX_MSG, read_fp);
    }
    user->num_messages--;

    //Write the remaining
    while(count < user->num_messages) {
        fgets(tmp, MAX_MSG, read_fp);
        while(!strstr(tmp, "###")) {
            fprintf(write_fp, "%s", tmp);
            fgets(tmp, MAX_MSG, read_fp);
        }
        fprintf(write_fp, "%s", tmp);
        count++;
    }

    //Remove old file and rename temp file
    fclose(read_fp);
    fclose(write_fp);
    remove(file_name);
    rename(temp_file_name, file_name);

    //Set the read pointer to old position
    int new_msg_count = msg_count;
    if(msg_count >= user->num_messages) {
        new_msg_count = 0;
    }

    count = 0;
    read_fp = fopen(file_name, "r");
    while(count < new_msg_count) {
        fgets(tmp, MAX_MSG, read_fp);
        while(!strstr(tmp, "###")) {
            fgets(tmp, MAX_MSG, read_fp);
        }
        count++;
    }

    printf("Message deleted\n");
    return read_fp;
}

int main(int argc, char* argv[]) {
    printf("Server started\n");
    char user_in[MAX_IN];
    char* token;
    char* commands[2];
    userNode* curr_user = NULL;
    FILE* read_fp = NULL;
    int msg_count = 0;

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

            curr_user = exists_user(commands[1]);
            if(curr_user == NULL) {
                printf("User %s does not exist\n", commands[1]);
            }
            else {
                printf("User %s exists. Totally %d messages in spool file\n", commands[1], curr_user->num_messages);
                read_fp = NULL;
                msg_count = 0;
            }
        }
        else if(strcmp(commands[0], "READM") == 0) {
            printf("READM: Read the mail\n");

            if(curr_user == NULL) {
                printf("No user selected. Please select the user\n");
            }
            else {
                read_fp = read_mail(curr_user, read_fp);
                if(++msg_count == curr_user->num_messages) {
                    fseek(read_fp, 0, SEEK_SET);
                    msg_count = 0;
                }
            }
        }
        else if(strcmp(commands[0], "DELM") == 0) {
            printf("DELM: Delete the mail\n");

            if(curr_user == NULL) {
                printf("No user selected. Please select the user\n");
            }
            else {
                read_fp = delete_mail(curr_user, msg_count);
                if(msg_count >= curr_user->num_messages) {
                    msg_count = 0;
                }
            }

        }
        else if(strcmp(commands[0], "SEND") == 0) {
            printf("SEND: Send a mail to %s\n", commands[1]);

            if(curr_user == NULL) {
                printf("No sending user selected. Please select the sending user\n");
            }
            else {
                userNode* receiver = NULL;
                receiver = exists_user(commands[1]);
                if(receiver == NULL) {
                    printf("Receiver %s does not exist\n", commands[1]);
                }
                else {
                    send_message(curr_user->id, receiver);
                }
            }
        }
        else if(strcmp(commands[0], "DONEU") == 0) {
            printf("DONEU: Done with the user\n");
            curr_user = NULL;
            if(read_fp !=NULL) {
                fclose(read_fp);
            }
            read_fp = NULL;
            msg_count = 0;
        }
        else if(strcmp(commands[0], "QUIT") == 0) {
            printf("QUIT: Exiting\n");
            break;
        }
    }

    return 0;
}