#include <windows.h>

#include "Monitor.h"

int main()
{
    constexpr uint32_t serverPort = 12345;

    try
    {
        Monitor m{ serverPort };
        if (!m.init()) // in practice some sort of alarm should be raised here
            return 0;

        m.startMonitoring();
    }
    catch (std::exception& e)
    {
        printf("exception in monitor: %s", e.what());
    }
}