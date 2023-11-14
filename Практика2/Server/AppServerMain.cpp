#include "windows.h"
#include "AppServer.h"

int main (int argc, char *argv[])
{
    int port = argc > 1 ? atoi(argv[1]) : 0;
    Server s;
    while(!s.init(port))
        Sleep(1000);
    s.run();
}