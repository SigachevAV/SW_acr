#include "windows.h"
#include "AppServer.h"
#include <iostream>


int main (int argc, char *argv[])
{
    std::setlocale(LC_ALL, "rus");

    /*int port = 60765;*/
    /*if (argc < 2)
    {
        printf("USAGE: Server.exe <PORT>\n\n");
        printf("Example: Server.exe 12345 \n");
        return 0;
    }*/
    //int port = argc > 1 ? atoi(argv[1]) : 0;
    Server s;
    int port;
    //port = atoi(argv[1]);
    port = 60767;
    while(!s.init(port))
        Sleep(1000);
    s.run();
}