/**
 * NodeId
 *
 * C++ Wrapper Class für UA_NodeId to handle NodeIds in STL-Containers
 *
 * @file	  NodeId.hpp
 * @namespace 
 * @copyright 2017 by Janitza electronics GmbH
 * @date	  24.08.2017
 * @note      This example misses Node-ID Types GUID and  String/ByteString
 *
 */
#ifndef NODEID_HPP_
#define NODEID_HPP_

/*
 * Local includes
 */
/*
 * System includes
 */
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>
#include <open62541/types.h>


class NodeId final {
public:
	class Const;
	enum class Identifier {
		Unknown,
		Numeric,
		Guid,
		String,
		ByteString
	};

public:
	using NamespaceT = uint16_t;
	using NumericT = uint32_t;

public:
	static constexpr NamespaceT NullNamespace = 0;
	static constexpr NumericT NullNumeric = 0;
	static constexpr NamespaceT InvalidNamespace = 0xffff;
	static constexpr NumericT InvalidNumeric = 0;

	static const NodeId Null;
	static const std::string InvalidString;
	static const UA_NodeId InvalidUa;

public:
	/**
	 * Constructs NULL node ID.
	 */
	NodeId();

	NodeId(NodeId& other);
	NodeId(const NodeId& other);

	NodeId(NamespaceT namespaceIndex, NumericT numeric);

	explicit NodeId(UA_NodeId& ua);
	explicit NodeId(const UA_NodeId& ua);

	explicit NodeId(UA_NodeId* ua);
	explicit NodeId(const UA_NodeId* ua);

	NodeId& operator =(const NodeId& other);
	NodeId& operator =(const UA_NodeId& other);

	bool operator ==(const NodeId& other) const;
	bool operator !=(const NodeId& other) const;
	bool operator <(const NodeId& other) const;
	bool operator <=(const NodeId& other) const;
	bool operator >(const NodeId& other) const;
	bool operator >=(const NodeId& other) const;

	/**
	 * Converts to string.
	 * Returns InvalidString if is invalid.
	 */
	operator std::string() const;
	/**
	 * Converts to a NOT! pointer-free and NOT! thread-safe UA_NodeId!
	 * Returns InvalidUa if is invalid.
	 */
	operator const UA_NodeId() const;
	/**
	 * Converts to a NodeId::Const!
	 * @return NodeId::Const, before valid NodeId can become invalid since maybe identifier not supported any more!
	 */
	operator Const() const;
	/**
	 * Converts to string.
	 * @return Stringified or InvalidString if is invalid.
	 */
	std::string toString() const;
	/**
	 * Converts to a NOT! pointer-free and NOT! thread-safe UA_NodeId!
	 * @return UA NodeId or InvalidUa if is invalid.
	 */
	const UA_NodeId toUaNodeId() const;
	/**
	 * Converts to a NodeId::Const!
	 * @return NodeId::Const, before valid NodeId can become invalid since maybe identifier not supported any more!
	 */
	Const toConst() const;

	bool isValid() const;
	bool isNull() const;

	bool isNamespace0() const;
	const NamespaceT& getNamespace() const;

	const Identifier& getIdentifier() const;
	const NumericT& getNumeric() const;

private:
	bool m_valid;

	NamespaceT m_namespace;

	Identifier m_identifier;
	NumericT m_numeric;
	std::shared_ptr<std::string> m_stringOrByteString;

	bool isIdentifierNull() const;

public:
	class Const final {
	public:
		/**
		 * Constructs NULL node ID.
		 */
		constexpr Const()
		: m_valid(true),
		  m_namespace(NullNamespace),
		  m_identifier(Identifier::Numeric),
		  m_numeric(NullNumeric) {}

		constexpr explicit Const(Const& other)
		: Const(const_cast<const Const&>(other)) {}

		constexpr explicit Const(const Const& other)
		: m_valid(other.m_valid),
		  m_namespace(other.m_namespace),
		  m_identifier(other.m_identifier),
		  m_numeric(other.m_numeric) {}

