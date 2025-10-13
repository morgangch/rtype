/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef BYTES_PRINTER_H
#define BYTES_PRINTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_bytes(void *data, size_t size)
{
    printf("BYTE ID:    ");
    for (size_t i = 0; i < size; i++) {
        unsigned char byte = *((unsigned char *)data + i);
        printf("%zu ", i);
        if (byte > 99) printf(" ");
        if (i < 10) printf(" ");
    }
    printf("\nHEX:        ");
    for (size_t i = 0; i < size; i++) {
        unsigned char byte = *((unsigned char *)data + i);
        printf("%02x ", byte);
        if (byte > 99) printf(" ");
    }
    printf("\nDECIMAL:    ");
    for (size_t i = 0; i < size; i++) {
        unsigned char byte = *((unsigned char *)data + i);
        printf("%02d ", byte);
    }
    printf("\nASCII:      ");
    for (size_t i = 0; i < size; i++) {
        unsigned char byte = *((unsigned char *)data + i);
        if (byte == '\0')
            printf("\\0 ");
        else if (byte == '\n')
            printf("\\n ");
        else {
            if (byte < 32 || byte > 126)
                printf("�  ");
            else
                printf("%c  ", byte);
        }
        if (byte > 99) printf(" ");
    }
    printf("\n");
}

#endif //BYTES_PRINTER_H
