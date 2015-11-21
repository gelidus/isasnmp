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
	return kSNMPHeaderSize + static_cast<Byte>(CalculateLastByteIndex(value_) + 1);
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
	to.push_back(static_cast<Byte>(value_.length()));

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
	return kSNMPHeaderSize + static_cast<Byte>(value_.length());
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
	to.push_back(static_cast<Byte>(value_.size() - 1));

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
	return kSNMPHeaderSize + static_cast<Byte>(value_.size() - 1); // -1 because the first two bytes are encoded as one
}


SNMPValue::SNMPValue() {
	set_type(SNMPDataType::Null);
	value_ = nullptr;
}

SNMPValue::SNMPValue(SNMPDataType type, SNMPEntity *value) {
	set_type(type);
	value_ = value;
}

SNMPValue::~SNMPValue() {
	if (value_ != nullptr) {
		delete value_;
	}
}

Error SNMPValue::Marshal(std::list<Byte> &to) {
	// TODO: SNMPValue marshalling should be able to
	// marhsall all the stuff, but we dont need it now
	to.push_back(type());
	to.push_back(0);

	return Error::None;
}

Error SNMPValue::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));

	switch(type()) {
		case SNMPDataType::Integer:
		case SNMPDataType::Counter32:
		case SNMPDataType::Gauge32:
		case SNMPDataType::TimeTicks:
		case SNMPDataType::Counter64: {
			value_ = new SNMPInteger{};
		}
		case SNMPDataType::OctetString: {
			value_ = new SNMPOctetString{};
		}
		case SNMPDataType::ObjectIdentifier: {
			value_ = new SNMPObjectIdentifier{};
		}
		case SNMPDataType::Null: {
			// pop both, type and length, as nothing can
			// process them (special type of variable)
			from.pop_front();
			from.pop_front();
			value_ = nullptr;
		}
		default:
			return Error::SNMPValueUnrecognized;
	}

	if (value_ != nullptr) {
		value_->Unmarshal(from);
	}

	return Error::None;
}

Byte SNMPValue::length() {
	if (type() == SNMPDataType::Null) {
		return kSNMPHeaderSize;
	}
	return kSNMPHeaderSize + value_->length();
}


SNMPVarbind::SNMPVarbind() {
	set_type(SNMPDataType::Sequence);
}

SNMPVarbind::SNMPVarbind(SNMPObjectIdentifier identifier, SNMPValue value) {
	set_type(SNMPDataType::Sequence);
	identifier_ = identifier;
	value_ = value;
}

SNMPVarbind::~SNMPVarbind() {

}

Error SNMPVarbind::Marshal(std::list<Byte> &to) {
	to.push_back(type());
	to.push_back(identifier_.length() + value_.length());
	identifier_.Marshal(to);
	value_.Marshal(to);

	return Error::None;
}

Error SNMPVarbind::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front();

	Byte len = from.front();
	from.pop_front();

	identifier_.Unmarshal(from);
	value_.Unmarshal(from);

	return Error::None;
}

Byte SNMPVarbind::length() {
	return kSNMPHeaderSize + identifier_.length() + value_.length();
}


SNMPVarbindList::SNMPVarbindList() {
	set_type(SNMPDataType::Sequence);
}

SNMPVarbindList::SNMPVarbindList(std::list<SNMPVarbind> varbinds) {
	set_type(SNMPDataType::Sequence);
	varbinds_ = varbinds;
}

SNMPVarbindList::~SNMPVarbindList() {

}

bool SNMPVarbindList::Add(SNMPVarbind varbind) {
	varbinds_.push_back(varbind);

	return true;
}

Error SNMPVarbindList::Marshal(std::list<Byte> &to) {
	to.push_back(type());

	// get the length of the elements
	int len = 0;
	for (auto it = varbinds_.begin(); it != varbinds_.end(); it++) {
		len += it->length();
	}
	to.push_back(len);

	for (auto it = varbinds_.begin(); it != varbinds_.end(); it++) {
		it->Marshal(to);
	}

	return Error::None;
}

Error SNMPVarbindList::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front();

	Byte length = from.front();
	from.pop_front();

	// unmarshal bind
	SNMPVarbind bind{};
	bind.Unmarshal(from);

	Add(bind);

	return Error::None;
}

Byte SNMPVarbindList::length() {
	Byte length = 0;
	for (auto it = varbinds_.begin(); it != varbinds_.end(); it++) {
		length += it->length();
	}

	return kSNMPHeaderSize + length;
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
	to.push_back(request_id_.length() + error_.length() + error_index_.length() + varbinds_.length());
	request_id_.Marshal(to);
	error_.Marshal(to);
	error_index_.Marshal(to);
	varbinds_.Marshal(to);

	return Error::None;
}

Error SNMPPDU::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front();

	Byte len = from.front();
	from.pop_front();

	request_id_.Unmarshal(from);
	error_.Unmarshal(from);
	error_index_.Unmarshal(from);
	varbinds_.Unmarshal(from);

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
	to.push_back(version_.length() + community_string_.length() + pdu_.length()); // length of the packet is second
	version_.Marshal(to); // version is third
	community_string_.Marshal(to); // 4
	pdu_.Marshal(to); // 5

	return Error::None;
}

Error SNMPGetPacket::Unmarshal(std::list<Byte> &from) {
	set_type(static_cast<SNMPDataType>(from.front()));
	from.pop_front();

	Byte length = from.front();
	from.pop_front();

	Error err{};

	if ((err = version_.Unmarshal(from)) != Error::None) {
		return err;
	}
	if ((err = community_string_.Unmarshal(from)) != Error::None) {
		return err;
	}
	if ((err = pdu_.Unmarshal(from)) != Error::None) {
		return err;
	}

	return Error::None;
}

Byte SNMPGetPacket::length() {
	return kSNMPHeaderSize + version_.length() + community_string_.length() + pdu_.length();
}
