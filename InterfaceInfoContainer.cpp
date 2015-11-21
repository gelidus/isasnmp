//
// Created by Peter on 11/21/2015.
//

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
		interfaces_[interface] << "time";
	} else {
		// existing interface
		interfaces_[interface] << ";thing";
	}

	return Error::None;
}

void InterfaceInfoContainer::OutputResults() {
	for (const auto &pair : interfaces_) {
		cout << pair.second.str() << endl;
	}
}
