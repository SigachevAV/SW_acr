#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"
#include "SocketClient.h"
#include "UtilString.h"
#include "UtilFile.h"
#include <map>

Client::~Client()
{
    m_socket.close();
}

bool Client::init(const std::string& url, uint32_t sendTimeout, uint32_t recvTimeout)
{
    return m_socket.init(sendTimeout, recvTimeout) && m_socket.connect(url);
}

bool Client::send(const std::string& msg)
{
    printf("sending text message \"%s\"\n", msg.c_str());
    int len = m_socket.sendStr(msg);
    if (len == SOCKET_ERROR)
        printf("nothing was sent, wsa error: %d\n", WSAGetLastError());

    printf("sent %d bytes\n", len);
    return len > 0;
}

bool Client::recv(std::string& msg)
{
    int recvSize = m_socket.recv();
    if (recvSize == 0)
        return false;

    char* recvData = m_socket.data();
    msg.assign(recvData, recvSize);

    return true;
}

void Client::shutdownSend()
{
    m_socket.shutdownSend();
}
