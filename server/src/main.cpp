#include "network.h"
#include "packets.h"
#include "rtype.h"
#include "controllers/RoomController.h"

void rtype::server::Rtype::loop() {
    network::loop_recv(udp_server_fd);
    network::loop_send(udp_server_fd);
    packetHandler.processPackets(packetManager.fetchReceivedPackets());
}


int main() {
    rtype::server::Rtype &r = root;
    r.udp_server_fd = rtype::server::network::setupUDPServer(8080);

    root.packetHandler.registerCallback(Packets::JOIN_ROOM, rtype::server::controllers::room_controller::handleJoinRoomPacket);

    while (true) {
        r.loop();
    }
    return 0;
}
