#define _CRT_SECURE_NO_WARNINGS
#include "AppClient.h"
#include "helpers/SocketClient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <map>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <process.h>
#include <algorithm>

bool Client::send(const std::string& url, const std::string& msg)
{
    SocketClient s;
    if(!s.init() || !s.connect(url))
        return false;

    printf("sending text message \"%s\"\n", msg.c_str());
    std::filesystem::path path(msg);
    std::string payload = "POST ";
    if (std::filesystem::is_regular_file(path))
    {
        payload += "file ";
        std::ifstream file(path, std::ios_base::binary);
        std::string line;
        std::string text;
        if (!file.is_open())
            std::cout << "The file can't be opened" << std::endl;
        else
        {
            while (std::getline(file, line))
            {
                text += line + "\n";
            }
            file.close();
        }
        payload += path.filename().generic_string() + " ";
        payload += text;
    }
    else
    {
        payload += msg;
    }
    int len = s.sendStr(payload);
    printf("sent %d bytes\n", len);
    return len > 0;
}
