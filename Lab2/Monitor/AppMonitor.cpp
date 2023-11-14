#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include "AppMonitor.h"
#include "helpers/Process.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"

static Process sServer;
static std::string sPort = "60767";

bool Monitor::init()
{
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sServer.create(cmd); // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);
    return ok;
}

bool Monitor::check()
{
    sServer.wait(3000);
    return true;
}

void Monitor::reset()
{
    sServer.terminate();
}
