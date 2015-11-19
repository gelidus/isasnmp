//
// Created by Peter on 10/31/2015.
//

#include "SNMPClient.h"

SNMPClient::SNMPClient(std::string address, std::string community, int interval) {
	address_ = address;
	community_ = community;
	interval_ = interval;
}

SNMPClient::~SNMPClient() {
#ifdef __unix__
	close(socket_);
#else
	closesocket(socket_);
#endif
}

Error SNMPClient::Run() {
	Error err = SetupConnection();
	if (err != Error::None) {
		return err;
	}

	for (;;) {

	}
}

Error SNMPClient::SetupConnection() {
	// create datagram socket
	if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return Error::CannotCreateSocket;
	}

	server_.sin_addr.s_addr = inet_addr(this->address_.c_str());
	server_.sin_family = AF_INET;
	server_.sin_port = htons(SNMPPort);

	return Error::None;
}

Error SNMPClient::SendMessage(char *msg, int length) {
	// try to send provided data into the socket
	int sent = sendto(socket_, msg, length, 0, (struct sockaddr*)&server_, sizeof(server_));
	if (sent == -1) {
		return Error::CannotSendData;
	} else if (sent != length) {
		return Error::CannotSendFullData;
	}

	server_info_length_ = sizeof(server_);

	return Error::None;
}

Error SNMPClient::ReceiveMessage(int length, char *msg) {

	char buffer[length];

	int recvd = recvfrom(socket_, buffer, length, 0, (struct sockaddr*)&server_, &server_info_length_);
	if (recvd == -1) {
		return Error::CannotReceiveData;
	}

	return Error::None;
}