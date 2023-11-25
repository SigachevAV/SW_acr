#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include <string>
#include "Monitor.h"
#include "OSProcess.h"
#include "UtilString.h"
#include "UtilFile.h"

#include <stdexcept>

using namespace std::string_literals;
using namespace std::chrono;

static OSProcess s_primaryServer;
static OSProcess s_secondaryServer;

Monitor::Monitor(uint32_t primaryServerPort) :
    m_primaryServerPort(primaryServerPort),
    m_secondaryServerPort(generateSecondaryServerPort(primaryServerPort)),
    m_primaryUrl("127.0.0.1:"s + std::to_string(m_primaryServerPort)),
    m_secondaryUrl("127.0.0.1:"s + std::to_string(m_secondaryServerPort))
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Monitor::~Monitor()
{
    reset();
}

bool Monitor::init()
{
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
    
    std::string primaryCmd = createServerRunCommand(m_primaryServerPort);
    std::string secondaryCmd = createServerRunCommand(m_secondaryServerPort);

    //serversCreated = runProcess(m_primaryServerPort, s_primaryServer);
    bool serversCreated = runProcess(m_secondaryServerPort, s_secondaryServer);

    return serversCreated;
}

void Monitor::startMonitoring()
{
    while (true)
    {
        std::this_thread::sleep_for(milliseconds(pingPeriodMs));
        ProcessesState runningState = checkRunningAndTryRestart();

        bool primaryUrlActive = false;
        if(runningState.primaryRunning || runningState.secondaryRunning)
            primaryUrlActive = m_pinger.ping(m_primaryUrl);

        bool secondaryUrlActive = false;
        if(runningState.secondaryRunning || runningState.secondaryRunning)
             secondaryUrlActive = m_pinger.ping(m_secondaryUrl);

        if (!primaryUrlActive && !secondaryUrlActive)
            printf("Neither primary, nor secondary servers are not responding\n");

        if (runningState.primaryRunning && runningState.secondaryRunning &&(!primaryUrlActive || !secondaryUrlActive))
        {
            s_primaryServer.terminate();
            s_secondaryServer.terminate();
        }
        else if (runningState.primaryRunning && (!primaryUrlActive || !secondaryUrlActive))
        {
            if (!primaryUrlActive && m_state == WarmSpareState::primaryServerOnPrimaryAddress)
                s_primaryServer.terminate();
            else if (!secondaryUrlActive && m_state == WarmSpareState::secondaryServerOnPrimaryAddress)
                s_primaryServer.terminate();
        }
        else if (runningState.secondaryRunning && (!primaryUrlActive || !secondaryUrlActive))
        {
            if (!secondaryUrlActive && m_state == WarmSpareState::primaryServerOnPrimaryAddress)
                s_secondaryServer.terminate();
            else if (!primaryUrlActive && m_state == WarmSpareState::secondaryServerOnPrimaryAddress)
                s_secondaryServer.terminate();
        }

        if (primaryUrlActive && secondaryUrlActive && m_state == WarmSpareState::secondaryServerOnPrimaryAddress)
        {
            uint32_t swapPort = m_secondaryServerPort + 1;
            std::string swapUrl = "127.0.0.1:"s + std::to_string(swapPort);
            std::string commandToBindSecondaryServerOnSwapPort = "reinit "s + std::to_string(swapPort);
            if (!send(m_primaryUrl, commandToBindSecondaryServerOnSwapPort))
            {
                printf("cannot send reinit command on url %s\n", m_primaryUrl.c_str());
                continue;
            }

            std::string commandToBindPrimaryServerOnPrimaryPort = "reinit "s + std::to_string(m_primaryServerPort);
            if (!send(m_secondaryUrl, commandToBindPrimaryServerOnPrimaryPort))
            {
                printf("cannot send reinit command on url %s\n", m_secondaryUrl.c_str());
                continue;
            }

            std::string commandToBindSecondaryServerOnSecondaryPort = "reinit "s + std::to_string(m_secondaryServerPort);
            if (!send(swapUrl, commandToBindSecondaryServerOnSecondaryPort))
            {
                printf("cannot send reinit command on url %s\n", swapUrl.c_str());
                continue;
            }
            m_state = WarmSpareState::primaryServerOnPrimaryAddress;
        }
        else if (!primaryUrlActive && secondaryUrlActive)
        {
            std::string bindSecondaryServerOnPrimaryAddress = "reinit "s + std::to_string(m_primaryServerPort);
            if (!send(m_secondaryUrl, bindSecondaryServerOnPrimaryAddress))
            {
                printf("cannot send reinit command on url %s\n", m_secondaryUrl.c_str());
            }
            else
            {
                m_state = WarmSpareState::secondaryServerOnPrimaryAddress;
            }
        }
    }
}

