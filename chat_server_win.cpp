#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <ctime>

std::vector<int> client_sockets;
std::map<int, std::string> client_names;
std::mutex clients_mutex;

void broadcast(const std::string& msg, int sender_sock) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int sock : client_sockets) {
        if (sock != sender_sock) {
            send(sock, msg.c_str(), msg.length(), 0);
        }
    }
}

std::string current_time() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

void handle_client(int client_sock) {
    char buffer[1024];

    // 第一步：接收名稱
    memset(buffer, 0, sizeof(buffer));
    int name_len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (name_len <= 0) {
        closesocket(client_sock);
        return;
    }
    std::string client_name = std::string(buffer);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_names[client_sock] = client_name;
    }

    std::string join_msg = "[" + client_name + "] joined the chat.\n";
    std::cout << "[" << current_time() << "] " << join_msg << std::flush;
    broadcast(join_msg, client_sock);

    // 聊天訊息 loop
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break;
        }

        std::string msg = "[" + client_name + "] " + buffer + "\n";
        std::cout << "[" << current_time() << "] " << msg << std::flush;

        broadcast(msg, client_sock);
    }

    // 離線處理
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), client_sock), client_sockets.end());
        client_names.erase(client_sock);
    }

    std::string leave_msg = "[" + client_name + "] left the chat.\n";
    std::cout << "[" << current_time() << "] " << leave_msg << std::flush;
    broadcast(leave_msg, client_sock);

    closesocket(client_sock);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);

    std::cout << "[Server] Chat server started on port 12345." << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        int client_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_size);

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            client_sockets.push_back(client_sock);
        }

        std::cout << "[Server] New client connected: socket " << client_sock << std::endl;

        std::thread(handle_client, client_sock).detach();
    }

    closesocket(server_sock);
    WSACleanup();
    return 0;
}
// 編譯：g++ server.cpp -o server.exe -lws2_32 -std=c++17
// 執行：server.exe
