/**
 * Name: Anirudh S
 * Roll no: CS17B003
 * Source: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*/

#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "udp.h"
#define MAX_BUF 1024

UDP_server::UDP_server(int port_num) {
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
            
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
            
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port_num);
            
    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

UDP_server::~UDP_server() {
    close(sockfd);
}

std::string UDP_server::recieve() {
    int n;
    socklen_t len;
    char buffer[MAX_BUF];
  
    len = sizeof(cliaddr);  //len is value/resuslt
        
    n = recvfrom(sockfd, (char *)buffer, MAX_BUF, MSG_WAITALL,
                    (struct sockaddr *) &cliaddr, &len);
    buffer[n] = '\0';

    std::string msg = buffer;
    return msg;
}

void UDP_server::send(std::string msg) {
    sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM,
            (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
}


UDP_client::UDP_client() {
    // Creating socket file descriptor
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
        
    memset(&servaddr, 0, sizeof(servaddr));
}

UDP_client::~UDP_client() {
    close(sockfd);
}

std::string UDP_client::receive() {
    int n;
    socklen_t len;
    char buffer[MAX_BUF];
    n = recvfrom(sockfd, (char *)buffer, MAX_BUF, MSG_WAITALL,
                    (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';

    std::string msg = buffer;
    return msg;
}

void UDP_client::send(int port_num, std::string msg) {
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port_num);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM,
            (const struct sockaddr *) &servaddr, sizeof(servaddr));
}

