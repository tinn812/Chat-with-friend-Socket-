#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <winsock2.h>     // WinSock 的主要功能
#include <ws2tcpip.h>   // WinSock 的 TCP/IP 功能

void receive_messages(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "\n[Client] Disconnected from server." << std::endl;
            break;
        }
        std::cout << buffer << std::flush;
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }
    // 建立 socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection to server failed!" << std::endl;
        return 1;
    }

    // 輸入名稱並發送
    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.length(), 0);

    std::thread receiver(receive_messages, sock);

    // 傳送訊息 loop
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if (msg == "/quit") break;
        send(sock, msg.c_str(), msg.length(), 0);
    }

    closesocket(sock);
    WSACleanup();
    receiver.join();
    return 0;
}
//  編譯：g++ chat_client.cpp -o client.exe -lws2_32 -std=c++17 