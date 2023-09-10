#include "AppServer.h"
#include "windows.h"

int main (int argc, char *argv[])
{
    int port = argc > 1 ? atoi(argv[1]) : 0;
    Server s;
    s.init(port);
    s.run();
}