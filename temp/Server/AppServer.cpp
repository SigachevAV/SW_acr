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

        /*fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), "");*/ // pet the watchdog
        std::shared_ptr<Socket> client = m_socket.accept();
        bool flagFile=false;
        if(!client->isValid())
            continue;

        int n = client->recv();
        char* data = client->data();
        printf("-----RECV-----\n%s\n--------------\n", n > 0 ? data : "Error");

        
        const fs::path pathF(data);
        if (fs::is_regular_file(pathF))
        {
          if (!std::filesystem::exists(".\\files\\" + pathF.filename().string())) {
            std::filesystem::copy_file(data, ".\\files\\" + pathF.filename().string());
            std::cout << "The file has been sent to the server" << std::endl;
            flagFile = true;
          }
          else
            std::cout << "The file with this name already exists on the server" << std::endl;         
        }
        else
          std::cout << "The message is not a file path" << std::endl;
    
        const std::vector<std::string>& tokens = split(data, " ");
        if(tokens.size() >= 2 && tokens[0] == "GET")
        {
            const std::string& filename = join(split(tokens[1], "/"), "\\");
            if(filename == "\\")
            {
                std::string payload = m_data.at(m_data.size() - 1);
                //std::string payload = ".\\files\\" + pathF.filename().string();
                const fs::path pathFile(payload);
                std::ifstream file(payload, std::ios_base::binary);
                std::string line;
                std::string text;
                if (!file.is_open())
                  std::cout << "The file can't be opened"<<std::endl; 
                else
                {
                  while (std::getline(file, line))
                  {
                    text += line + "\n";
                  }
                  file.close();
                }
                if (pathFile.extension() == ".png") {
                  client->sendStr("HTTP/1.1 200 Okay\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + toStr(text.length()) + ";charset=ISO-8859-4 \r\n\r\n" + text);
                }
                else if (pathFile.extension() == ".jpg") {
                  client->sendStr("HTTP/1.1 200 Okay\r\nContent-Type: image/jpg; Content-Transfer-Encoding: binary; Content-Length: " + toStr(text.length()) + ";charset=ISO-8859-4 \r\n\r\n" + text);
                }
                else if (pathFile.extension() == ".gif") {
                  client->sendStr("HTTP/1.1 200 Okay\r\nContent-Type: image/gif; Content-Transfer-Encoding: binary; Content-Length: " + toStr(text.length()) + ";charset=ISO-8859-4 \r\n\r\n" + text);
                }
                else if (pathFile.extension() == ".bmp") {
                  client->sendStr("HTTP/1.1 200 Okay\r\nContent-Type: image/bmp; Content-Transfer-Encoding: binary; Content-Length: " + toStr(text.length()) + ";charset=ISO-8859-4 \r\n\r\n" + text);
                }
                else {
                  std::cout << "The file is not a picture"<<std::endl; 
                }
            }
            else
                client->sendStr("HTTP/1.1 404 Not Found\r\n\r\n");
        }
        else if(n > 0)
        {
          if (flagFile) {
            m_data.push_back(".\\files\\" + pathF.filename().string());
         }
          else {
            m_data.push_back(data);
          }
          fileAppend("resources\\STATE", m_data.back() + "\n");
        }
    }
}
