/*
 * NodeId.cpp
 *
 *  Created on: 13.04.2017
 *   Copyright: 2017 by Janitza electronics GmbH
 */

/*
 * Class header
 */
#include "NodeId.hpp"
/*
 * System includes
 */
#include <sstream>
#include <stdexcept>
#include <algorithm>

const NodeId NodeId::Null = NodeId();
const std::string NodeId::InvalidString("?=?");
const UA_NodeId NodeId::InvalidUa = { InvalidNamespace, static_cast<UA_NodeIdType>(0xff), {0} };

NodeId::NodeId()
: m_valid(true),
  m_namespace(NullNamespace),
  m_identifier(Identifier::Numeric),
  m_numeric(NullNumeric),
  m_stringOrByteString(nullptr) {}

NodeId::NodeId(NodeId& other)
: NodeId(const_cast<const NodeId&>(other)) {}

NodeId::NodeId(const NodeId& other)
: m_valid(other.m_valid),
  m_namespace(other.m_namespace),
  m_identifier(other.m_identifier),
  m_numeric(other.m_numeric),
  m_stringOrByteString(other.m_stringOrByteString ? std::make_shared<std::string>(*other.m_stringOrByteString) : nullptr) {}

NodeId::NodeId(NamespaceT namespaceIndex, NumericT numeric)
: m_valid(true),
  m_namespace(namespaceIndex),
  m_identifier(Identifier::Numeric),
  m_numeric(numeric),
  m_stringOrByteString(nullptr) {}

NodeId::NodeId(UA_NodeId& ua)
: NodeId(const_cast<const UA_NodeId*>(&ua)) {}

NodeId::NodeId(const UA_NodeId& ua)
: NodeId(&ua) {}

NodeId::NodeId(UA_NodeId* ua)
: NodeId(const_cast<const UA_NodeId*>(ua)) {}

NodeId::NodeId(const UA_NodeId* ua)
: m_valid(false),
  m_namespace(ua ? ua->namespaceIndex : InvalidNamespace),
  m_identifier(Identifier::Unknown),
  m_numeric(InvalidNumeric),
  m_stringOrByteString(nullptr) {

	if (ua) {
		switch (ua->identifierType) {
		case UA_NODEIDTYPE_NUMERIC:
			m_identifier = Identifier::Numeric;
			m_numeric = ua->identifier.numeric;
			m_valid = true;
			break;
		case UA_NODEIDTYPE_STRING:
			m_identifier = Identifier::String;
			m_stringOrByteString = std::make_shared<std::string>(
					reinterpret_cast<char*>(const_cast<UA_Byte*>(ua->identifier.string.data)),
					ua->identifier.string.length);
			m_valid = true;
			break;
		case UA_NODEIDTYPE_BYTESTRING:
			m_identifier = Identifier::ByteString;
			m_stringOrByteString = std::make_shared<std::string>(
					reinterpret_cast<char*>(const_cast<UA_Byte*>(ua->identifier.byteString.data)),
					ua->identifier.byteString.length);
			m_valid = true;
			break;
		case UA_NODEIDTYPE_GUID:
			/* not implemented here */
		default:
			std::cerr << "Got unknown node ID identifier (type) '" << ua->identifierType << "'" << std::endl;
			break;
		}
	}
}

NodeId& NodeId::operator =(const NodeId& other) {
	if (this == &other) {
		return *this;
	}

	m_valid = other.m_valid;
	m_namespace = other.m_namespace;
	m_identifier = other.m_identifier;
	m_numeric = other.m_numeric;
	m_stringOrByteString = (other.m_stringOrByteString
			? std::make_shared<std::string>(*other.m_stringOrByteString)
			: std::shared_ptr<std::string>(nullptr));
	return *this;
}

NodeId& NodeId::operator =(const UA_NodeId& other) {
	*this = NodeId(other);
	return *this;
}

