/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "rtype.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int setupUDPServer(int port) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    std::cout << "[INFO] UDP server listening on port " << port << " and fd " << sockfd << std::endl;
    return sockfd;
}