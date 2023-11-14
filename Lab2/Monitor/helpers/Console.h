#pragma once
#include <windows.h>
#include <string>
#include <vector>

// helper functions for a console app
class Console
{
public:
    Console();
    virtual ~Console() = default;
    void handleCtrlC(void (*handler)()); // allows to specify a function for handling Ctrl+C keypress
    void clearScreen(); // clears console screen
    bool isEscapePressed(); // checks if ESC key is pressed
};
