#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

std::mutex mtx;

void scanPort(const std::string& ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return;

    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    // Try to connect
    int result = connect(sockfd, (struct sockaddr*)&target, sizeof(target));
    close(sockfd);

    if (result == 0) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Port " << port << " is OPEN\n";
    }
}

int main() {
    std::string ip;
    int startPort = 1, endPort = 1024;

    std::cout << "Enter target IP address: ";
    std::cin >> ip;

    std::vector<std::thread> threads;

    for (int port = startPort; port <= endPort; ++port) {
        threads.emplace_back(scanPort, ip, port);

        // Limit thread count to avoid overload
        if (threads.size() >= 100) {
            for (auto& th : threads) th.join();
            threads.clear();
        }
    }

    // Join remaining threads
    for (auto& th : threads) th.join();

    std::cout << "Scan completed.\n";
    return 0;
}
