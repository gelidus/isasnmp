//
// Created by Peter on 11/15/2015.
//

#include "SNMPPacket.h"

SNMPPacket::SNMPPacket(Byte version, std::string community, Byte request_id) {
	version_ = version;
	community_ = community;
	request_id_ = request_id;
}

Error SNMPPacket::Marshal(std::vector<unsigned char> &bytes) {

	return Error::None;
}

Error SNMPPacket::Unmarshal(std::vector<unsigned char> &bytes) {

	return Error::None;
}

void SNMPPacket::set_error(Byte error) {
	error_ = error;
}

Byte SNMPPacket::error() {
	return error_;
}

void SNMPPacket::set_error_index(Byte index) {
	error_index_ = index;
}

Byte SNMPPacket::error_index() {
	return error_index_;
}
