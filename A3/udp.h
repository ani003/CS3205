#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define MAX_BUF 1024

#pragma once

class UDP_server {
    private:
        int sockfd;
        struct sockaddr_in servaddr, cliaddr;

    public:
        UDP_server(int port_num);
        ~UDP_server();

        std::string recieve();

        void send(std::string msg);
};

class UDP_client {
    private:
        int sockfd;
        struct sockaddr_in servaddr;
        
    public:
        UDP_client();
        ~UDP_client();

        std::string receive();

        void send(int port_num, std::string msg);
};