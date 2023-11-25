#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <WS2tcpip.h>

#include "SocketClient.h"
#include "../../helpers/UtilString.h"

bool SocketClient::connect(const std::string& url)
{
    struct sockaddr_in server;
    const std::vector<std::string>& urlTokens = split(url, ":");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(urlTokens[0].c_str());
    server.sin_port = htons(toInt(urlTokens[1]));
    if(::connect(m_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connection error to %s\n", url.c_str());
        close();
        m_socket = -1;
        return false;
    }
    return true;
}
