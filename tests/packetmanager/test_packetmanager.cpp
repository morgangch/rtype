/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include <cstring>
#include <cstdlib>
#include "packetmanager.h"

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

// Test Framework Classes
class TestResult {
public:
    int passed = 0;
    int failed = 0;
    int crashed = 0;

    int total() const { return passed + failed + crashed; }
    bool hasFailures() const { return failed > 0 || crashed > 0; }

    void printSummary() const {
        std::cout << "\n" << COLOR_BLUE << "=== Test Summary ===" << COLOR_RESET << std::endl;
        std::cout << COLOR_GREEN << "Passed: " << passed << COLOR_RESET << std::endl;
        std::cout << COLOR_RED << "Failed: " << failed << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Crashed: " << crashed << COLOR_RESET << std::endl;
        std::cout << COLOR_BLUE << "Total: " << total() << COLOR_RESET << std::endl;

        if (!hasFailures()) {
            std::cout << COLOR_GREEN << "All tests passed! ✓" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Some tests failed! ✗" << COLOR_RESET << std::endl;
        }
    }
};

class TestRunner {
private:
    TestResult result;

    void log(const std::string& level, const std::string& message, const std::string& color) {
        std::cout << "[" << color << level << COLOR_RESET << "] " << message << std::endl;
    }

public:
    template<typename T>
    void assertEqual(const std::string& testName, const T& expected, const T& actual, const std::string& description = "") {
        if (expected == actual) {
            result.passed++;
            log("PASS", testName, COLOR_GREEN);
        } else {
            result.failed++;
            log("FAIL", testName, COLOR_RED);
            std::cout << COLOR_RED << "-> " << description << " Expected: " << expected
                      << ", Actual: " << actual << COLOR_RESET << std::endl;
        }
    }

    void assertStringEqual(const std::string& testName, const char* expected, const char* actual, const std::string& description = "") {
        if (strcmp(expected, actual) == 0) {
            result.passed++;
            log("PASS", testName, COLOR_GREEN);
        } else {
            result.failed++;
            log("FAIL", testName, COLOR_RED);
            std::cout << COLOR_RED << "-> " << description << " Expected: '" << expected
                      << "', Actual: '" << actual << "'" << COLOR_RESET << std::endl;
        }
    }

    void assertTrue(const std::string& testName, bool condition, const std::string& description = "") {
        if (condition) {
            result.passed++;
            log("PASS", testName, COLOR_GREEN);
        } else {
            result.failed++;
            log("FAIL", testName, COLOR_RED);
            std::cout << COLOR_RED << "-> " << description << COLOR_RESET << std::endl;
        }
    }

    void runTest(const std::string& testName, std::function<void()> testFunction) {
        try {
            testFunction();
        } catch (const std::exception& e) {
            result.crashed++;
            log("CRASH", testName + " - Exception: " + e.what(), COLOR_RED);
        } catch (...) {
            result.crashed++;
            log("CRASH", testName + " - Unknown exception", COLOR_RED);
        }
    }

    TestResult getResult() const { return result; }

    void printProgress() const {
        std::cout << COLOR_BLUE << "Progress: " << result.total() << " tests completed" << COLOR_RESET << std::endl;
    }
};

// Test Data Structure
typedef struct super_packet_s {
    int my_age;
    bool im_gay;
    char my_name[50];
} super_packet_t;

// Test Helper Functions
class PacketTestHelper {
public:
    static super_packet_t createTestPacket(int age = 25, bool gay = true, const char* name = "John Doe") {
        super_packet_t packet;
        packet.my_age = age;
        packet.im_gay = gay;
        strncpy(packet.my_name, name, sizeof(packet.my_name) - 1);
        packet.my_name[sizeof(packet.my_name) - 1] = '\0';
        return packet;
    }

    static void* createPacketData(const super_packet_t& packet) {
        void* data = malloc(sizeof(super_packet_t));
        memcpy(data, &packet, sizeof(super_packet_t));
        return data;
    }

    static void transferPacket(PacketManager& sender, PacketManager& receiver) {
        std::vector<std::unique_ptr<packet_t>> to_send = sender.fetchPacketsToSend();
        if (!to_send.empty()) {
            packet_t* packet = to_send[0].get();
            std::vector<uint8_t> raw_packet = PacketManager::serializePacket(*packet);
            receiver.handlePacketBytes(raw_packet.data(), raw_packet.size());
        }
    }
};

