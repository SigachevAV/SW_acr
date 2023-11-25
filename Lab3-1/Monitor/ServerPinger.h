#pragma once

#include "Client.h"

#include <atomic>
#include <thread>

class ServerPinger
{
public:
	ServerPinger();

	bool ping(const std::string& serverUrl);

private:
	static constexpr uint32_t waitBeforeRecvMs = 100;
	static constexpr uint32_t sendTimeoutMs = 1000;
	static constexpr uint32_t recvTimeoutMs = 1000;
};
