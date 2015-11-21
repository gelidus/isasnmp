//
// Created by Peter on 11/15/2015.
//

#ifndef ISA_SNMPPACKET_H
#define ISA_SNMPPACKET_H

#include <iostream>
#include <list>
#include <deque>
#include "types.h"

// SNMPEntity is semi-abstract class that provides interface
// which should be implemented in every SNMPEntity class.
// This will provide the tool to build packets directly from
// the base entity (marhsalling) and building base entity
// from the data (unmarshalling).
class SNMPEntity {
		SNMPDataType type_;

protected:
		// set_type will set the type of the current node
		// This is needed in case, node type must be determined
		// from the base SNMPEntity class
		void set_type(SNMPDataType type) { type_ = type; }

public:
		// type will return the type of the value of current snmp
		// entity. This should correspond to T type.
		SNMPDataType type() { return type_; }

		// length returns total length of the T value. This method
		// must be overriden for every typed class
		virtual Byte length() = 0;

		// Marshal serializes the structure into the given list.
		// The input list will now be cleared, as this is recursive
		// builder. Marhshal should call other marshals in this structure
		// to correctly continue in the building
		virtual Error Marshal(std::list<Byte> &to) = 0;

		// Unmarshal works with the input byte list, trying it to
		// deserialize into the current structure. If the data
		// is corrupted, this function returns an approriate error
		virtual Error Unmarshal(std::list<Byte> &from) = 0;
};

// SNMPInteger encapsulates logic for the integer
// type block. This will handle the parse of the integer
// variable in the packet
class SNMPInteger : public SNMPEntity {
private:
		long long value_;

public:
		SNMPInteger();
		SNMPInteger(long long value);
		virtual ~SNMPInteger();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPOctetString encapsualtes the logic for the
// string object. This allows system to serialize and
// deserialize the octet string blocks
class SNMPOctetString : public SNMPEntity {
private:
		std::string value_;

public:
		SNMPOctetString();
		SNMPOctetString(std::string value);
		virtual ~SNMPOctetString();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPObjectIdentifier implements the SNMPEntity interface
// for the ObjectIdentifier block of the SNMP packet.
class SNMPObjectIdentifier : public SNMPEntity {
private:
		std::list<Byte> value_;

public:
		SNMPObjectIdentifier();
		SNMPObjectIdentifier(std::list<Byte> value);
		virtual ~SNMPObjectIdentifier();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPValue represents any type. The type will be dynamically
// resolved during the runtime.
class SNMPValue : public SNMPEntity {
public:
		SNMPEntity *value_;

public:
		SNMPValue();
		SNMPValue(SNMPDataType type, SNMPEntity *value);
		virtual ~SNMPValue();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPVarbind is interface for the Varbind type of the
// snmp block. This encapsulates the identifier and the value
// of the object.
class SNMPVarbind : public SNMPEntity {
private:
		SNMPObjectIdentifier identifier_;
		SNMPValue value_;

public:
		SNMPVarbind();
		SNMPVarbind(SNMPObjectIdentifier identifier, SNMPValue value);
		virtual ~SNMPVarbind();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPVarbindList encapsulates the varbins in the
// VarbindList block of the snmp packet.
class SNMPVarbindList : public SNMPEntity {
private:
		std::list<SNMPVarbind> varbinds_;

public:
		SNMPVarbindList();
		SNMPVarbindList(std::list<SNMPVarbind> varbinds_);
		virtual ~SNMPVarbindList();

		bool Add(SNMPVarbind varbind);

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPPDUEntity implements the entity interface which
// handles the PDU block of the request.
class SNMPPDU : public SNMPEntity {
private:
		SNMPInteger request_id_;
		SNMPInteger error_;
		SNMPInteger error_index_;
		SNMPVarbindList varbinds_;

public:
		SNMPPDU();
		SNMPPDU(SNMPDataType type, SNMPInteger request_id, SNMPInteger error, SNMPInteger error_index, SNMPVarbindList varbinds);
		virtual ~SNMPPDU();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

// SNMPGetPacket implements interface for the 'get' snmp
// packets. It automatically serializes and deserializes
// the structure and it's sub-structures.
class SNMPGetPacket : public SNMPEntity {
private:
		SNMPInteger version_;
		SNMPOctetString community_string_;
		SNMPPDU pdu_;

public:
		SNMPGetPacket();
		SNMPGetPacket(SNMPDataType type, SNMPInteger version, SNMPOctetString community_string, SNMPPDU);
		virtual ~SNMPGetPacket();

		virtual Error Marshal(std::list<Byte> &to) override;
		virtual Error Unmarshal(std::list<Byte> &from) override;

		virtual Byte length();
};

#endif //ISA_SNMPPACKET_H
