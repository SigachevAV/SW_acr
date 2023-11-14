#pragma once
#include <windows.h>
#include <string>
#include <vector>

// set of functions for operating Windows processes
class Process
{
public:
    Process() = default;
    virtual ~Process() = default;
    bool create(char* cmd); // creates child process and redirects its stdout to current process
    bool wait(int timeout); // waits for certain period of time and immediately reacts if child process exits during this time
    std::string pid(); // child process' PID
    void terminate(); // kills child process

private:
    HANDLE m_process;
    HANDLE m_thread;
    bool m_init = false;
};
