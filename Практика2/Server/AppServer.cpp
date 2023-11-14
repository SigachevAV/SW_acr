#define _CRT_SECURE_NO_WARNINGS
#include "AppServer.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <string>
#include <process.h>

bool Server::init(int port)
{
    if(!m_socket.init(1000) || !m_socket.listen(port))
        return false;

    if(!fileWriteExclusive("resources\\CREATED", toStr(m_socket.port()) + "," + toStr(_getpid())))
        return false;

    printf("server started: port %d, pid %d\n", m_socket.port(), _getpid());

    char* state = fileReadStr("resources\\STATE"); // load state from previous run
    if(state)
    {
        for(std::string& line : split(state, "\n"))
            if(!line.empty())
                m_data.push_back(line);
        delete [] state;
    }

    return true;
}

void Server::run()
{
    while(1)
    {
        fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), ""); // pet the watchdog
        std::shared_ptr<Socket> client = m_socket.accept(); // accept incoming connection
        if(!client->isValid())
            continue;

        int n = client->recv(); // receive data from the connection, if any
        char* data = client->data();
        printf("-----RECV-----\n%s\n--------------\n", n > 0 ? data : "Error");
        const std::vector<std::string>& tokens = split(data, " ");
        if(tokens.size() >= 2 && tokens[0] == "GET") // this is browser's request
        {
            // convert URL to file system path, e.g. request to img/1.png resource becomes request to .\img\1.png file in Server's directory tree
            const std::string& filename = join(split(tokens[1], "/"), "\\");
            if(filename == "\\")
            { // main entry point (e.g. http://localhost:12345/)
                std::string payload = "";
                for(auto s : m_data)
                    payload += (s+"<br>"); // collect all the feed and send it back to browser
                client->sendStr("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + toStr(payload.length()) + "\r\n\r\n" + payload);
            }
            else
                client->sendStr("HTTP/1.1 404 Not Found\r\n\r\n");
        }
        else if(n > 0) // this is Client's request who wants to upload some data
        {
            m_data.push_back(data); // store it in the feed
            fileAppend("resources\\STATE", m_data.back() + "\n"); // store it in the file for subsequent runs
        }
    }
}
