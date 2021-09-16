//============================================================================
// Name        : main.cpp
// Version     :
// Copyright   : 2021 by Janitza electronics GmbH
// Description : Read Recordings example for UMG801
//============================================================================

#include "Umg801.hpp"

#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <cstdlib>

int main(int argc, char* argv[]) {
	std::string serverHost = "localhost";
	uint16_t serverPort = 4840;

	/* Set timezone env for correct localtime */
	setenv("TZ", "/usr/share/zoneinfo/Europe/Berlin", 1); // POSIX-specific!!

	if(argc >= 2) {
		serverHost = argv[1];
		if(argc == 3) {
			serverPort = std::atoi(argv[2]);
		}
	} else {
		std::cout << "Useage: " << argv[0] << " <host> [<port>]" << std::endl;
		std::cout << "\thost:\tHostname/IP of the device to be read out" << std::endl;
		std::cout << "\tport:\tOPCUA-Port number (optional, defaults to 4840)" << std::endl;
		return 0;
	}

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	const std::string serverUrl = "opc.tcp://"+serverHost+":"+std::to_string(serverPort);
	Umg801 umg;

	if(!umg.connect(serverUrl)) {
		std::cerr << "Failed to connect UMG801 OPCUA-Service on '" << serverUrl << "'!" << std::endl;
		return (1);
	}

	int ret = 0;
	for (const auto& r : umg.getRecordings()) {
		UA_DateTime startTime, stopTime;
		/* Here would be a good point to set 'startTime' to last synchronization time,
		 * so not all data must be fetched every time. After successful read out, 'endTime'
		 * can be stored as new synchronization time.
		 * This example just fetches all data, so we grab overall time range from device.
		 */
		if(r.getRange(startTime, stopTime) == UA_STATUSCODE_GOOD) {
			auto count = r.countByRange(startTime,stopTime);
			if(count > 0) {
				std::cout << "Found Recording " << r.getId() << " with " << count << " Points between "
						<< OpcUaUtil::dateTimeToString(startTime) << " and " << OpcUaUtil::dateTimeToString(stopTime) << std::endl;
				ret |= (r.readByStartAndCount(startTime, count) != UA_STATUSCODE_GOOD);
			}
		}
	}

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	std::cout << "Finished in " << (float)duration/1000000.0 << "s" << std::endl;

	return ret;
}
