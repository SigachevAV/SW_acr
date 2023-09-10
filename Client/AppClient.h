#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <string>

// command-line Client app that can send messages to Server.
class Client
{
public:
    Client() = default;
    virtual ~Client() = default;

    bool send(const std::string& url, const std::string& msg); // sends message to Server
};
