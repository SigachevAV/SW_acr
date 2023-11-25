#include <windows.h>
#include "AppMonitor.h"
#include <thread>
#include <vector>

void Monitoring()
{
    Monitor m;
    while (true)
    {
        if (!m.check())
        {
            m.init();
        }
        Sleep(1000);
    }
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; i++)
    {
        threads.push_back(std::thread(Monitoring));
    }

    for (std::thread& th : threads)
    {
        // If thread Object is Joinable then Join that thread.
        if (th.joinable())
            th.join();
    }
}