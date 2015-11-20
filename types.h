//
// Created by Peter on 11/19/2015.
//

#ifndef ISA_TYPES_H
#define ISA_TYPES_H

typedef unsigned char Byte;

enum class Error {
		None,
		CannotCreateSocket,
		CannotSendData,
		CannotSendFullData,
		CannotReceiveData,
};

enum SNMPDataType : Byte {
		Integer = 0x02,
		OctetString = 0x04,
		Sequence = 0x30,

		GetNextRequest = 0xa1,
		GetResponse = 0xa2
};

// byte conversion from numbers
union ByteUInt {
	unsigned int value;
	char bytes[4];
};

union ByteUShort {
	unsigned short value;
	char bytes[2];
};

#endif //ISA_TYPES_H
