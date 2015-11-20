//
// Created by Peter on 10/31/2015.
//

#ifndef ISA_SNMPAGENT_H
#define ISA_SNMPAGENT_H

#include <iostream>
#include "SNMPPacket.h"

#ifdef __unix__
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<err.h>
#include <fcntl.h>
#else
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

const int SNMPVersion = 0x00;
const int SNMPPort = 161;

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
		unsigned long dont_block_ = 1;
#endif

public:
		SNMPClient(std::string address, std::string community, int interval);
		virtual ~SNMPClient();

		Error Run();

private:
		Error SetupConnection();

		Error SendBytes(Byte *msg, unsigned long long length);

		Error ReceiveBytes(std::vector<Byte> &bytes);

		Error SendGetPacket(SNMPGetPacket *packet);

		Error ReceiveGetPacket(SNMPGetPacket *packet);
};


#endif //ISA_SNMPAGENT_H
