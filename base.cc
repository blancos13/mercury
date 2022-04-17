/**********************************************************************************
    Mercury Growtopia ENet Proxy
    Copyright (C) 2022 Viam1101
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************************************/

#include <iostream>
#include <thread>
#include <vector>
#include "http.h"
#include "enet/include/enet.h"
#include <WinNls.h>
#include <WinInet.h>
#include <regex>
#include <fstream>
#include <random>


ENetHost* client_;
ENetHost* server_;
ENetPeer* cpeer;
ENetPeer* speer;

using namespace std;
vector<thread> threads;



#include "packet.hpp"
#include "client.hpp"
#include "server.hpp"

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

void send_http() {

    static CHAR header[] = "Content-Type: application/x-www-form-urlencoded\r\n";
    static CHAR data[] = "version=3%2E82&platform=0&protocol=158";
    
    
    std::string temp;
    if (private_server) temp = server_ip;
    else temp = "growtopia1.com";

    LPCSTR ip = temp.c_str();
    HINTERNET hInternet = InternetOpenA("", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    HINTERNET hConnection = InternetConnectA(hInternet, ip, 80, " ", " ", INTERNET_SERVICE_HTTP, 0, 0);
    HINTERNET hData = HttpOpenRequestA(hConnection, "POST", "/growtopia/server_data.php", "HTTP/1.0", NULL, NULL, NULL, 0);
    HttpSendRequestA(hData, header, strlen(header), data, strlen(data));
    
    std::string res;
    const int nBuffSize = 1024;
    char buff[nBuffSize];

    BOOL bKeepReading = true;
    DWORD dwBytesRead = -1;

    while (bKeepReading && dwBytesRead != 0)
    {
        bKeepReading = InternetReadFile(hData, buff, nBuffSize, &dwBytesRead);
        res.append(buff, dwBytesRead);
    }

    InternetCloseHandle(hData);
    InternetCloseHandle(hConnection);
    InternetCloseHandle(hInternet);

    
    string delimiter = "|";
    vector<string> v = split(res.c_str(), delimiter);
    if (v.size() < 3)
    {
        cout << "[Server Data] Error - Request Invalid" << endl;
        system("PAUSE");
        system("exit");
    }

    int a = res.find("port|");
    string b = res.substr(a + 5);
    int c = b.find("\n");
    string d = b.substr(0, c);
    server_port = std::atoi(d.c_str());

    
    int x = res.find("server|");
    string y = res.substr(x + 7);
    int z = y.find("\n");
    string viam = y.substr(0, z);
    replacestr(viam, "\n", "");
    server_ip = viam;

    if (res.find("meta|") != string::npos)
    {
        int a = res.find("meta|");
        string b = res.substr(a + 5);
        int c = b.find("\n");
        string d = b.substr(0, c);
        meta = d;
    }
    
    cout << "[Server Data] Target set to (" + server_ip + "):" + to_string(server_port) << endl;
    return;
}
int main()
{
    system("color c");

    ifstream file;
    file.open("config.txt");
    if (file)
    {
        for (string line; getline(file, line);) {
            string delimiter = "|";
            vector<string> a = split(line.c_str(), delimiter);
            if (a.at(0) == "server_ip") server_ip = a.at(1);
            else if (a.at(0) == "private_server") private_server = (a.at(1) == "true" ? true : false);
            else if (a.at(0) == "custom_port") custom_port = atoi((a.at(1) == "auto" ? "0" : a.at(1)).c_str());

        }

        file.close();

    }
    else {
        ofstream viam("config.txt");
        viam << "server_ip|1.3.3.7\ncustom_port|auto\nprivate_server|false\n";
        viam.close();
        file.close();
        cout << "[Config] Couldn't find config.json but created one\n";
        system("PAUSE");
        system("exit");
    }
    
    if (!private_server) {
        //wont mess with real gt
        //server_ip = "213.179.209.168";
        //server_port = 17196;
        cout << "[Error] Sorry, I haven't settle the auth of real server yet\n";
        system("PAUSE");
        system("exit");
    }
    if (custom_port == 0) send_http();
    else server_port = custom_port;

    threads.push_back(thread(http::run, local_host,local_port,meta));
    Sleep(100);

    

	enet_initialize();
    ENetAddress addr;
    enet_address_set_host(&addr, "0.0.0.0");
    addr.port = local_port;
    client_ = enet_host_create(&addr, 1024, 10, 0, 0);
    client_->usingNewPacket = false;
    if (!client_) {
        cout << "[Client] Error - Couldn't start ENet service" << endl;
        system("PAUSE");
        return 0;
    }
    client_->checksum = enet_crc32;
    auto code = enet_host_compress_with_range_coder(client_);
    if (code != 0) {
        cout << "[Client] Error - ENet Host compress" << endl;
        system("PAUSE");
        return 0;
    }

    cout << "[Client] Started ENet service" << endl;
   
    server_ = enet_host_create(0, 1, 2, 0, 0);
    server_->usingNewPacket = (private_server ? false : true);
    if (!server_) {
        cout << "[Server] Error - Couldn't start ENet service" << endl;
        system("PAUSE");
        return 0;
    }
    server_->checksum = enet_crc32;
    auto code2 = enet_host_compress_with_range_coder(server_);
    if (code2 != 0) {
        cout << "[Server] Error - ENet host compress" << endl;
        system("PAUSE");
        return 0;
    }
    enet_host_flush(server_);

    cout << "[Server] Started ENet service" << endl;
        

    while (true)
    {
        Sleep(1); //decrease cpu usage
        clientevent_(); //packet transmission between client and proxy
        if (!server_) continue;
        serverevent_(); //packet transmission between proxy and server
    }
    cout << "[System] Program Ended" << endl;
    system("PAUSE");
    return -1;
}
