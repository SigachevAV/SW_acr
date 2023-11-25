#pragma once
#include <string>
#include <vector>

class SocketServer;

// base class for a socket
class Socket
{
public:
    Socket();
    virtual ~Socket();
    bool init(uint32_t sendTimeout = 0, uint32_t recvTimeout = 0); // creates socket and performs basic initialization
    bool isValid(); // checks if socket is valid
    int send(const char* msg, int len); // sends a message via socket
    int sendStr(const std::string& str); // sends a string via socket
    int recv(); // receives a message via socket
    char* data(); // buffer that holds received message
    void close(); // closes socket
    void shutdownSend();

public:
    int m_socket = -1; // low-level socket object

    uint32_t m_sendTimeout = 0;
    uint32_t m_recvTimeout = 0; // if input/output operation is not complete during this period of time, this operation aborts
    std::vector<char> m_recv; // buffer that holds received message

    friend class SocketServer;
};
