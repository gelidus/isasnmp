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

	last_request_id_ = 1;
}

SNMPClient::~SNMPClient() {
#ifdef __unix__
	close(socket_);
#else
	closesocket(socket_);
#endif
}

Error SNMPClient::Run() {
	RetrieveInformation();

	return Error::None;
}

Error SNMPClient::RetrieveInformation() {
	Error err = SetupConnection();
	if (err != Error::None) {
		return err;
	}

	list<Byte> oid{1, 3, 6, 1, 2, 1, 2, 2};

	do {
		SNMPGetPacket *nextRequest = CreateGetPacket(SNMPObjectIdentifier{oid});
		err = SendGetPacket(nextRequest);
		delete nextRequest; // delete packet as we dont need it

		if (err != Error::None) {
			return err;
		}

		SNMPGetPacket response{};
		err = ReceiveGetPacket(&response);
		if (err != Error::None) {
			return err;
		}

		err = interface_container_.ProcessPacket(&response);
		// next iteration is on the given object
		oid = response.pdu().varbinds().binds().begin()->identifier().value();
	} while (err == Error::None);

	if (err == Error::SNMPNotAnInterface) {
		return Error::None; // NotAnInterface is wanted status
	} else {
		return err; // else we should return the correct error
	}

	interface_container_.OutputResults();
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

Error SNMPClient::SendBytes(vector<Byte> &msg) {
	// try to send provided data into the socket
	int sent = sendto(socket_, reinterpret_cast<const char*>(msg.data()), static_cast<int>(msg.size()), 0, (struct sockaddr*)&server_, sizeof(server_));
	if (sent == -1) {
		return Error::CannotSendData;
	} else if (sent != static_cast<int>(msg.size())) {
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

	return SendBytes(bytes);
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


SNMPGetPacket *SNMPClient::CreateGetPacket(SNMPObjectIdentifier oid) {
	return new SNMPGetPacket{
			SNMPDataType::Sequence,
			SNMPInteger{kSNMPVersion},
			SNMPOctetString{community_},
			SNMPPDU{
					SNMPDataType::GetNextRequest,
					SNMPInteger{GenerateRequestID()}, // request_id
					SNMPInteger{0}, // error
					SNMPInteger{0}, // error index
					SNMPVarbindList{
							list<SNMPVarbind>{
									SNMPVarbind{ // add varbind for the object of iftable
											oid,
											//SNMPObjectIdentifier{
											//		list<Byte>{1, 3, 6, 1, 2, 1, 2, 2} // if table object
											//},
											SNMPValue{
													SNMPDataType::Null,
													nullptr
											}
									}
							}
					}
			}
	};
}

int SNMPClient::GenerateRequestID() {
	return last_request_id_++;
}
