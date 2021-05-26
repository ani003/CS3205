/*
Copyright (c) 2021 Anirudh S <anirudh6626@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
