#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>

#include "SocketServer.h"

// Server that accepts connections from Clients and Viewers.
// Clients can upload text and images to Server.
class Server
{
public:
    Server() = default;

    virtual ~Server() = default;

    bool init(int port = 0); // initialize server socket, load configuration

    void run(); // accept incoming connections, receive their data and send responses

private:
    bool init_socket(int port = 0);

private:
    SocketServer m_socket; // server socket
    std::vector<std::string> m_data; // representation of uploaded data
};
