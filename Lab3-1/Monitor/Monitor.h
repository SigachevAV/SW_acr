#pragma once

#include "Console.h"

#include "ServerPinger.h"
#include "ServerType.h"
#include "WarmSpareState.h"

class OSProcess;

struct ProcessesState
{
    bool primaryRunning;
    bool secondaryRunning;
};

// Monitor app that manages Server's lifecycle and monitors its state
class Monitor
{
public:
    Monitor(uint32_t primaryServerPort);

    ~Monitor();

    bool init(); // launches Server

    void startMonitoring();

    bool runPrimaryServer(const std::string& cmd);

    bool runSecondaryServer(const std::string& cmd);

private:
    bool runProcess(uint32_t port, OSProcess& p);

    bool send(const std::string& url, const std::string& message);

    // returns true if processes are running
    ProcessesState checkRunningAndTryRestart();

    static uint32_t generateSecondaryServerPort(uint32_t primaryServerPort);

    static std::string createServerRunCommand(uint32_t serverPort);

    static bool processRunning(OSProcess& p); // checks Server state

    static void reset(); // terminates irresponsive Server

private:
    static constexpr const char* serverFilename = "AppServer.exe";
    static constexpr uint64_t pingPeriodMs = 1000;

    uint32_t m_primaryServerPort;
    uint32_t m_secondaryServerPort;

    std::string m_primaryUrl;
    std::string m_secondaryUrl;

    Console m_console;

    ServerPinger m_pinger;

    Client m_serverClient;

    WarmSpareState m_state;
};
