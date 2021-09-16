/*
 * OpcUaUtil.cpp
 *
 *  Created on: 22.04.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#include "OpcUaUtil.hpp"

#include <chrono>
#include <ctime>

OpcUaUtil::OpcUaUtil() {
}

OpcUaUtil::~OpcUaUtil() {
}

const std::string OpcUaUtil::toString(const UA_String& uaStr) {
	return std::string((const char*) uaStr.data, uaStr.length);
}

const std::string OpcUaUtil::dateTimeToString(const UA_DateTime& t) {
	using namespace std::chrono;
	microseconds us((t - UA_DATETIME_UNIX_EPOCH) / UA_DATETIME_USEC );
	seconds s = duration_cast<seconds>(us);
	std::time_t tm = s.count();
	char buf[100];
	strftime(buf, 100, "%a %b %d %T %Y", localtime(&tm));
	return std::string(buf);
}

bool OpcUaUtil::isPrefix(const std::string& str, const std::string& prefix) {
	return (std::mismatch(prefix.begin(), prefix.end(), str.begin()).first == prefix.end());
}

int OpcUaUtil::getIdSuffix(const std::string& str) {
	size_t i = 0;
	for ( ; i < str.length(); i++ ){
		if ( isdigit(str[i]) )
			break;
	}
	std::string sub = str.substr(i, str.length() - i );
	return atoi(sub.c_str());
}

