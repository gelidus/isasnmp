//
// Created by Peter on 10/31/2015.
//

#ifndef ISA_SNMPAGENT_H
#define ISA_SNMPAGENT_H

#include <iostream>
#include <vector>
#include "InterfaceInfoContainer.h"

#ifdef __unix__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <err.h>
#else
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

const int kSNMPVersion = 0x00;
const int kSNMPPort = 161;

class SNMPClient {
private:
		// interface_container_ handles all information that
		// we have about interfaces
		InterfaceInfoContainer interface_container_;

		// last_request_id_ is generator helper variable
		// that is incremented each time the generator is
		// called. This should not be manually incremented
		// as the GenerateRequestID() may be reimplemented
		int last_request_id_;

		// global client settings
		std::string address_;
		std::string community_;
		int interval_;

		// connection details
		struct sockaddr_in server_;
#ifdef __unix__
		int socket_;
		unsigned int server_info_length_;
#else
		SOCKET socket_;
		int server_info_length_;
#endif

public:
		SNMPClient(std::string address, std::string community, int interval);
		virtual ~SNMPClient();

		// Run will start the connection to the
		Error Run();

		Error RetrieveInformation();

		InterfaceInfoContainer& interface_container() { return interface_container_; }

private:
		// SetupConnection creates the socket that will be used
		// for the communication with the SNMP agent. The connection
		// will be set up on the default port and given address
		Error SetupConnection();

		// SendBytes sends the vector of bytes to the client
		// This method may end up with an error, if the data
		// could not be correctly sent to the destination
		Error SendBytes(std::vector<Byte> &msg);

		// ReceiveBytes will store maximum of provided length
		// bytes into the given vector. This method may end up
		// with an error, if the data could not be received.
		// The length is just a maximum, not minimum. The socket
		// will not block if smaller packet can be received.
		Error ReceiveBytes(std::vector<Byte> &bytes, int length);

		Error SendGetPacket(SNMPGetPacket *packet);

		Error ReceiveGetPacket(SNMPGetPacket *packet);

		// CreateGetPacket is a factory that is creating
		// Get packets for the SNMP. Method will return the
		// SNMP packet object, that already has all the defaults.
		SNMPGetPacket *CreateGetPacket(SNMPObjectIdentifier oid);

		int GenerateRequestID();
};


#endif //ISA_SNMPAGENT_H
