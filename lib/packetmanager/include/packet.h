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
    uint32_t data_size;  // Size of the data payload
} packet_header_t;

typedef struct packet_s {
    packet_header_t header;
    void *data;
} packet_t;

#endif //PACKET_H
