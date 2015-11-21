//
// Created by Peter on 10/31/2015.
//

#include "SNMPClient.h"
#include <memory.h>
#include <vector>

using namespace std;

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

	// first packet for the interface table
	SNMPGetPacket ifTablePacket{
			SNMPDataType::Sequence,
			SNMPInteger{kSNMPVersion},
			SNMPOctetString{community_},
			SNMPPDU{
					SNMPDataType::GetNextRequest,
					SNMPInteger{1}, // request_id
					SNMPInteger{0}, // error
					SNMPInteger{0}, // error index
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

	err = SendGetPacket(&ifTablePacket);
	if (err != Error::None) {
		return err;
	}

	SNMPGetPacket response{};
	err = ReceiveGetPacket(&response);
	if (err != Error::None) {
		return err;
	}

	cout << "id: " << response.pdu().request_id().value();
	cout << "oid: ";
	auto oidlist = response.pdu().varbinds().iterator()->identifier().value();
	for (auto b = oidlist.begin(); b != oidlist.end(); b++) {
		cout << (*b) << " ";
	}

	return Error::None;
}

Error SNMPClient::SetupConnection() {
	// create datagram socket
	if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return Error::CannotCreateSocket;
	}

#ifndef __unix__
	server_.sin_addr.s_addr = inet_addr(this->address_.c_str());
#else
	int conversion = inet_aton(this->address_.c_str(), &server_.sin_addr);
	if (conversion == 0) {
		// given input could not be translated
		struct hostent *host;
		if ((host = gethostbyname(this->address_.c_str())) == NULL) {
			return Error::CannotTranslateAddress;
		}
		memcpy(&server_.sin_addr, host->h_addr_list[0], host->h_length);
	}
#endif
	server_.sin_family = AF_INET;
	server_.sin_port = htons(kSNMPPort);

	server_info_length_ = sizeof(server_);

	cout << "Connected to the server" << endl;

	return Error::None;
}

Error SNMPClient::SendBytes(vector<Byte> &msg, int length) {
	// try to send provided data into the socket
	int sent = sendto(socket_, reinterpret_cast<const char*>(msg.data()), length, 0, (struct sockaddr*)&server_, sizeof(server_));
	if (sent == -1) {
		return Error::CannotSendData;
	} else if (sent != length) {
		return Error::CannotSendFullData;
	}

	return Error::None;
}

Error SNMPClient::ReceiveBytes(vector<Byte> &to, int length) {
	char *buffer = new char[length];

	int recvd = recvfrom(socket_, buffer, length, 0, (struct sockaddr*)&server_, &server_info_length_);
	if (recvd == -1) {
		return Error::CannotReceiveData;
	}

	to.insert(to.end(), &buffer[0], &buffer[recvd]);
	delete buffer;

	return Error::None;
}

Error SNMPClient::SendGetPacket(SNMPGetPacket *packet) {

	// Marshal packet into the bytes queue
	std::vector<Byte> bytes{};
	Error err = packet->Marshal(bytes);
	if (err != Error::None) {
		return err;
	}

	cout << "Sending bytes " << bytes.size() << endl;

	return SendBytes(bytes, static_cast<int>(bytes.size()));
}

Error SNMPClient::ReceiveGetPacket(SNMPGetPacket *packet) {

	// we must receive the whole message from the
	// socket. Other than that, the message remaining
	// will be discarded
	vector<Byte> bytes{};
	ReceiveBytes(bytes, 1024);

	// create list for the unmarshal
	std::list<Byte> byte_list{};
	byte_list.assign(bytes.begin(), bytes.end());

	// unmarshal
	return packet->Unmarshal(byte_list);
}
