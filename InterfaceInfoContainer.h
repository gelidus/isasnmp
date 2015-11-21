//
// Created by Peter on 11/21/2015.
//

#ifndef ISA_INTERFACEINFOCONTAINER_H
#define ISA_INTERFACEINFOCONTAINER_H

#include "SNMPPacket.h"

class InterfaceInfoContainer {
		int i;

public:
		InterfaceInfoContainer();
		virtual ~InterfaceInfoContainer();

		Error ProcessPacket(SNMPGetPacket *packet);
};


#endif //ISA_INTERFACEINFOCONTAINER_H
