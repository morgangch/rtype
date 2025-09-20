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
    int total;
} test_result_t;

typedef struct super_packet_s {
    int my_age;
    bool im_gay;
    char my_name[50];
} super_packet_t;


int main() {
    PacketManager manager;
    test_result_t test_result = {0, 0, 0};
    std::string test_name;
    display_log("INFO", "Starting PacketManager tests...", COLOR_GREEN);


    super_packet_t p = {25, true, "John Doe"};
    void *data = malloc(sizeof(super_packet_t));
    memcpy(data, &p, sizeof(super_packet_t));
    size_t data_size = sizeof(super_packet_t);
    manager.sendPacketBytes(&data, &data_size, 1);

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

    // Transfer the packet to the receiver
    PacketManager receiver;

    test_name = "Packet transfer to the receiver";
    try {
        std::vector<std::unique_ptr<packet_t> > to_send = manager.fetchPacketsToSend();
        packet_t *packet = to_send[0].get();
        size_t raw_size = sizeof(packet_header_t) + packet->header.data_size;
        void *raw_data = malloc(raw_size);
        std::vector<uint8_t> raw_packet = PacketManager::serializePacket(*packet);
        memcpy(raw_data, raw_packet.data(), raw_size);
        receiver.handlePacketBytes((uint8_t *) raw_data, raw_size);
        free(raw_data);
        test_result.passed++;
        display_log("PASS", test_name, COLOR_GREEN);
    } catch (...) {
        test_result.crashed++;
        display_log("CRASH", test_name, COLOR_RED);
    }

    // Check the first packet data reception
    test_name = "Peer first packet data reception";
    try {

        if (receiver._get_recv_seqid() != 1) {
            test_result.failed++;
            display_log("FAIL", test_name, COLOR_RED);
            std::cout << COLOR_RED << "-> Expected recv_seqid: 1, Actual recv_seqid: " << receiver._get_recv_seqid()
                    << COLOR_RESET << std::endl;
        }

        std::vector<std::unique_ptr<packet_t> > receiver_mailbox = receiver.fetchReceivedPackets();
        if (receiver_mailbox.size() != 1 || receiver._get_buffer_received()->size() != 0) {
            test_result.failed++;
            display_log("FAIL", test_name, COLOR_RED);
            std::cout << COLOR_RED << "-> Expected received packets size: 1, Actual received packets size: " <<
                    receiver_mailbox.size() << COLOR_RESET << std::endl;
            std::cout << COLOR_RED << "-> Expected buffer received size: 0, Actual buffer received size: " <<
                    receiver._get_buffer_received()->size() << COLOR_RESET << std::endl;
        }


        // Check the packet header
        packet_t *packet = receiver_mailbox[0].get();
        if (packet->header.type != 1 || packet->header.seqid != 1 || packet->header.ack != 0) {
            test_result.failed++;
            display_log("FAIL", test_name, COLOR_RED);
            std::cout << COLOR_RED << "-> Bad header. Expected packet type: 1, Actual packet type: " << (int) packet->
                    header.type <<
                    COLOR_RESET << std::endl;
            std::cout << COLOR_RED << "-> Bad header. Expected packet seqid: 1, Actual packet seqid: " << packet->header
                    .seqid <<
                    COLOR_RESET << std::endl;
            std::cout << COLOR_RED << "-> Bad header.  Expected packet ack: 0, Actual packet ack: " << packet->header.
                    ack <<
                    COLOR_RESET << std::endl;
        }

        // Check the packet data
        super_packet_t *received_data = (super_packet_t *) packet->data;
        if (received_data->my_age != 25 || received_data->im_gay != true || strcmp(received_data->my_name, "John Doe")
            != 0) {
            test_result.failed++;
            display_log("FAIL", test_name, COLOR_RED);
            std::cout << COLOR_RED << "-> Bad data. Expected age: 25, Actual age: " << received_data->my_age <<
                    COLOR_RESET << std::endl;
            std::cout << COLOR_RED << "-> Bad data. Expected im_gay: true, Actual height: " << (received_data->im_gay ? "true" : "false")
                    <<
                    COLOR_RESET << std::endl;
            std::cout << COLOR_RED << "-> Bad data. Expected name: John Doe, Actual name: " << received_data->my_name <<
                    COLOR_RESET << std::endl;
        }
    } catch (...) {
        test_result.crashed++;
        display_log("CRASH", test_name, COLOR_RED);
    }

    return (test_result.failed + test_result.crashed) == 0 ? 0 : 1;
}
