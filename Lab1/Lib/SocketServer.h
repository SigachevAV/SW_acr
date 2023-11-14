#pragma once
#include "Socket.h"
#include <string>
#include <vector>
#include <memory>

// server-side socket. Accepts connections from client-side sockets.
class SocketServer : public Socket
{
public:
    SocketServer() = default;
    virtual ~SocketServer() = default;
    bool listen(int port); // attaches socket to certain port of a computer. 0 means random port.
    int port();
    void setPort(int port); // specifies fixed port to be used for attaching the socket
    std::shared_ptr<Socket> accept(); // checks for incoming connection and accepts it if it's present

private:
    int m_port = 0;
};
