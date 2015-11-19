//
// Created by Peter on 11/15/2015.
//

#ifndef ISA_SNMPPACKET_H
#define ISA_SNMPPACKET_H

#include <iostream>
#include <vector>

enum class Error {
		None,
		CannotCreateSocket,
		CannotSendData,
		CannotSendFullData,
		CannotReceiveData,
};

enum class DataType {
		Integer = 0x02,
};

class SNMPPDU {
private:
		std::string name_;
		char type_;
		void *value_;
};

typedef unsigned char Byte;

class SNMPPacket {
public:
		SNMPPacket(Byte version, std::string community, Byte request_id);

		Error Marshal(std::vector<unsigned char> &bytes);

		Error Unmarshal(std::vector<unsigned char> &bytes);

private:
		Byte version_;
		std::string community_;
		char request_type_;
		Byte request_id_;
		Byte error_;
		Byte error_index_;
		Byte non_repeaters_;
		Byte max_repetitions_;
		std::vector<SNMPPDU*> variables_;
public:
		void set_error(Byte error);
		Byte error();

		void set_error_index(Byte index);
		Byte error_index();
};

#endif //ISA_SNMPPACKET_H
