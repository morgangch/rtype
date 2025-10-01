/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PACKET_H
#define PACKET_H
#include <cstdint>

typedef struct packet_header_s {
    uint32_t seqid;
    uint32_t ack;
    uint8_t type;
    uint32_t auth;
    uint8_t client_addr[4];
    uint16_t client_port;
    uint32_t data_size;
} packet_header_t;

typedef struct packet_s {
    packet_header_t header;
    void *data;
} packet_t;

#endif //PACKET_H
