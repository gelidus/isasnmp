//
// Created by Peter on 11/15/2015.
//

#ifndef ISA_SNMPPACKET_H
#define ISA_SNMPPACKET_H

#include <iostream>
#include <vector>

class SNMPPacket {
private:
		unsigned char version_;
		std::string community_;
		char request_type_;
		unsigned char request_id_;
		unsigned char error_;
		unsigned char error_index_;
		unsigned char non_repeaters_;
		unsigned char max_repetitions_;
		std::vector<SNMPPDU> variables_;
};

class SNMPPDU {
private:
		std::string name_;
		int type_;
		void *value_;
};

#endif //ISA_SNMPPACKET_H
