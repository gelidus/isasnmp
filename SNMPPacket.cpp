//
// Created by Peter on 11/15/2015.
//

#include "SNMPPacket.h"

SNMPInteger::SNMPInteger() {}

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
	SNMPDataType t = static_cast<SNMPDataType>(from.front());
	from.pop_front(); // data type

	int length = from.front();
	from.pop_front(); // length

	value_ = from.front(); // retrieve value from of the integer
	from.pop_front();

	return Error::None;
}

Byte SNMPInteger::length() {
	return sizeof(Byte);
}


SNMPOctetString::SNMPOctetString() {

}

SNMPOctetString::SNMPOctetString(std::string value) {

}

SNMPOctetString::~SNMPOctetString() {

}

Error SNMPOctetString::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPOctetString::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPOctetString::length() {
	return 0;
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

SNMPPDU::SNMPPDU() {

}

SNMPPDU::SNMPPDU(SNMPInteger request_id, SNMPInteger error, SNMPInteger error_index, SNMPVarbindList varbins) {

}

SNMPPDU::~SNMPPDU() {

}

Error SNMPPDU::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPPDU::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPPDU::length() {
	return 0;
}

SNMPGetPacket::SNMPGetPacket() {

}

SNMPGetPacket::SNMPGetPacket(SNMPDataType type, SNMPInteger version, SNMPOctetString community_string,
														 SNMPPDU snmppdu) {

}

SNMPGetPacket::~SNMPGetPacket() {

}

Error SNMPGetPacket::Marshal(std::list<Byte> &to) {
	return Error::None;
}

Error SNMPGetPacket::Unmarshal(std::list<Byte> &from) {
	return Error::None;
}

Byte SNMPGetPacket::length() {
	return 0;
}