bool NodeId::operator ==(const NodeId& other) const {
	if (m_namespace != other.m_namespace) {
		return false;
	}
	if (m_identifier != other.m_identifier) {
		return false;
	}
	switch (m_identifier) {
	case Identifier::Numeric:
		return (m_numeric == other.m_numeric);
	case Identifier::String:
		[[fallthrough]];
	case Identifier::ByteString:
		return (*m_stringOrByteString == *(other.m_stringOrByteString));
	default:
		std::cerr << "Tried to compare node IDs w/ unknown identifier!" << std::endl;
		return false;
	}
}

bool NodeId::operator !=(const NodeId& other) const {
	return !(*this == other);
}

bool NodeId::operator <(const NodeId& other) const {
	if (m_namespace < other.m_namespace) {
		return true;
	} else if (m_namespace > other.m_namespace) {
		return false;
	}
	if (m_identifier < other.m_identifier) {
		return true;
	} else if (m_identifier > other.m_identifier) {
		return false;
	}
	switch (m_identifier) {
	case Identifier::Numeric:
		return (m_numeric < other.m_numeric);
	case Identifier::String:
		[[fallthrough]];
	case Identifier::ByteString:
		return (*m_stringOrByteString < *(other.m_stringOrByteString));
	case Identifier::Guid:
	default:
		std::cerr << "Tried to compare node IDs w/ unknown identifier!" << std::endl;
		return false;
	}
}

bool NodeId::operator <=(const NodeId& other) const {
	return (*this == other || *this < other);
}

bool NodeId::operator >(const NodeId& other) const {
	if (m_namespace > other.m_namespace) {
		return true;
	} else if (m_namespace < other.m_namespace) {
		return false;
	}
	if (m_identifier > other.m_identifier) {
		return true;
	} else if (m_identifier < other.m_identifier) {
		return false;
	}
	switch (m_identifier) {
	case Identifier::Numeric:
		return (m_numeric > other.m_numeric);
	case Identifier::String:
		[[fallthrough]];
	case Identifier::ByteString:
		return (*m_stringOrByteString > *(other.m_stringOrByteString));
	case Identifier::Guid:
	default:
		std::cerr << "Tried to compare node IDs w/ unknown identifier!" << std::endl;
		return false;
	}
}

bool NodeId::operator >=(const NodeId& other) const {
	return (*this == other || *this > other);
}

NodeId::operator std::string() const {
	return toString();
}

NodeId::operator const UA_NodeId() const {
	return toUaNodeId();
}

NodeId::operator Const() const {
	return toConst();
}

std::string NodeId::toString() const {
	std::stringstream s2;

	if (m_namespace > 0) {
		s2 << "ns=" << m_namespace << ";";
	}

	switch (m_identifier) {
	case Identifier::Numeric:
		s2 << "i=" << m_numeric;
		break;
	case Identifier::String:
		s2 << "s=" << *m_stringOrByteString;
		break;
	case Identifier::ByteString:
		s2 << "b=" << *m_stringOrByteString;
		break;
	case Identifier::Guid:
	default:
		std::cerr << "Tried to convert to string for node ID w/ unknown identifier!" << std::endl;
		return InvalidString;
	}

    return s2.str();
}

const UA_NodeId NodeId::toUaNodeId() const {
	switch (m_identifier) {
	case Identifier::Numeric:
		return UA_NODEID_NUMERIC(m_namespace, m_numeric);
	case Identifier::String:
		return UA_NODEID_STRING(m_namespace, m_stringOrByteString->data());
	case Identifier::ByteString:
		return UA_NODEID_BYTESTRING(m_namespace, m_stringOrByteString->data());
	case Identifier::Guid:
	default:
		std::cerr << "Tried to convert to UA for node ID w/ unknown identifier!" << std::endl;
		return InvalidUa;
	}
}

NodeId::Const NodeId::toConst() const {
	return Const(*this);
}

bool NodeId::isValid() const {
	return m_valid;
}

