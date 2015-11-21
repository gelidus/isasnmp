//
// Created by Peter on 11/21/2015.
//

#include "InterfaceInfoContainer.h"

using namespace std;

InterfaceInfoContainer::InterfaceInfoContainer() {
	i = 0;
}

InterfaceInfoContainer::~InterfaceInfoContainer() {

}

Error InterfaceInfoContainer::ProcessPacket(SNMPGetPacket *packet) {
	cout << "Got packet id: " << packet->pdu().request_id().value() << endl;
	i++;

	if (i > 5) {
		return Error::SNMPNotAnInterface;
	}
	return Error::None;
}
