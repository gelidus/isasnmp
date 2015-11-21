//
// Created by Peter on 11/15/2015.
//

#include "SNMPPacket.h"

SNMPInteger::SNMPInteger() {
	set_type(SNMPDataType::Integer);
}

SNMPInteger::SNMPInteger(long long value) {
	value_ = value;

	set_type(SNMPDataType::Integer);
}

SNMPInteger::~SNMPInteger() {}

Error SNMPInteger::Marshal(std::list<Byte> &to) {
	to.push_back(SNMPDataType::Integer);

	// calculate last byte index that should be transfered
	Byte length = static_cast<Byte>(CalculateLastByteIndex(value_) + 1);

	// push the length
	to.push_back(length);

	// push the bytes of the value
	ByteLongLong conv{value_};
	for(int i = length-1; i >= 0; i--) {
		to.push_back(static_cast<Byte>(conv.bytes[i]));
	}

	return Error::None;
}

Error SNMPInteger::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front(); // data type

	// retrieve length
	Byte length = from.front();
	from.pop_front();

	ByteLongLong conv{0};
	for (int i = length -1; i >= 0; i--) {
		conv.bytes[i] = from.front();
		from.pop_front();
	}

	value_ = conv.value;

	return Error::None;
}

Byte SNMPInteger::length() {
	return kSNMPHeaderSize + static_cast<Byte>(CalculateLastByteIndex(value_));
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
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front();

	Byte length = from.front();
	from.pop_front();

	value_ = "";
	for (int i = 0; i < length; i++) {
		value_ += from.front();
		from.pop_front();
	}

	return Error::None;
}

Byte SNMPOctetString::length() {
	return static_cast<Byte>(value_.length());
}


SNMPObjectIdentifier::SNMPObjectIdentifier() {
	set_type(SNMPDataType::ObjectIdentifier);
}

SNMPObjectIdentifier::SNMPObjectIdentifier(std::list<Byte> value) {
	set_type(SNMPDataType::ObjectIdentifier);
	value_ = value;
}

SNMPObjectIdentifier::~SNMPObjectIdentifier() {}

Error SNMPObjectIdentifier::Marshal(std::list<Byte> &to) {
	to.push_back(type());
	to.push_back(length());

	auto it = value_.begin();
	for (int i = 0; i < value_.size(); i++, it++) {
		if (i == 0) {
			Byte first = (*it) * static_cast<Byte>(40);
			Byte second = (*(++it));

			// push the first byte
			to.push_back(first + second);

			i++; // skip the second element
			continue;
		}

		// push the remaining bytes
		to.push_back(*it);
	}

	return Error::None;
}

Error SNMPObjectIdentifier::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front();

	Byte length = from.front();
	from.pop_front();

	Byte first = from.front();
	from.pop_front();

	value_.push_back(first / static_cast<Byte>(40));
	value_.push_back(first % static_cast<Byte>(40));

	// retrieve remaining bytes
	for (int i = 1; i < length; i++) {
		value_.push_back(from.front());
		from.pop_front();
	}

	return Error::None;
}

Byte SNMPObjectIdentifier::length() {
	return value_.size() - 1; // -1 because the first two bytes are encoded as one
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