		constexpr Const(NamespaceT namespaceIndex, NumericT numeric)
		: m_valid(true),
		  m_namespace(namespaceIndex),
		  m_identifier(Identifier::Numeric),
		  m_numeric(numeric) {}

		explicit Const(NodeId& other);
		explicit Const(const NodeId& other);

		Const& operator =(const Const& other);

		constexpr bool operator ==(const Const& other) const {
			if (m_namespace != other.m_namespace) {
				return false;
			}
			if (m_identifier != other.m_identifier) {
				return false;
			}
			switch (m_identifier) {
			case Identifier::Numeric:
				return (m_numeric == other.m_numeric);
			default:
				std::cerr << "Tried to compare const node IDs w/ unsupported identifier!" << std::endl;
				return false;
			}
		}

		constexpr bool operator !=(const Const& other) const {
			return !(*this == other);
		}

		constexpr bool operator <(const Const& other) const {
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
			default:
				std::cerr << "Tried to compare const node IDs w/ unsupported identifier!" << std::endl;
				return false;
			}
		}

		constexpr bool operator <=(const Const& other) const {
			return (*this == other || *this < other);
		}

		constexpr bool operator >(const Const& other) const {
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
			default:
				std::cerr << "Tried to compare const node IDs w/ unsupported identifier!" << std::endl;
				return false;
			}
		}

		constexpr bool operator >=(const Const& other) const {
			return (*this == other || *this > other);
		}

		/**
		 * Converts to string.
		 * Returns InvalidString if is invalid.
		 */
		operator std::string() const;
		/**
		 * Converts to a NOT! pointer-free and NOT! thread-safe UA_NodeId!
		 * Returns InvalidUa if is invalid.
		 */
		operator const UA_NodeId() const;
		/**
		 * Converts to a NodeId.
		 * Returns Invalid if is invalid.
		 */
		operator NodeId() const;
		/**
		 * Converts to string.
		 * @return Stringified or InvalidString if is invalid.
		 */
		std::string toString() const;
		/**
		 * Converts to a NOT! pointer-free and NOT! thread-safe UA_NodeId!
		 * @return UA NodeId or InvalidUa if is invalid.
		 */
		const UA_NodeId toUaNodeId() const;
		/**
		 * Converts to a NodeId.
		 * @return NodeId or Invalid if is invalid.
		 */
		NodeId toNodeId() const;

		constexpr bool isValid() const {
			return m_valid;
		}

		constexpr bool isNull() const {
			if (!m_valid) {
				std::cerr << "Tried to retrieve isNull() for invalid const node ID!" << std::endl;
				return false;
			}

			if (m_namespace != 0) {
				return false;
			}
			return isIdentifierNull();
		}

		constexpr bool isNamespace0() const {
			if (!m_valid) {
				std::cerr << "Tried to retrieve isNamespace0() for invalid const node ID!" << std::endl;
				return false;
			}

			if (m_namespace != 0) {
				return false;
			}
			return !isIdentifierNull();
		}

		constexpr const NamespaceT& getNamespace() const {
			if (!m_valid) {
				std::cerr << "Tried to retrieve getNamespace() for invalid const node ID!" << std::endl;
			}
			return m_namespace;
		}

		constexpr const Identifier& getIdentifier() const {
			if (!m_valid) {
				std::cerr << "Tried to retrieve getIdentifier() for invalid const node ID!" << std::endl;
			}
			return m_identifier;
		}

		constexpr const NumericT& getNumeric() const {
			if (!m_valid) {
				std::cerr << "Tried to retrieve getNumeric() for invalid node ID!" << std::endl;
			}
			return m_numeric;
		}

	private:
		bool m_valid;

		NamespaceT m_namespace;

		Identifier m_identifier;
		NumericT m_numeric;

		constexpr bool isIdentifierNull() const {
			switch (m_identifier) {
			case Identifier::Numeric:
				return (m_numeric == 0);
			default:
				std::cerr << "Tried to access const node ID w/ unsupported identifier!" << std::endl;
				return false;
			}
		}
	};
};
	
#endif /* NODEID_HPP_ */