bool Monitor::runPrimaryServer(const std::string& cmd)
{
    bool created = s_primaryServer.create(cmd);
    printf(created ? "monitoring primary server \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd.c_str());

    return created;
}

bool Monitor::runSecondaryServer(const std::string& cmd)
{
    bool created = s_secondaryServer.create(cmd);
    printf(created ? "monitoring secondary server \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd.c_str());

    return created;
}

void Monitor::reset()
{
    s_primaryServer.terminate();
    s_secondaryServer.terminate();
}

bool Monitor::runProcess(uint32_t port, OSProcess& p)
{
    std::string runningCommand = createServerRunCommand(port);
    return p.create(runningCommand);;
}

bool Monitor::send(const std::string& url, const std::string& message)
{
    Client client;
    if (!client.init(url))
        return false;

    return client.send(message);
}

ProcessesState Monitor::checkRunningAndTryRestart()
{
    bool primaryRunning = processRunning(s_primaryServer);
    bool secondaryRunning = processRunning(s_secondaryServer);

    ProcessesState runningState{ primaryRunning , secondaryRunning };

    if (!primaryRunning && !secondaryRunning)
    {
        runningState.primaryRunning = runProcess(m_primaryServerPort, s_primaryServer);
        runningState.secondaryRunning = runProcess(m_secondaryServerPort, s_secondaryServer);
        if (!runningState.primaryRunning || !runningState.secondaryRunning)
            printf("cannot create neither primary server on port %d, nor secondary processes on port %d",
                m_primaryServerPort, m_secondaryServerPort);
    }
    else if (!primaryRunning && m_state == WarmSpareState::secondaryServerOnPrimaryAddress)
    {
        runningState.primaryRunning = runProcess(m_secondaryServerPort, s_primaryServer);
        if (!runningState.primaryRunning)
            printf("cannot create primary server on port %d", m_secondaryServerPort);
    }
    else if (!primaryRunning && m_state == WarmSpareState::primaryServerOnPrimaryAddress)
    {
        runningState.primaryRunning = runProcess(m_primaryServerPort, s_primaryServer);
        if (!runningState.primaryRunning)
            printf("cannot create primary server on port %d", m_primaryServerPort);
    }
    else if (!secondaryRunning && m_state == WarmSpareState::primaryServerOnPrimaryAddress)
    {
        runningState.secondaryRunning = runProcess(m_secondaryServerPort, s_secondaryServer);
        if (!runningState.secondaryRunning)
            printf("cannot create secondary server on port %d", m_secondaryServerPort);
    }
    else if (!secondaryRunning && m_state == WarmSpareState::secondaryServerOnPrimaryAddress)
    {
        runningState.secondaryRunning = runProcess(m_primaryServerPort, s_secondaryServer);
        if (!runningState.secondaryRunning)
            printf("cannot create secondary server on port %d", m_primaryServerPort);
    }

    return runningState;
}

uint32_t Monitor::generateSecondaryServerPort(uint32_t primaryServerPort)
{
    return primaryServerPort + 1;
}

std::string Monitor::createServerRunCommand(uint32_t serverPort)
{
    std::string cmd = serverFilename + " "s + std::to_string(serverPort);

    return cmd;
}

bool Monitor::processRunning(OSProcess& p)
{
    return p.wait(3000);
}
