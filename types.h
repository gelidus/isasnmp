//
// Created by Peter on 11/19/2015.
//

#ifndef ISA_TYPES_H
#define ISA_TYPES_H

typedef unsigned char Byte;

const Byte kSNMPHeaderSize = 2;

enum class Error {
		None,
		CannotTranslateAddress,
		CannotCreateSocket,
		CannotSendData,
		CannotSendFullData,
		CannotReceiveData,

		SNMPValueUnrecognized,
};

enum SNMPDataType : unsigned char {
		Integer = 0x02,
		OctetString = 0x04,
		Null = 0x05,
		ObjectIdentifier = 0x06,
		Sequence = 0x30,
		IpAddress = 0x40,
		Counter32 = 0x41,
		Gauge32 = 0x42,
		TimeTicks = 0x43,
		Counter64 = 0x46,

		GetNextRequest = 0xa1,
		GetResponse = 0xa2,
		Trap = 0xa4,
};

union ByteLongLong {
		long long value;
		char bytes[8];
};

int CalculateLastByteIndex(long long int);

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
