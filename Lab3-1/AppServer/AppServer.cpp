#include "windows.h"
#include "Server.h"

int main(int argc, char* argv[])
{
    try
    {
        int port = argc > 1 ? atoi(argv[1]) : 0;
        Server s;
        if (!s.init(port))
        {
            printf("cannot init server on port %d\n", port);
            return EXIT_FAILURE;
        }
        s.run();
    }
    catch (std::exception& e)
    {
        printf("Server exception: %s", e.what());
    }
    catch (...)
    {
        printf("Server unhandled exception");
    }
}