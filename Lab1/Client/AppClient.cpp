#define _CRT_SECURE_NO_WARNINGS
#include "AppClient.h"
#include "../Lib/SocketClient.h"
#include "../Lib/UtilString.h"
#include "../Lib/UtilFile.h"
#include <map>
#include <chrono>
#include <thread>
#include <iostream>

bool Client::send(const std::string& url, const std::string& msg)
{
    SocketClient s;
    bool err = false;
    for (int i = 0; i < 6; i++)
    {
        if (!s.init() || !s.connect(url))
        {
            err = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        else
        {
            err = false;
            break;
        }
    }
    if (err)
    {
        std::cout << "ARQ end";
        return false;
    }

    
    printf("sending text message \"%s\"\n", msg.c_str());
    int len = s.sendStr(msg);

    printf("sent %d bytes\n", len);
    return len > 0;
}
