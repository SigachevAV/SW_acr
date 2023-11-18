#define _CRT_SECURE_NO_WARNINGS
#include "AppServer.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <string>
#include <process.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Shlwapi.h"
namespace fs = std::filesystem;

bool Server::init(int port)
{
    if(!m_socket.init(1000) || !m_socket.listen(port))
        return false;

    /*if(!fileWriteExclusive("resources\\CREATED", toStr(m_socket.port()) + "," + toStr(_getpid())))
        return false;*/

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
        //fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), ""); // pet the watchdog
        std::shared_ptr<Socket> client = m_socket.accept(); // accept incoming connection
        if (!client->isValid())
            continue;

        int n = client->recv(); // receive data from the connection, if any
        char* data = client->data();
        printf("-----RECV-----\n%s\n--------------\n", n > 0 ? data : "Error");
        const std::filesystem::path pathF(data);
        const std::vector<std::string>& tokens = split(data, " ");
        if (tokens.size() >= 2 && tokens[0] == "GET") // this is browser's request
        {

            if(tokens[1].find("files") != std::string::npos)
            {
                std::string payload = "." + tokens[1];
                //std::string payload = ".\\files\\" + pathF.filename().string();
                const fs::path pathFile(payload);
                std::ifstream file(payload, std::ios_base::binary);
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
                std::string extention = pathFile.extension().generic_string();
                client->sendStr("HTTP/1.1 200 Okay\r\nContent-Type: image/"+ extention.substr(1, extention.length()) +"; Content - Transfer - Encoding: binary; Content - Length: " + toStr(text.length()) + "; charset = ISO - 8859 - 4 \r\n\r\n" + text);
            }
            // convert URL to file system path, e.g. request to img/1.png resource becomes request to .\img\1.png file in Server's directory tree
            
            else

                {
                    const std::string& filename = join(split(tokens[1], "/"), "\\");
                    if (filename == "\\")
                    { // main entry point (e.g. http://localhost:12345/)
                        std::string payload = "<p>Laba</p>";
                        for (auto s : m_data)
                        {
                            if (std::filesystem::exists(s))
                            {
                                payload += "<img src = \"" + s + "\" alt = \"" + s + "\">";
                            }
                            else
                            {
                                payload += ("<p>" + s + "<br></p>"); // collect all the feed and send it back to browser
                            }

                        }
                        payload += "";
                        client->sendStr("HTTP/1.1 200 OK\\r\n\Content-Type: html\r\nContent-Length: " + toStr(payload.length()) + "\r\n\r\n" + payload);
                    }
                };
        }
        else if (tokens.size() >=2 && tokens[0] == "POST")
        {
            if (tokens[1] == "file")
            {
                std::filesystem::path path(".\\files\\" + tokens[2]);
				if (!std::filesystem::exists(path))
				{
					std::ofstream file(path, std::ios_base::binary);
                    if (file.is_open())
                    {
                        int offset = 0;
                        for (int i = 0; i < 3; i++)
                        {
                            offset += tokens[i].length() + 1;
                        }
                        std::string fileBuffer = std::string(data).substr(offset, n-offset);
                        file.write(data+offset, n-offset);
                        file.close();
                    }
				}
                else
                    std::cout << "The file with this name already exists on the server" << std::endl;
                m_data.push_back(".\\files\\" + path.filename().string());

            }
            else
            {
                m_data.push_back(tokens[1]); // store it in the feed
                fileAppend("resources\\STATE", m_data.back() + "\n"); // store it in the file for subsequent runs

            }
        }
    }
}
