/*
 * OpcuaClient.hpp
 *
 *  Created on: 20.04.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#ifndef OPCUACLIENT_HPP_
#define OPCUACLIENT_HPP_
#include "NodeId.hpp"
#include "OpcUaUtil.hpp"

#include <string>
#include <map>
#include <vector>
#include <open62541/open62541.h>

struct PathElement {
	uint16_t ns;
	std::string browseName;
	uint32_t referenceId;
};

struct CmpPathElement {
	bool operator()(const std::vector<PathElement> &lhs, const std::vector<PathElement> &rhs) const {
		if(lhs.size() != rhs.size())
			return (lhs.size() < rhs.size());
		for(size_t i=0; i<lhs.size(); i++) {
			if(lhs[i].ns != rhs[i].ns) return (lhs[i].ns < rhs[i].ns);
			else if(lhs[i].browseName != rhs[i].browseName) return (lhs[i].browseName < rhs[i].browseName);
			else if(lhs[i].referenceId != rhs[i].referenceId) return (lhs[i].referenceId < rhs[i].referenceId);
		}
		return false;
	}
};

class OpcuaClient {
public:

	OpcuaClient();
	virtual ~OpcuaClient();
	OpcuaClient(const OpcuaClient&) = delete; // non construction-copyable
	OpcuaClient& operator=(const OpcuaClient&) = delete; // non copyable

	bool connect(const std::string& url);
	UA_StatusCode clientCall(const NodeId objectId, const NodeId methodId,
			size_t inputSize, const UA_Variant *input, size_t *outputSize, UA_Variant **output) const;

	NodeId browsePathToNodeId(const NodeId& startingNode, const std::vector<PathElement>& p);

	std::map<std::string, NodeId> getHierarichalNodes(const NodeId& nodeId) const;

	template<typename T>
	const T getVariantValue(const NodeId& nodeId) const;
	template<typename T>
	std::vector<T> getVariantArrayValue(const NodeId& nodeId) const;

private:
	UA_Client* m_client;
	std::map<std::vector<PathElement>,NodeId, CmpPathElement> m_nodeIdCache;
};

template<typename T>
	const T OpcuaClient::getVariantValue(const NodeId& nodeId) const {
	T ret;
	UA_Variant *val = UA_Variant_new();
	if(UA_Client_readValueAttribute(m_client, nodeId, val) == UA_STATUSCODE_GOOD) {
		ret = *(T*)val->data;
	}
	UA_Variant_delete(val);
	return ret;
}

template<typename T>
	std::vector<T> OpcuaClient::getVariantArrayValue(const NodeId& nodeId) const {
	std::vector<T> ret;
	UA_Variant *val = UA_Variant_new();
	if(UA_Client_readValueAttribute(m_client, nodeId, val) == UA_STATUSCODE_GOOD) {
		if(!UA_Variant_isScalar(val)) {
			if(val->type->typeIndex == UA_TYPES_EXTENSIONOBJECT) {
				UA_ExtensionObject* e = (UA_ExtensionObject*)val->data;
				for(size_t i=0; i < val->arrayLength; i++) {
					ret.emplace_back(*(T*)e[i].content.decoded.data);
				}
			} else {
				ret.assign((T*)val->data, ((T*)val->data) + val->arrayLength);
			}
		}
	}
	UA_Variant_delete(val);
	return ret;
}

#endif /* OPCUACLIENT_HPP_ */
