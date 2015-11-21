//
// Created by Peter on 11/21/2015.
//

#include <ctime>
#include <iomanip>
#include <chrono>
#include "InterfaceInfoContainer.h"

using namespace std;

InterfaceInfoContainer::InterfaceInfoContainer() {
}

InterfaceInfoContainer::~InterfaceInfoContainer() {

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

	// check if the interface already exists
	if (interfaces_.count(interface) == 0) {
		// non existing interface
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
		);

		std::time_t t = std::time(nullptr);
		interfaces_[interface] << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S.");
		interfaces_[interface] << (ms.count() % 1000);
	} else {
		// existing interface
		SNMPEntity *entity = packet->pdu().varbinds().binds().begin()->value().value();
		interfaces_[interface] << ";";
		switch (entity->type()) {
			case SNMPDataType::Integer: {
				SNMPInteger *integer = (SNMPInteger *) entity;
				interfaces_[interface] << integer->value();
				break;
			}
			case SNMPDataType::OctetString: {
				SNMPOctetString *octetString = (SNMPOctetString *) entity;
				interfaces_[interface] << octetString->value();
				break;
			}
			case SNMPDataType::ObjectIdentifier: {
				SNMPObjectIdentifier *objectIdentifier = (SNMPObjectIdentifier *) entity;
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
	}

	return Error::None;
}

void InterfaceInfoContainer::OutputResults() {
	for (const auto &pair : interfaces_) {
		cout << pair.second.str() << endl;
	}
}
