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

	// first packet for the interface table
	SNMPGetPacket ifTablePacket{
			SNMPDataType::Sequence,
			kSNMPVersion,
			community_,
			SNMPPDU{
					1, // request_id
					0, // error
					0, // error index
					SNMPVarbindList{
							list<SNMPVarbind>{
									SNMPVarbind{ // add varbind for the object of iftable
											SNMPObjectIdentifier{
													list<Byte>{1, 3, 6, 1, 2, 1, 2, 2} // if table object
											},
											SNMPValue{
													SNMPDataType::Null,
													nullptr
											}
									}
							}
					}
			}
	};

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
	server_.sin_port = htons(kSNMPPort);

	server_info_length_ = sizeof(server_);

#ifdef __unix__
	int flags = fcntl(socket_, F_GETFL, 0);
  fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
#else
	ioctlsocket(socket_, FIONBIO, &dont_block_);
#endif

	return Error::None;
}

Error SNMPClient::SendBytes(list<Byte> &msg, unsigned long long length) {
	// try to send provided data into the socket
	int sent = sendto(socket_, reinterpret_cast<const char*>(msg.front()), static_cast<int>(length), 0, (struct sockaddr*)&server_, sizeof(server_));
	if (sent == -1) {
		return Error::CannotSendData;
	} else if (sent != length) {
		return Error::CannotSendFullData;
	}

	return Error::None;
}

Error SNMPClient::ReceiveBytes(list<Byte> &to) {

	const int kBufferSize = 100;
	char buffer[kBufferSize];

	int recvd = recvfrom(socket_, buffer, kBufferSize, 0, (struct sockaddr*)&server_, &server_info_length_);
	if (recvd == -1) {
		return Error::CannotReceiveData;
	}

	for (int i = 0; i < recvd; i++) {
		to.push_back(buffer[i]);
	}

	return Error::None;
}

Error SNMPClient::SendGetPacket(SNMPGetPacket *packet) {

	// Marshal packet into the bytes queue
	std::list<Byte> bytes{};
	Error err = packet->Marshal(bytes);
	if (err != Error::None) {
		return err;
	}

	return SendBytes(bytes, bytes.size());
}

Error SNMPClient::ReceiveGetPacket(SNMPGetPacket *packet) {

	// create queue of bytes and unmarshal the packet
	// from it
	std::list<Byte> bytes{};
	ReceiveBytes(bytes);

	Error err = packet->Unmarshal(bytes);
	if (err != Error::None) {
		return err;
	}

	return Error::None;
}
