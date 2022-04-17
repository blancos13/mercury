#pragma once
#include "sandbird/sandbird.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")
#include <string>
#include <iostream>
namespace http {
	int handler(sb_Event* evt);
	void run(std::string local_ip, int local_port,std::string meta2);
}

std::string redirect_host;
int redirect_port;
sb_Options options;
sb_Server* http_server;

std::string meta0;

int http::handler(sb_Event* evt) {
	if (evt->type == SB_EV_REQUEST) {
		
		if (strstr(evt->path, "/growtopia/server_data.php") != nullptr) {
			sb_send_status(evt->stream, 200, "OK");
			sb_send_header(evt->stream, "Content-Type", "text/plain");
			std::string server_data = "server|" + redirect_host + "\nport|" + std::to_string(redirect_port) + "\ntype|1\nmeta|" + meta0 + "\nRTENDMARKERBS1001\n";
			const char* response = server_data.c_str();
			sb_writef(evt->stream, response);
		}
	}
	return SB_RES_OK;
}


void http::run(std::string local_ip, int local_port,std::string meta2) {
	redirect_host = local_ip;
	redirect_port = local_port;
	meta0 = meta2;
	options.host = "0.0.0.0";
	options.port = "80";
	options.handler = handler;
	http_server = sb_new_server(&options);
	if (!http_server) {
		std::cout << "[Error] Couldn't bind 80 port, It's already running?\n";
		system("PAUSE");
		exit(0);
	} 
	else {
		std::cout << "[HTTP] Service started, listening on 80 port\n";
	} 
	while (true) {
		sb_poll_server(http_server, 5);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	sb_close_server(http_server);
}