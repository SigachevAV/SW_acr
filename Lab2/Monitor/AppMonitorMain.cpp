#include <windows.h>
#include "AppMonitor.h"

int main()
{
    Monitor m;
    if(!m.init()) // in practice some sort of alarm should be raised here
        return 0;
    
    if(!m.check())
        m.reset();
}