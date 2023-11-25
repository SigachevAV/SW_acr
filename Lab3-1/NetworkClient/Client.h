#pragma once

#include "SocketClient.h"

#include <string>

// command-line Client app that can send messages to Server.
class Client
{
public:
    ~Client();

    bool init(const std::string& url, uint32_t sendTimeout = 0, uint32_t recvTimeout = 0);

    bool send(const std::string& msg); // sends message to Server

    bool recv(std::string& msg); // sends message to Server

    void shutdownSend();

private:
    SocketClient m_socket;

    uint32_t m_sendTimeout;
    uint32_t m_recvTimeout;
};
