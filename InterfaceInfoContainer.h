//
// Created by Peter on 11/21/2015.
//

#ifndef ISA_INTERFACEINFOCONTAINER_H
#define ISA_INTERFACEINFOCONTAINER_H

#include "SNMPPacket.h"
#include <map>
#include <sstream>

const int kSNMPEntryLength = 9;
const int kSNMPEntry[kSNMPEntryLength] = {1,3,6,1,2,1,2,2,1};
const int kSNMPEntryInterfaceIndex = 10;

class InterfaceInfoContainer {
private:
		std::map<Byte, std::stringstream> interfaces_;

public:
		InterfaceInfoContainer();
		virtual ~InterfaceInfoContainer();

		Error ProcessPacket(SNMPGetPacket *packet);

		void OutputResults();
};


#endif //ISA_INTERFACEINFOCONTAINER_H