bool NodeId::isNull() const {
	if (!m_valid) {
		std::cerr << "Tried to retrieve isNull() for invalid node ID!" << std::endl;
		return false;
	}

	if (m_namespace != 0) {
		return false;
	}
	return isIdentifierNull();
}

bool NodeId::isNamespace0() const {
	if (!m_valid) {
		std::cerr << "Tried to retrieve isNamespace0() for invalid node ID!" << std::endl;
		return false;
	}

	if (m_namespace != 0) {
		return false;
	}
	return !isIdentifierNull();
}

const NodeId::NamespaceT& NodeId::getNamespace() const {
	if (!m_valid) {
		std::cerr << "Tried to retrieve getNamespace() for invalid node ID!" << std::endl;
	}
	return m_namespace;
}

const NodeId::Identifier& NodeId::getIdentifier() const {
	if (!m_valid) {
		std::cerr << "Tried to retrieve getIdentifier() for invalid node ID!" << std::endl;
	}
	return m_identifier;
}

bool NodeId::isIdentifierNull() const {
	switch (m_identifier) {
	case Identifier::Numeric:
		return (m_numeric == 0);
	case Identifier::String:
		[[fallthrough]];
	case Identifier::ByteString:
		return m_stringOrByteString->empty();
	case Identifier::Guid:
	default:
		std::cerr << "Tried to access node ID w/ unknown identifier!" << std::endl;
		return false;
	}
}

const NodeId::NumericT& NodeId::getNumeric() const {
	if (!m_valid) {
		std::cerr << "Tried to retrieve getNumeric() for invalid node ID!" << std::endl;
	}
	return m_numeric;
}

NodeId::Const::Const(NodeId& other)
: Const(const_cast<const NodeId&>(other)) {}

NodeId::Const::Const(const NodeId& other)
: m_valid(other.m_valid),
  m_namespace(other.m_namespace),
  m_identifier(other.m_identifier),
  m_numeric(other.m_numeric) {
	if (m_identifier != Identifier::Numeric) {
		m_valid = false;
	}
}

NodeId::Const& NodeId::Const::operator =(const NodeId::Const& other) {
	if (this == &other) {
		return *this;
	}

	m_valid = other.m_valid;
	m_namespace = other.m_namespace;
	m_identifier = other.m_identifier;
	m_numeric = other.m_numeric;
	return *this;
}

NodeId::Const::operator std::string() const {
	return toString();
}

NodeId::Const::operator const UA_NodeId() const {
	return toUaNodeId();
}

NodeId::Const::operator NodeId() const {
	return NodeId(toNodeId());
}

std::string NodeId::Const::toString() const {
	std::stringstream s2;

	if (m_namespace > 0) {
		s2 << "ns=" << m_namespace << ";";
	}

	switch (m_identifier) {
	case Identifier::Numeric:
		s2 << "i=" << m_numeric;
		break;
	default:
		std::cerr << "Tried to convert to string for const node ID w/ unsupported identifier!" << std::endl;
		return InvalidString;
	}

    return s2.str();
}

const UA_NodeId NodeId::Const::toUaNodeId() const {
	switch (m_identifier) {
	case Identifier::Numeric:
		return UA_NODEID_NUMERIC(m_namespace, m_numeric);
	default:
		std::cerr << "Tried to convert to UA for const node ID w/ unsupported identifier!" << std::endl;
		return InvalidUa;
	}
}

NodeId NodeId::Const::toNodeId() const {
	switch (m_identifier) {
	case Identifier::Numeric:
		return NodeId(m_namespace, m_numeric);
	default:
		std::cerr << "Tried to convert to node ID for const node ID w/ unsupported identifier!" << std::endl;
		NodeId id;
		id.m_valid = false;
		id.m_identifier = Identifier::Unknown;
		id.m_namespace = InvalidNamespace;
		id.m_numeric = InvalidNumeric;
		return id;
	}
}
