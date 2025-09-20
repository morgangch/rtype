/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include <iostream>
#include <memory>
#include "packetmanager.h"


#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RESET "\033[0m"

void display_log(const std::string &title, const std::string &message, const std::string &color) {
    std::cout << "[" << color << title << COLOR_RESET << "] " << message << std::endl;
}

int main() {
    PacketManager manager;
    display_log("INFO", "Starting PacketManager tests...", COLOR_GREEN);

}
