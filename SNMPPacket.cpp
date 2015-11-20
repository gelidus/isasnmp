//
// Created by Peter on 11/15/2015.
//

#include "SNMPPacket.h"

SNMPInteger::SNMPInteger() {
	set_type(SNMPDataType::Integer);
}

SNMPInteger::SNMPInteger(int value) {
	value_ = value;

	set_type(SNMPDataType::Integer);
}

SNMPInteger::~SNMPInteger() {}

Error SNMPInteger::Marshal(std::list<Byte> &to) {
	to.push_back(SNMPDataType::Integer);
	to.push_back(sizeof(Byte));
	to.push_back(static_cast<Byte>(value_));

	return Error::None;
}

Error SNMPInteger::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front(); // data type

	from.pop_front(); // length

	value_ = from.front(); // retrieve value from of the integer
	from.pop_front();

	return Error::None;
}

Byte SNMPInteger::length() {
	return kSNMPHeaderSize + sizeof(Byte);
}


SNMPOctetString::SNMPOctetString() {
	set_type(SNMPDataType::OctetString);
}

SNMPOctetString::SNMPOctetString(std::string value) {
	value_ = value;
	set_type(SNMPDataType::OctetString);
}

SNMPOctetString::~SNMPOctetString() {}

Error SNMPOctetString::Marshal(std::list<Byte> &to) {
	to.push_back(type());
	to.push_back(length());

	for (auto it = value_.begin(); it != value_.end(); it++) {
		to.push_back(static_cast<Byte>(*it));
	}

	return Error::None;
}

Error SNMPOctetString::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPOctetString::length() {
	return static_cast<Byte>(value_.length());
}


SNMPObjectIdentifier::SNMPObjectIdentifier() {

}

SNMPObjectIdentifier::SNMPObjectIdentifier(std::list<Byte> value) {

}

SNMPObjectIdentifier::~SNMPObjectIdentifier() {

}

Error SNMPObjectIdentifier::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPObjectIdentifier::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPObjectIdentifier::length() {
	return 0;
}


SNMPValue::SNMPValue() {

}

SNMPValue::SNMPValue(SNMPDataType type, SNMPEntity *value) {

}

SNMPValue::~SNMPValue() {

}

Error SNMPValue::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPValue::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPValue::length() {
	return 0;
}


SNMPVarbind::SNMPVarbind() {

}

SNMPVarbind::SNMPVarbind(SNMPObjectIdentifier identifier, SNMPValue value) {

}

SNMPVarbind::~SNMPVarbind() {

}

Error SNMPVarbind::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPVarbind::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPVarbind::length() {
	return 0;
}


SNMPVarbindList::SNMPVarbindList() {

}

SNMPVarbindList::SNMPVarbindList(std::list<SNMPVarbind> varbinds_) {

}

SNMPVarbindList::~SNMPVarbindList() {

}

bool SNMPVarbindList::Add(SNMPVarbind *varbind) {
	return false;
}

Error SNMPVarbindList::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPVarbindList::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPVarbindList::length() {
	return 0;
}


SNMPPDU::SNMPPDU() {}

SNMPPDU::SNMPPDU(SNMPDataType type, SNMPInteger request_id, SNMPInteger error, SNMPInteger error_index, SNMPVarbindList varbinds) {
	set_type(type);
	request_id_ = request_id;
	error_ = error;
	error_index_ = error_index;
	varbinds_ = varbinds;
}

SNMPPDU::~SNMPPDU() {

}

Error SNMPPDU::Marshal(std::list<Byte> &to) {
	to.push_back(type());
	to.push_back(length());
	request_id_.Marshal(to);
	error_.Marshal(to);
	error_index_.Marshal(to);
	varbinds_.Marshal(to);

	return Error::None;
}

Error SNMPPDU::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPPDU::length() {
	return kSNMPHeaderSize + request_id_.length() + error_.length() + error_index_.length() + varbinds_.length();
}


SNMPGetPacket::SNMPGetPacket() {

}

SNMPGetPacket::SNMPGetPacket(SNMPDataType type, SNMPInteger version, SNMPOctetString community_string,
														 SNMPPDU snmppdu) {
	set_type(type);
	version_ = version;
	community_string_ = community_string;
	pdu_ = snmppdu;
}

SNMPGetPacket::~SNMPGetPacket() {

}

Error SNMPGetPacket::Marshal(std::list<Byte> &to) {
	to.clear();

	to.push_back(type()); // type is first
	to.push_back(length()); // length of the packet is second
	version_.Marshal(to); // version is third
	community_string_.Marshal(to); // 4
	pdu_.Marshal(to); // 5

	return Error::None;
}

Error SNMPGetPacket::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPGetPacket::length() {
	return kSNMPHeaderSize + version_.length() + community_string_.length() + pdu_.length();
}
