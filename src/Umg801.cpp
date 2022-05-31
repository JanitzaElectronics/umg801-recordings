/*
 * Umg801.cpp
 *
 *  Created on: 20.04.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#include "Umg801.hpp"

#include <iostream>

Umg801::Umg801() : OpcuaClient(),
	m_lookupInfo() {
}

Umg801::~Umg801() {
}

std::list<Recording> Umg801::getRecordings() {
	std::list<Recording> recordings;
	NodeId recordingsNodeId = browsePathToNodeId(NodeId(0, UA_NS0ID_OBJECTSFOLDER),
			{{2,"Device", UA_NS0ID_ORGANIZES},
			 {2,"Recordings", UA_NS0ID_HASCOMPONENT}});
	if(recordingsNodeId.isNull()) {
		std::cerr << "Node-ID of Node /Objects/Device/Recordings not found!" << std::endl;
	} else {
		for (const auto& n : getHierarichalNodes(recordingsNodeId)) {
			if(OpcUaUtil::isPrefix(n.first, "Recording")) {
				auto rec = Recording(*this, OpcUaUtil::getIdSuffix(n.first), n.second);
				if(rec.getNodeIds() == UA_STATUSCODE_GOOD) {
					recordings.emplace_back(rec);
				} else {
					std::cerr << "Skipping Recording" << rec.getId() << std::endl;
				}
			}
		}
	}
	return recordings;
}

std::optional<std::string> Umg801::lookup(const NodeId& id, const UA_Tag& tag) {
	UA_StatusCode retval = UA_STATUSCODE_GOOD;
	auto ret = m_lookupInfo.find(id);
	if(ret == m_lookupInfo.end()) {
		// NodeId not found in cache, to a new request!
		const NodeId deviceId = browsePathToNodeId(NodeId(0, UA_NS0ID_OBJECTSFOLDER), {{2,"Device", UA_NS0ID_ORGANIZES}});
		if(deviceId.isNull()) {
			std::cerr << "Node Objects/Device could not be found!" << std::endl;
			return std::nullopt;
		}
		const NodeId lookupId = browsePathToNodeId(deviceId, {{2,"Lookup", UA_NS0ID_HASCOMPONENT}});
		if(lookupId.isNull()) {
			std::cerr << "Method-Node Objects/Device/Lookup could not be found!" << std::endl;
			return std::nullopt;
		}
		UA_Variant input;
		UA_Variant_init(&input);
		UA_Variant_setScalarCopy(&input, &tag, &UA_TYPES[UA_TYPES_INT32]);
		size_t outputSize;
		UA_Variant *output;
		retval = clientCall(deviceId, lookupId, 1, &input, &outputSize, &output);
		if(retval != UA_STATUSCODE_GOOD) {
			std::cerr << "Method call to Lookup was unsuccessful: " << UA_StatusCode_name(retval) << std::endl;
		} else {
			UA_ExtensionObject* e = (UA_ExtensionObject*) output[0].data;
			for(size_t i=0; i < output[0].arrayLength; i++) {
				UA_LookupInfo* info = (UA_LookupInfo*) e[i].content.decoded.data;
				m_lookupInfo.emplace(NodeId(info->nodeId), OpcUaUtil::toString(info->browsePath));
				info++;
			}
			UA_Array_delete(output, outputSize, &UA_TYPES[UA_TYPES_VARIANT]);
		}
		UA_Variant_clear(&input);
	}
	ret = m_lookupInfo.find(id);
	if(ret != m_lookupInfo.end()) {
		return ret->second;
	}
	return std::nullopt;
}