// Test Functions
void testPacketSending(TestRunner& runner) {
    PacketManager manager;
    super_packet_t testPacket = PacketTestHelper::createTestPacket();
    void* data = PacketTestHelper::createPacketData(testPacket);
    size_t data_size = sizeof(super_packet_t);

    manager.sendPacketBytes(&data, &data_size, 1);

    runner.assertEqual("Send buffer size check", 1UL, manager._get_buffer_send()->size(), "Buffer should contain 1 packet");
    runner.assertEqual("Send sequence ID check", 1U, manager._get_send_seqid(), "Sequence ID should be 1");

    free(data);
}

void testPacketTransfer(TestRunner& runner) {
    PacketManager sender, receiver;
    super_packet_t testPacket = PacketTestHelper::createTestPacket();
    void* data = PacketTestHelper::createPacketData(testPacket);
    size_t data_size = sizeof(super_packet_t);

    sender.sendPacketBytes(&data, &data_size, 1);

    runner.runTest("Packet transfer", [&]() {
        PacketTestHelper::transferPacket(sender, receiver);
    });

    free(data);
}

void testPacketReception(TestRunner& runner) {
    PacketManager sender, receiver;
    super_packet_t testPacket = PacketTestHelper::createTestPacket();
    void* data = PacketTestHelper::createPacketData(testPacket);
    size_t data_size = sizeof(super_packet_t);

    // Send and transfer packet
    sender.sendPacketBytes(&data, &data_size, 1);
    PacketTestHelper::transferPacket(sender, receiver);

    // Test reception
    runner.assertEqual("Receive sequence ID", 1U, receiver._get_recv_seqid(), "Received sequence ID should be 1");

    std::vector<std::unique_ptr<packet_t>> mailbox = receiver.fetchReceivedPackets();
    runner.assertEqual("Received packets count", 1UL, mailbox.size(), "Should receive exactly 1 packet");
    runner.assertEqual("Buffer cleared after fetch", 0UL, receiver._get_buffer_received()->size(), "Buffer should be empty after fetch");

    free(data);
}

void testPacketData(TestRunner& runner) {
    PacketManager sender, receiver;
    super_packet_t testPacket = PacketTestHelper::createTestPacket(25, true, "John Doe");
    void* data = PacketTestHelper::createPacketData(testPacket);
    size_t data_size = sizeof(super_packet_t);

    // Send, transfer, and receive packet
    sender.sendPacketBytes(&data, &data_size, 1);
    PacketTestHelper::transferPacket(sender, receiver);
    std::vector<std::unique_ptr<packet_t>> mailbox = receiver.fetchReceivedPackets();

    if (!mailbox.empty()) {
        packet_t* packet = mailbox[0].get();

        // Test header
        runner.assertEqual("Packet type", (uint8_t)1, packet->header.type, "Packet type should be 1");
        runner.assertEqual("Packet sequence ID", 1U, packet->header.seqid, "Packet sequence ID should be 1");
        runner.assertEqual("Packet ACK", 0U, packet->header.ack, "Packet ACK should be 0");
        runner.assertEqual("Packet data size", (uint32_t)sizeof(super_packet_t), packet->header.data_size, "Data size should match");

        // Test data
        super_packet_t* received_data = (super_packet_t*)packet->data;
        runner.assertEqual("Data age", 25, received_data->my_age, "Age should be 25");
        runner.assertEqual("Data boolean flag", true, received_data->im_gay, "Boolean flag should be true");
        runner.assertStringEqual("Data name", "John Doe", received_data->my_name, "Name should be 'John Doe'");
    } else {
        runner.assertTrue("Packet data validation", false, "No packet received for data validation");
    }

    free(data);
}

int main() {
    TestRunner runner;

    std::cout << COLOR_BLUE << "=== Starting PacketManager Tests ===" << COLOR_RESET << std::endl;

    // Run all tests
    testPacketSending(runner);
    testPacketTransfer(runner);
    testPacketReception(runner);
    testPacketData(runner);

    // Print results
    TestResult result = runner.getResult();
    result.printSummary();

    return result.hasFailures() ? 1 : 0;
}
