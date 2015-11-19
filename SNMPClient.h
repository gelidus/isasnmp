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
#pragma comment(lib,"ws2_32.lib")
#endif

const int SNMPVersion = 0x00;
const int SNMPPort = 161;

enum class Error {
		None,
		CannotCreateSocket,
		CannotSendData,
		CannotSendFullData,
		CannotReceiveData,
};

enum class SNMPProtocol {
		GetNextRequest = 0xa1,
};

class SNMPClient {
private:
		std::string address_;
		std::string community_;
		int interval_;

#ifdef __unix__
		int socket_;
#else
		SOCKET socket_;
#endif

		struct sockaddr_in server_;
		int server_info_length_;

public:
		SNMPClient(std::string address, std::string community, int interval);
		virtual ~SNMPClient();

		int Run();

private:
		Error SetupConnection();

		Error SendMessage(char *msg, int length);

		Error ReceiveMessage(int length, char *msg);
};


#endif //ISA_SNMPAGENT_H
