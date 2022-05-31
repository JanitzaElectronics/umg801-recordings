/*
 * OpcuaClient.cpp
 *
 *  Created on: 20.04.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#include "OpcuaClient.hpp"
#include "CustomUaTypes.hpp"

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

OpcuaClient::OpcuaClient() :
	m_client(nullptr)
	,m_customTypes({
			UA_LookupInfoType,
			UA_RecordingValueInfoType,
			UA_RecordingTypeInfoType,
			UA_RecordingValueType,
			UA_ReferenceStatusType,
			UA_RecordingDataTypeType,
			UA_RecordingPointType,
			UA_RecordingExtremalsType
			})
	,m_customDataTypes({nullptr, m_customTypes.size(), m_customTypes.data()})
	,m_nodeIdCache()
	{

}

OpcuaClient::~OpcuaClient() {
	if(m_client) {
		UA_Client_delete(m_client); /* Disconnects the client internally */
		m_client = nullptr;
	}
}

bool OpcuaClient::connect(const std::string& url) {
	UA_String securityPolicyUri = UA_STRING_NULL;
	UA_MessageSecurityMode securityMode = UA_MESSAGESECURITYMODE_INVALID; /* allow everything */
	m_client = UA_Client_new();
	UA_ClientConfig *cc = UA_Client_getConfig(m_client);
	UA_ClientConfig_setDefault(cc);
	cc->customDataTypes = &m_customDataTypes;
	cc->securityMode = securityMode;
	cc->securityPolicyUri = securityPolicyUri;

	UA_StatusCode retval = UA_Client_connect(m_client, url.c_str());
	if(retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(m_client);
		m_client = nullptr;
		std::cerr << "Connect to " << url << " failed with " << UA_StatusCode_name(retval) << std::endl;
		return false;
	}
	return true;
}

UA_StatusCode OpcuaClient::clientCall(const NodeId objectId, const NodeId methodId,
			size_t inputSize, const UA_Variant *input, size_t *outputSize, UA_Variant **output) const {
	return UA_Client_call(m_client, objectId, methodId, inputSize, input, outputSize, output);
}

NodeId OpcuaClient::browsePathToNodeId(const NodeId& startingNode, const std::vector<PathElement>& path) {
	NodeId result;
	if(path.size() == 0) {
		std::cerr << "Failed to browse path since length is 0" << std::endl;
		return result;
	}
	/**
	 * @warn This is a very poor cache implementation, as it has no invalidation algorithm and does *not*
	 * take care about different startingNode-Values!
	 */
	auto it = m_nodeIdCache.find(path);
	if(it != m_nodeIdCache.end()) {
		std::cout << "Found nodeId for Browsepath to '"<< path.back().browseName <<"' in cache" << std::endl;
		return it->second;
	}
	UA_BrowsePath browsePath;
	UA_BrowsePath_init(&browsePath);
	browsePath.startingNode = startingNode;
	browsePath.relativePath.elements = (UA_RelativePathElement*)UA_Array_new(path.size(), &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]);
	browsePath.relativePath.elementsSize = path.size();

	size_t i = 0;
	for (const auto p : path) {
		UA_RelativePathElement *elem = &browsePath.relativePath.elements[i++];
		elem->referenceTypeId = NodeId(0, p.referenceId);
		elem->targetName = UA_QUALIFIEDNAME_ALLOC(p.ns, p.browseName.c_str());
	}

	UA_TranslateBrowsePathsToNodeIdsRequest request;
	UA_TranslateBrowsePathsToNodeIdsRequest_init(&request);
	request.browsePaths = &browsePath;
	request.browsePathsSize = 1;
	UA_TranslateBrowsePathsToNodeIdsResponse response = UA_Client_Service_translateBrowsePathsToNodeIds(m_client, request);
	if(response.responseHeader.serviceResult == UA_STATUSCODE_GOOD && response.resultsSize == 1) {
		if(response.results[0].targetsSize == 1) {
			result = response.results[0].targets[0].targetId.nodeId;
			//std::cout << "Found node for Browsepath: " << result.namespaceIndex << "," <<  result.identifier.numeric << std::endl;
		} else {
			std::cerr << "translateBrowsePathsToNodeIds failed: " << UA_StatusCode_name(response.results[0].statusCode) << std::endl;
		}
	} else {
		std::cerr << "translateBrowsePathsToNodeIds failed: " << UA_StatusCode_name(response.responseHeader.serviceResult) << std::endl;
	}
	UA_BrowsePath_clear(&browsePath);
	UA_TranslateBrowsePathsToNodeIdsResponse_clear(&response);

	m_nodeIdCache.emplace(path, result); // cache request for later requests

	return result;
}

std::map<std::string, NodeId> OpcuaClient::getHierarichalNodes(const NodeId& nodeId) const {
	std::map<std::string, NodeId> ret;
	const std::set<int> hierarchicalReferences = {
			UA_NS0ID_HASCHILD,
			UA_NS0ID_HASEVENTSOURCE,
			UA_NS0ID_AGGREGATES,
			UA_NS0ID_HASCOMPONENT,
			UA_NS0ID_HASORDEREDCOMPONENT,
			UA_NS0ID_HASHISTORICALCONFIGURATION,
			UA_NS0ID_HASPROPERTY,
			UA_NS0ID_HASSUBTYPE,
			UA_NS0ID_HASEVENTSOURCE,
			UA_NS0ID_HASNOTIFIER,
			UA_NS0ID_ORGANIZES
	};
	UA_BrowseRequest bReq;
	UA_BrowseRequest_init(&bReq);
	bReq.requestedMaxReferencesPerNode = 0;
	bReq.nodesToBrowse = UA_BrowseDescription_new();
	bReq.nodesToBrowseSize = 1;
	bReq.nodesToBrowse[0].nodeId = nodeId;
	bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_REFERENCETYPEINFO
			| UA_BROWSERESULTMASK_BROWSENAME; /* only fetch reference type and browsename */
	bReq.nodesToBrowse[0].nodeClassMask = 0;
	UA_BrowseResponse bResp = UA_Client_Service_browse(m_client, bReq);
	if (bResp.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
		std::cerr << "Read attributes of " << NodeId(nodeId).toString()
				<< " failed: "
				<< UA_StatusCode_name(bResp.responseHeader.serviceResult)
				<< std::endl;
	} else {
		for (size_t i = 0; i < bResp.resultsSize; ++i) {
			for (size_t j = 0; j < bResp.results[i].referencesSize; ++j) {
				UA_ReferenceDescription *ref = &(bResp.results[i].references[j]);
				if (hierarchicalReferences.find(
						ref->referenceTypeId.identifier.numeric)
						!= hierarchicalReferences.end()) {
					NodeId id(ref->nodeId.nodeId);
					ret[OpcUaUtil::toString(ref->browseName.name)] = id;
				}
			}
		}
	}
	UA_BrowseRequest_clear(&bReq);
	UA_BrowseResponse_clear(&bResp);

	return ret;
}

