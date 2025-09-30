#include "network.h"
#include "rtype.h"

void rtype::server::Rtype::loop() {
    network::loop_recv(udp_server_fd);
    network::loop_send(udp_server_fd);
    packetHandler.processPackets(packetManager.fetchReceivedPackets());
}


int main() {
    rtype::server::Rtype &r = root;
    r.udp_server_fd = rtype::server::network::setupUDPServer(8080);

    while (1) {
        r.loop();
    }
    return 0;
}
