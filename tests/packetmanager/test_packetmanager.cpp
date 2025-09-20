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

typedef struct test_result_s {
    int passed;
    int failed;
    int crashed;
} test_result_t;

typedef struct super_packet_s {
    int my_age;
    float my_height;
    char my_name[50];
} super_packet_t;


int main() {
    PacketManager manager;
    test_result_t test_result = {0, 0, 0};
    std::string test_name;
    display_log("INFO", "Starting PacketManager tests...", COLOR_GREEN);


    super_packet_t p = {25, 5.9, "John Doe"};
    void *data = malloc(sizeof(super_packet_t));
    memcpy(data, &p, sizeof(super_packet_t));
    size_t size = sizeof(super_packet_t);
    manager.sendPacketBytes(&data, &size, 1);

    display_log("INFO", "Starting PacketManager tests...", COLOR_GREEN);


    // First packet will be sent to the peer.
    test_name = "First packet sent to the queue";
    try {
        if (manager._get_buffer_send()->size() == 1 && manager._get_send_seqid() == 1) {
            test_result.passed++;
            display_log("PASS", test_name, COLOR_GREEN);
        } else {
            test_result.failed++;
            display_log("FAIL", test_name, COLOR_RED);
            // Display expected vs actual in red
            std::cout << COLOR_RED << "-> Expected buffer size: 1, Actual buffer size: " << manager._get_buffer_send()->
                    size() << COLOR_RESET << std::endl;
            std::cout << COLOR_RED << "-> Expected send_seqid: 1, Actual send_seqid: " << manager._get_send_seqid() <<
                    COLOR_RESET << std::endl;
        }
    } catch (...) {
        test_result.crashed++;
        display_log("CRASH", test_name, COLOR_RED);
    }


    // Display summary
    std::cout << "\nTest Summary: (" << test_result.passed + test_result.failed + test_result.crashed << " tests)" <<
            std::endl;
    if (test_result.passed > 0)
        std::cout << COLOR_GREEN << "Passed: " << test_result.passed << COLOR_RESET << std::endl;
    if (test_result.failed > 0)
        std::cout << COLOR_RED << "Failed: " << test_result.failed << COLOR_RESET << std::endl;
    if (test_result.crashed > 0)
        std::cout << COLOR_RED << "Crashed: " << test_result.crashed << COLOR_RESET << std::endl;
    return (test_result.failed + test_result.crashed) == 0 ? 0 : 1;
}
