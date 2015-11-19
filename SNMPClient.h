//
// Created by Peter on 10/31/2015.
//

#ifndef ISA_SNMPAGENT_H
#define ISA_SNMPAGENT_H

#include <iostream>

#ifdef __unix__
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<err.h>
#else
#include <winsock2.h>
#include "SNMPPacket.h"

#pragma comment(lib,"ws2_32.lib")
#endif

const int SNMPVersion = 0x00;
const int SNMPPort = 161;

enum SNMPProtocol : unsigned char {
		GetNextRequest = 0xa1,
		GetResponse = 0xa2
};

class SNMPClient {
private:
		// global client settings
		std::string address_;
		std::string community_;
		int interval_;

		// connection details
		struct sockaddr_in server_;
#ifdef __unix__
		int socket_;
		socketlen_t server_info_length;
#else
		SOCKET socket_;
		int server_info_length_;
#endif

public:
		SNMPClient(std::string address, std::string community, int interval);
		virtual ~SNMPClient();

		Error Run();

private:
		Error SetupConnection();

		Error SendMessage(char *msg, int length);

		Error ReceiveMessage(int length, char *msg);

		Error SendPacket(SNMPPacket *packet);

		Error ReceivePacket(SNMPPacket *packet);
};


#endif //ISA_SNMPAGENT_H
