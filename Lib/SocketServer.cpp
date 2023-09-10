#include <winsock2.h>
#include <WS2tcpip.h>

#include "SocketServer.h"
#include "UtilString.h"

bool SocketServer::listen(int port)
{
    if(!m_port)
        m_port = port;

    struct sockaddr_in local, bound;
    memset(&bound, 0, sizeof(bound));
    local.sin_family = AF_INET;
    local.sin_port = htons(m_port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(m_socket, (struct sockaddr *)&local, sizeof(local)) == -1)
    {
        printf("bind error, port %d\n", m_port);
        close();
        return false;
    }
    if (::listen(m_socket, 5) == -1)
    {
        printf("listen error\n");
        close();
        return false;
    }
    socklen_t size = sizeof(bound);
    getsockname(m_socket, (struct sockaddr *)&bound, &size);
    m_port = ntohs(bound.sin_port);
    return true;
}

int SocketServer::port()
{
    return m_port;
}

void SocketServer::setPort(int port)
{
    m_port = port;
}

std::shared_ptr<Socket> SocketServer::accept()
{
    std::shared_ptr<Socket> result = std::make_shared<Socket>();
    struct sockaddr_in remote;
    socklen_t t = sizeof(remote);
 
    fd_set fs;
    struct timeval tv;
    FD_ZERO(&fs);
    FD_SET(m_socket, &fs);
    tv.tv_sec = 0;
    tv.tv_usec = m_timeout*1000;
    if(select(m_socket + 1, &fs, NULL, NULL, &tv) <= 0)
        return result;
    // in a very rare case when client connection drops here, watchdog may mistakenly kill the process
    result->m_socket = ::accept(m_socket, (struct sockaddr *)&remote, &t);
    return result;
}
