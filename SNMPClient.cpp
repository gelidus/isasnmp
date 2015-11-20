//
// Created by Peter on 10/31/2015.
//

#include "SNMPClient.h"
#include <memory>
#include <vector>

using namespace std;

SNMPClient::SNMPClient(std::string address, std::string community, int interval) {
	address_ = address;
	community_ = community;
	interval_ = interval;

#ifndef __unix__
	dont_block_ = 1;
#endif
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

	server_info_length_ = sizeof(server_);

#ifdef __unix__
	int flags = fcntl(socket_, F_GETFL, 0);
  fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
#else
	ioctlsocket(socket_, FIONBIO, &dont_block_);
#endif

	return Error::None;
}

Error SNMPClient::SendBytes(Byte *msg, unsigned long long length) {
	// try to send provided data into the socket
	int sent = sendto(socket_, reinterpret_cast<const char*>(msg), static_cast<int>(length), 0, (struct sockaddr*)&server_, sizeof(server_));
	if (sent == -1) {
		return Error::CannotSendData;
	} else if (sent != length) {
		return Error::CannotSendFullData;
	}

	return Error::None;
}

Error SNMPClient::ReceiveBytes(vector<Byte> &bytes) {

	const int kBufferSize = 100;
	char buffer[kBufferSize];
	vector<Byte> vec_buffer{};

	int recvd = recvfrom(socket_, buffer, kBufferSize, 0, (struct sockaddr*)&server_, &server_info_length_);
	if (recvd == -1) {
		return Error::CannotReceiveData;
	}

	return Error::None;
}

Error SNMPClient::SendGetPacket(SNMPGetPacket *packet) {

	// Marshal packet into the bytes vector
	std::vector<Byte> bytes{};
	Error err = packet->Marshal(bytes);
	if (err != Error::None) {
		return err;
	}

	return SendBytes(bytes.data(), bytes.size());
}

Error SNMPClient::ReceiveGetPacket(SNMPGetPacket *packet) {

	// create vector of bytes and unmarshal the packet
	// from it
	std::vector<Byte> bytes{};
	ReceiveBytes(bytes);

	Error err = packet->Unmarshal(bytes);
	if (err != Error::None) {
		return err;
	}

	return Error::None;
}
