/*
 * Umg801.hpp
 *
 *  Created on: 20.04.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#ifndef UMG801_HPP_
#define UMG801_HPP_


#include "OpcuaClient.hpp"
#include "Recording.hpp"

#include <map>
#include <list>
#include <string>
#include <optional>

class Umg801 : public OpcuaClient {
public:
	Umg801();
	virtual ~Umg801();

	std::list<Recording> getRecordings();

	/**
	 * Lookup a browsepath to a given NodeId. This is done by a RPC-Call to /Objects/Device/Loopup().
	 * The Results of this method are stored to a Cache, so subsequent calls will not lead to
	 * OPC-UA-Communication.
	 * @note For a real application you should think about a Cache-Invalidation
	 * @param id NodeId of the Node the requested Browsepath belongs to.
	 * @param tag Group of Measurements that shall be requested. Default is MEASUREMENTS
	 */
	std::optional<std::string> lookup(const NodeId& id, const UA_Tag& tag = UA_TAG_MEASUREMENT);


private:
	std::map<NodeId,std::string> m_lookupInfo;
};

#endif /* UMG801_HPP_ */
