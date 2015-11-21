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
	cout << "Got packet id: " << packet->pdu().request_id().value() << endl;

	if (packet->pdu().varbinds().binds().begin()->identifier().value().size() > 11) {
		return Error::SNMPNotAnInterface;
	}

	return Error::None;
}

void InterfaceInfoContainer::OutputResults() {

}
