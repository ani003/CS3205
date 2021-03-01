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
#define MAX_IN 1000
#define MAX_MSG (MAX_IN + 100)
#define PORT 8080
#define SA struct sockaddr

int user_flag = 0;
char curr_user_id[100];

void socket_func(int sockfd);
char* prepare_message();

int main(int argc, char* argv[]) {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    */

    bzero(&servaddr, sizeof(servaddr));

    //Assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    /*
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
    */

   connect(sockfd, (SA*)&servaddr, sizeof(servaddr));

   //char init_msg[MAX_IN];
   //read(sockfd, init_msg, MAX_IN);

    //Interact with the server
    socket_func(sockfd);

    //Close the socket
    close(sockfd);

    return 0;
}

void socket_func(int sockfd) {
    char buff[MAX_IN];
    int quit_flag = 0;

    while(!quit_flag) {
        bzero(buff, MAX_IN);
        strcpy(buff, prepare_message());
        //printf("Sending: %s\n",buff);
        write(sockfd, buff, sizeof(buff));

        //If input was QUIT, then quit
        if (strstr(buff, "QUIT")) {
            quit_flag = 1;
        }

        bzero(buff, MAX_IN);
        read(sockfd, buff, sizeof(buff));
        printf("Server: %s", buff);
    }
}

char* prepare_message() {
    char user_in[MAX_IN];
    char* token;
    char* commands[2];

    char* msg = (char*)malloc(MAX_IN);
    strcpy(msg, "");

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
            strcpy(msg, "LSTU");
            break;
        }
        else if(strcmp(commands[0], "Adduser") == 0) {
            //Add new user
            sprintf(msg, "ADDU %s", commands[1]);
            break;
        }
        else if(strcmp(commands[0], "SetUser") == 0) {
            //Set the current user to the given user
            sprintf(msg, "USER %s", commands[1]);
            strcpy(curr_user_id, commands[1]);
            user_flag = 1;
            break;
        }
        else if(strcmp(commands[0], "Read") == 0) {
            //Read the mail pointed to by the read pointer
            strcpy(msg, "READM");
            break;
        }
        else if(strcmp(commands[0], "Delete") == 0) {
            //Delete the mail pointed to by the read pointer
            strcpy(msg, "DELM");
            break;
        }
        else if(strcmp(commands[0], "Send") == 0) {
            //Send a new mail from current user to the given user
            char mail_body[MAX_MSG];
            strcpy(mail_body, "");
            printf("Type message:\n");
            char str[MAX_IN];
            strcpy(str, "");

            //Read input from stdin and write it, until "###" is encountered
            while(!strstr(str, "###")) {
                fgets(str, MAX_IN, stdin);
                strcat(mail_body, str);
            }
            mail_body[strlen(mail_body) - 1] = '\0';
            sprintf(msg, "SEND %s %s", commands[1], mail_body);
            break;
        }
        else if(strcmp(commands[0], "Done") == 0) {
            //Reset the current user
            strcpy(msg, "DONEU");
            user_flag = 0;
            strcpy(curr_user_id, "");
            break;
        }
        else if(strcmp(commands[0], "Quit") == 0) {
            //Stop the client and server
            strcpy(msg, "QUIT");
            break;
        }
        else {
            printf("Invalid command\n");
        }
    }

    //printf("msg: %s\n", msg);
    return msg;
}