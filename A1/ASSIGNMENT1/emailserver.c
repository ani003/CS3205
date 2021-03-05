#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#define MAX_IN 1000
#define MAX_MSG (MAX_IN + 100)
#define SA struct sockaddr

//Linked list to store user information
struct userNode {
    char* id;
    int num_messages;
    struct userNode* next;
};

typedef struct userNode userNode;

userNode* head = NULL;
userNode* tail = NULL;

//Used for command processing
userNode* curr_user = NULL;
FILE* read_fp = NULL;
int msg_count = 0;

struct readRet {
    char* msg;
    FILE* fp;
};

typedef struct readRet readRet;

//Auxilliary functions
int add_user(char* user_id);
userNode* exists_user(char* user);
void send_mail(char* sender, userNode* receiver, char* mail_body);
readRet* read_mail(userNode* user, FILE* read_fp);
FILE* delete_mail(userNode* user, int msg_count);
void socket_func(int sockfd);
char* process_commands(char* inp);

int main(int argc, char* argv[]) {
    //Process command line arguments
    if(argc < 2) {
        printf("Too few arguments\n");
        printf("Input format: %s <port-no>\n", argv[0]);
        exit(0);
    }

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    //Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Assign IP, PORT
    int port_no = atoi(argv[1]);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port_no);

    //Setup the socket
    bind(sockfd, (SA*)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);
    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);

    //Listen and respond to the client
    socket_func(connfd);

    //Close the socket
    close(sockfd);

    return 0;
}

/**
 * Add a new user to the linked list, if not present
 * Returns 1 if already present, 0 otherwise
*/
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

/**
 * Check if the given user exists
 * Returns pointer to the user's node, if exists
*/
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

