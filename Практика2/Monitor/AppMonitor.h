#pragma once
#include "helpers/Console.h"

// Monitor app that manages Server's lifecycle and monitors its state
class Monitor
{
public:
    Monitor() = default;
    virtual ~Monitor() = default;
    bool init(); // launches Server
    bool check(); // checks Server state
    static void reset(); // terminates irresponsive Server

private:
    Console m_console;
};
