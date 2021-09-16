/*
 * Recording.hpp
 *
 *  Created on: 06.09.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#ifndef RECORDING_HPP_
#define RECORDING_HPP_

#include "OpcuaClient.hpp"
#include "records.pb.h"

#include <list>
#include <vector>
#include <map>

class Umg801;

class Recording {
public:
	Recording(Umg801& client, const uint32_t& id, const NodeId& nodeId);

	/**
	 * Fetch all relevant NodeIDs for reading Recording data. This should be done before any
	 * other operation!
	 * @note: This could be done more efficient by caching and summarized opcua-requests
	 * @return OPC-UA Statuscode
	 */
	UA_StatusCode getNodeIds();

	/**
	 * Run a OPC-UA-RPC-Call to /Objects/Device/Recordings/Recording<id>/data/GetRange()
	 * to determine time-range of available Data
	 * @startTime Output-Parameter which is filled with timestamp of the oldest available data point
	 * @endTime Output-Parameter which is filled with timestamp of the newest available data point
	 * @return OPC-UA Statuscode
	 */
	UA_StatusCode getRange(UA_DateTime& startTime, UA_DateTime& endTime) const;

	/**
	 * Get Number of Recording Points on device in a certain Time Range.
	 * @param startTime
	 * @param endTime
	 * @return Number of recordings, -1 on error
	 */
	int countByRange(const UA_DateTime& startTime, const UA_DateTime& endTime) const;

	/**
	 * Read Recoding Data beginning on a certain Start-Time limited by a number of recording points.
	 * This method will internally subsequently do OPC-UA-RPC-Calls to
	 *  /Objects/Device/Recordings/Recording<id>/data/ReadByStartAndCount()
	 * until all requested data is read. It must be called multiple times, because the device will always
	 * return a maximum payload of 1MB!
	 * @note For this example the resulting Data is just printed to STDOUT
	 * @param startTime Timestamp for start reading
	 * @param count Number of recording points to be read beginning on startTime
	 * @return OPC-UA Statuscode
	 */
	UA_StatusCode readByStartAndCount(const UA_DateTime& startTime, const uint32_t& count) const;

	uint32_t getId() const;

private:
	/**
	 * Internal Object to hold information about a single value from RecordingConfiguration
	 * including belonging browsePath
	 */
	class RecordingValueInfo {
	public:
		RecordingValueInfo(const UA_RecordingValueInfo &i, const std::string &s) :
				info(i), browsePath(s) {}
		UA_RecordingValueInfo info;
		std::string browsePath;
	};

	/**
	 * Internal Object to hold all information for mapping of values in Protobuffers
	 */
	class RecordingConfiguration {
	public:
		RecordingConfiguration() :
			id(0), algorithm(), extremals(), interval_seconds(), values() {}
		uint32_t id;
		UA_RecordingAlgorithm algorithm;
		UA_RecordingExtremals extremals;
		uint32_t interval_seconds;
		std::vector<RecordingValueInfo> values;
	};

	/*
	 * Print function to print a single measurement value of a recording point including its extremals
	 * (sample/average + min;max;min_timestamp;max_timestamp)
	 * @note The Output-format is pseudo JSON; You may want to use a JSON-Library here (omitted for less dependencies)
	 * @param cfg belongingRecordingConfiguration to determine algorithm and extremals
	 * @param protobuf Reference to the Protobuffer element the algorithm/extremal values shall be fetched from
	 * @param index Reference to the index that is used to iterate through the values in the different protobuf value-type elements
	 * @param browsename Browsename of the measurement
	 */
	template<typename T>
	void printProtobuf(RecordingConfiguration& cfg, const T& protobuf, size_t& index, const std::string& browsename) const;

	/**
	 * This method decodes the data received from device according to the referenced Recoding configuration.
	 * Each recording point consists of a tuple of RecordingConfiguration-Id and a Bytestring representing a
	 * Protobuffer that holds the actual timestamps, Measurement values and extremals.
	 * This method reads the referenced Configuration and maps browsepaths with the values from the
	 * protobuffer.
	 * @param data List of Tuples with RecordingConfiguration-Id and Protobuffers holding the actual recorded measurement values
	 * @return OPC-UA Statuscode
	 */
	UA_StatusCode decodeData(std::list<std::pair<uint32_t, records::RecordedData>>& data) const;

	/**
	 * This method reads a recording configuration from the device with a certain ID.
	 * It also translates the NodeIds of configuration to the belonging browse-paths.
	 * @param id Id of the RecordingConfiguration to be read from device
	 * @param cfg Output parameter that is filled with the read configuration on success
	 * @return OPC-UA Statuscode
	 */
	UA_StatusCode readRecordingConfiguration(const uint32_t& id, RecordingConfiguration& cfg) const;

	Umg801& m_client;
	const uint32_t m_id;
	const NodeId m_nodeId;
	NodeId m_dataId;
	NodeId m_getRangeId;
	NodeId m_countByRangeId;
	NodeId m_readByStartAndCountIdId;
	std::map<uint32_t, NodeId> m_configIds;
};

#endif /* RECORDING_HPP_ */