/**
 * Send a mail from sender to receiver
*/
void send_mail(char* sender, userNode* receiver, char* mail_body) {
    //Initialise preamble
    char msg[MAX_MSG];
    bzero(msg, MAX_MSG);
    strcpy(msg, "From: ");
    strcat(msg, sender);
    strcat(msg, "\nTo: ");
    strcat(msg, receiver->id);

    //Get date and time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char date_time[100];
    strcpy(date_time, "");
    sprintf(date_time, "\nDate: %d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    strcat(msg, date_time);

    strcat(msg, "\nMessage:\n");

    char* end_msg;

    //Process the mail body to a suitable format
    end_msg = strstr(mail_body, "###");
    int msg_len = strlen(msg);
    for(int i = 0; (mail_body + i) != end_msg; i++){
        msg[msg_len + i] = mail_body[i];
    }
    msg[msg_len + strlen(mail_body)] = '\0';
    
    if(msg[strlen(msg) - 1] != '\n') {
        strcat(msg, "\n");
    }

    strcat(msg, "###\n");

    //Write the message to the receiver spool file
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

/**
 * Read the mail pointed to by the read_fp pointer
 * Returns the message read and the new position of the read file pointer
*/
readRet* read_mail(userNode* user, FILE* read_fp) {
    readRet* result;
    result = (readRet*)malloc(sizeof(readRet));
    
    result->msg = (char*)malloc(MAX_MSG);
    if(user->num_messages == 0) {
        //No messages to read
        strcpy(result->msg, "No more mail\n");
        result->fp = read_fp;
        return result;
    }

    //Open file for reading, if not opened
    if(read_fp == NULL) {
        char file_name[100];
        strcpy(file_name, "MAILSERVER/");
        strcat(file_name, user->id);
        read_fp = fopen(file_name, "r");
    }

    //Read the message
    char tmp[MAX_IN];
    strcpy(result->msg, "\n");
    fgets(tmp, MAX_MSG, read_fp);
    while(!strstr(tmp, "###")) {
        strcat(result->msg, tmp);
        fgets(tmp, MAX_MSG, read_fp);
    }

    result->fp = read_fp;

    return result;
}

/**
 * Delete the mail after msg_count mails from the user's spool
 * Returns the new position of read file pointer
*/
FILE* delete_mail(userNode* user, int msg_count) {
    //Open the spool file and the temporary file
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

    return read_fp;
}

/**
 * Send/receive messages to/from the client 
*/
void socket_func(int sockfd) {
    char buff[MAX_IN], inp[MAX_IN];

    while(1) {
        bzero(buff, MAX_IN);

        //Read message from client
        read(sockfd, buff, sizeof(buff));
        strcpy(inp, buff);

        bzero(buff, MAX_IN);
        strcpy(buff, process_commands(inp));
        //Send response back to the client
        write(sockfd, buff, sizeof(buff));

        //If input was QUIT, then quit
        if (strstr(inp, "QUIT")) {
            break;
        }
    }
}

/**
 * Process the command received from the client
 * Returns the message to be sent back to the client  
*/
char* process_commands(char* inp) {
    char in_msg[MAX_IN];
    char* out_msg = (char*)malloc(MAX_IN);
    char* token;
    char* commands[2];
    
    strcpy(in_msg, inp);

    //Extract space separated tokens
    token = strtok(in_msg, " ");
    if(token != NULL) {
        commands[0] = (char*)malloc(strlen(token) + 1);
        strcpy(commands[0], token);

        token = strtok(NULL, " ");
        if(token != NULL) {
            commands[1] = (char*)malloc(strlen(token) + 1);
            strcpy(commands[1], token);
        }
    }

    //Check the command
    if(strcmp(commands[0], "LSTU") == 0) {
        //List all users
        userNode* curr;
        curr = head;

        strcpy(out_msg, "");
        while (curr != NULL) {
            strcat(out_msg, curr->id);
            strcat(out_msg, " ");
            curr = curr->next;
        }
        strcat(out_msg, " \n");
    }
    else if(strcmp(commands[0], "ADDU") == 0) {
        //Add new user
        struct stat st = {0};
        if (stat("MAILSERVER", &st) == -1) {
            mkdir("MAILSERVER", 0777);
        }

        if(add_user(commands[1])) {
            strcpy(out_msg, "User already present\n");
        }
        else {
            strcpy(out_msg, "User added successfully\n");
        }
    }
    else if(strcmp(commands[0], "USER") == 0) {
        //Set the current user to the given user
        curr_user = exists_user(commands[1]);
        if(curr_user == NULL) {
            sprintf(out_msg, "User %s does not exist\n", commands[1]);
        }
        else {
            sprintf(out_msg, "User %s exists. Totally %d messages in spool file\n", commands[1], curr_user->num_messages);
            read_fp = NULL;
            msg_count = 0;
        }
    }
    else if(strcmp(commands[0], "READM") == 0) {
        //Read the mail pointed to by the read pointer
        if(curr_user == NULL) {
            strcpy(out_msg, "No user selected. Please select the user using \'SetUser\' command\n");
        }
        else {
            readRet* result;
            result = read_mail(curr_user, read_fp);
            read_fp = result->fp;
            if(++msg_count == curr_user->num_messages) {
                fseek(read_fp, 0, SEEK_SET);
                msg_count = 0;
            }

            strcpy(out_msg, result->msg);
        }
    }
    else if(strcmp(commands[0], "DELM") == 0) {
        //Delete the mail pointed to by the read pointer
        if(curr_user == NULL) {
            strcpy(out_msg, "No user selected. Please select the user using \'SetUser\' command\n");
        }
        else if(curr_user->num_messages == 0) {
            //No messages to delete
            strcpy(out_msg, "No more mail\n");
        }
        else {
            read_fp = delete_mail(curr_user, msg_count);
            if(msg_count >= curr_user->num_messages) {
                msg_count = 0;
            }
            strcpy(out_msg, "Message deleted\n");
        }
    }
    else if(strcmp(commands[0], "SEND") == 0) {
        //Send a new mail from current user to the given user
        if(curr_user == NULL) {
            strcpy(out_msg, "No user selected. Please select the user using \'SetUser\' command\n");
        }
        else {
            userNode* receiver = NULL;
            receiver = exists_user(commands[1]);
            if(receiver == NULL) {
                sprintf(out_msg, "Receiver %s does not exist\n", commands[1]);
            }
            else {
                char msg[MAX_IN];
                int offset = strlen(commands[1]) + 6;
                int msg_len = strlen(inp) - offset;
                for(int i = 0; i < msg_len; i++) {
                    msg[i] = inp[offset + i];
                }
                msg[msg_len] = '\0';

                send_mail(curr_user->id, receiver, msg);

                strcpy(out_msg, "Message sent\n");
            }
        }
    }
    else if(strcmp(commands[0], "DONEU") == 0) {
        //Reset the current user
        curr_user = NULL;
        if(read_fp !=NULL) {
            fclose(read_fp);
        }
        read_fp = NULL;
        msg_count = 0;
        strcpy(out_msg, "User reset\n");
    }
    else if(strcmp(commands[0], "QUIT") == 0) {
        //Stop the server
        strcpy(out_msg, "Connection closed\n");
    }

    return out_msg;
}
