/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <vector>
#include "packet.h"

#define PACKET_HISTORY_SIZE 512

class PacketManager {
public:
    PacketManager();

    void clean();

    /*
     * Handle incoming raw packet bytes from the socket (including header in data)
     * Parse the bytes into a packet_t struct and call handlePacket
     * If the packet is invalid, ignore it
     * @param data Pointer to the raw packet bytes
     * @param size Size of the raw packet bytes
     */
    void handlePacketBytes(const uint8_t *data, size_t size);

    /*
     * Prepare a packet to be sent by building the header
     * @param data Pointer to the data to be sent (will be copied)
     * @param size Size of the data to be sent
     */
    void sendPacketBytes(void **data, size_t *size, uint8_t packet_type);

    void ackMissing();

    std::vector<std::unique_ptr<packet_t> > fetchReceivedPackets();

private:
    uint32_t _send_seqid = 0;
    uint32_t _recv_seqid = 0;
    uint32_t _auth_key = 0;

    std::vector<packet_t> _history_sent;
    std::vector<uint32_t> _missed_packets;

    std::vector<std::unique_ptr<packet_t> > _buffer_received;
    std::vector<packet_t> _buffer_send;

    bool _resendPacket(uint32_t seqid);

    void _handlePacket(std::unique_ptr<packet_t> packet);
};

#endif //PACKETMANAGER_H
