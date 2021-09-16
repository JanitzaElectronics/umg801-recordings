/*
 * OpcUaUtil.hpp
 *
 *  Created on: 22.04.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#ifndef OPCUAUTIL_HPP_
#define OPCUAUTIL_HPP_

#include <open62541/open62541.h>
#include <string>

class OpcUaUtil {
public:
	OpcUaUtil();
	virtual ~OpcUaUtil();

	static const std::string toString(const UA_String& uaStr);
	static const std::string dateTimeToString(const UA_DateTime& t);
	static bool isPrefix(const std::string& str, const std::string& prefix);
	static int getIdSuffix(const std::string& str);
};

#endif /* OPCUAUTIL_HPP_ */
