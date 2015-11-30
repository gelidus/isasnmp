//
// Created by Peter on 11/21/2015.
//

#include <time.h>
#include <chrono>
#include <iomanip>
#include "InterfaceInfoContainer.h"

using namespace std;

InterfaceInfoContainer::InterfaceInfoContainer() {
}

InterfaceInfoContainer::~InterfaceInfoContainer() {

}

void InterfaceInfoContainer::Reset() {
	interfaces_.clear(); // clear the map
	packet_count_.clear();
}

Error InterfaceInfoContainer::ProcessPacket(SNMPGetPacket *packet) {
	// checking for valid oid
	list<Byte> oid = packet->pdu().varbinds().binds().begin()->identifier().value();

	// if the identifier is not the actual table size (11), just return, we are
	// out of interface objects
	if (oid.size() != 11) {
		return Error::SNMPNotAnInterface;
	}

	// check if the given oid is entry or not
	auto it = oid.begin();
	for (int i = 0; i < kSNMPEntryLength; i++, it++) {
		if (kSNMPEntry[i] != (*it)) {
			return Error::SNMPNotAnInterface;
		}
	}

	// retrieve the interface from the oid
	Byte interface = oid.back();

	// check if this is the start of the time mark
	if (interfaces_.size() == 0) {
		time_mark_ = "";
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
		);

		char buffer[100];

		time_t raw_time;
		time(&raw_time);

		struct tm* time_info;
		time_info = localtime(&raw_time);

		strftime(buffer, 100, "%Y-%m-%d %H:%M:%S.", time_info);
		char millis[3];
		sprintf(millis, "%03d", (int)(ms.count() % 1000));

		std::stringstream ss;
		ss << buffer << millis;
		time_mark_ = ss.str();
	}

	SNMPValue *entity = packet->pdu().varbinds().binds().begin()->value();
	interfaces_[interface] << ";";
	switch (entity->type()) {
		case SNMPDataType::Integer: {
			SNMPInteger *integer = (SNMPInteger *) entity->value();
			interfaces_[interface] << integer->value();
			break;
		}
		case SNMPDataType::OctetString: {
			SNMPOctetString *octetString = reinterpret_cast<SNMPOctetString *>(entity->value());
			// no need to do something with empty string
			if (octetString->value().length() <= 0) {
				break;
			}

			// 6th value of object is MAC
			if (packet_count_[interface] == 5) {
				interfaces_[interface] << std::setfill('0') << std::setw(2) << hex << (int)(Byte)octetString->value()[0];
				for (unsigned int i = 1; i < octetString->value().length(); i++) {
					interfaces_[interface] << ":" << std::setfill('0') << std::setw(2) << hex << (int)(Byte)octetString->value()[i];
				}
			} else {
				interfaces_[interface] << octetString->value();
			}
			break;
		}
		case SNMPDataType::ObjectIdentifier: {
			SNMPObjectIdentifier *objectIdentifier = reinterpret_cast<SNMPObjectIdentifier*>(entity->value());
			for (auto it = objectIdentifier->value().begin(); it != objectIdentifier->value().end(); it++) {
				if (it != objectIdentifier->value().begin()) {
					interfaces_[interface] << ":";
				}
				interfaces_[interface] << hex << (int) (*it);
			}

			interfaces_[interface] << dec; // reset to decimal
			break;
		}
		case SNMPDataType::Null:
			interfaces_[interface] << "0";
			break;
		default:
			return Error::SNMPValueUnrecognized;
	}

	// check for packet count progress
	if (packet_count_.find(interface) == packet_count_.end()) {
		packet_count_[interface] = 0;
	}
	packet_count_[interface]++;

	return Error::None;
}

void InterfaceInfoContainer::OutputResults() {
	cout << time_mark_;
	for (const auto &pair : interfaces_) {
		cout << pair.second.str();
	}
	cout << endl;
}
