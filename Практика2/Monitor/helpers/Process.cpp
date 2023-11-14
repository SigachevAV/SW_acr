#include <windows.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include "Process.h"
#include "UtilString.h"

bool Process::create(char* cmd)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    bool ok = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    m_process = pi.hProcess;
    m_thread = pi.hThread;
    m_init = ok;
    return ok;
}

bool Process::wait(int timeout)
{
    return m_init ? WaitForSingleObject(m_process, timeout) == WAIT_TIMEOUT : false;
}

std::string Process::pid()
{
    return m_init ? toStr(GetProcessId(m_process)) : "";
}

void Process::terminate()
{
    TerminateProcess(m_process, 0);
    CloseHandle(m_process);
    CloseHandle(m_thread);
    m_init = false;
}