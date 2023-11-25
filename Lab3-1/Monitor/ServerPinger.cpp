#include "ServerPinger.h"

#include <iostream>
#include <stdexcept>

#include <winsock.h>

using namespace std::chrono;

ServerPinger::ServerPinger()
{
}

bool ServerPinger::ping(const std::string& serverUrl)
{
	Client client;
	if(!client.init(serverUrl, sendTimeoutMs, recvTimeoutMs))
		return false;
	if (!client.send("ping"))
	{
		std::cout << "cannot send ping request to the server\n";
		return false;
	}
	client.shutdownSend();

	std::this_thread::sleep_for(milliseconds(waitBeforeRecvMs));

	std::string response;
	if (!client.recv(response))
	{
		printf("server doesn't respond on ping request, wsa error: %d\n", WSAGetLastError());
		return false;
	}

	if (response != "pong")
	{
		std::cout << "ServerPinger expected pong, but got invalid message\n";
		return false;
	}

	return true;
}
