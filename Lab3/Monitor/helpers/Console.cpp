#include "Console.h"
#include "../../helpers/UtilString.h"

void (*sCtrlCHandler)() = NULL;

BOOL WINAPI ConsoleCtrlHandler(DWORD type)
{
    if(type == CTRL_C_EVENT && sCtrlCHandler)
        sCtrlCHandler();
    return FALSE;
}

Console::Console()
{
    SetConsoleCtrlHandler(ConsoleCtrlHandler, true);
    system(" "); // enable VT100 terminal support
}


void Console::handleCtrlC(void (*handler)())
{
    sCtrlCHandler = handler;
}

void Console::clearScreen()
{
    fprintf(stderr, "\033[2J\033[1;1H");
}

bool Console::isEscapePressed()
{
    return GetAsyncKeyState(VK_ESCAPE);
}