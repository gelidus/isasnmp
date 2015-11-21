//
// Created by Peter on 11/21/2015.
//

#include "InterfaceInfoContainer.h"

InterfaceInfoContainer::InterfaceInfoContainer() {
	i = 0;
}

InterfaceInfoContainer::~InterfaceInfoContainer() {

}

Error InterfaceInfoContainer::ProcessPacket(SNMPGetPacket *packet) {
	i++;

	if (i > 5) {
		return Error::SNMPNotAnInterface;
	}
	return Error::None;
}
