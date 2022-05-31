/*
 * Recording.cpp
 *
 *  Created on: 06.09.2021
 *   Copyright: 2021 by Janitza electronics GmbH
 */

#include "Recording.hpp"
#include "Umg801.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

Recording::Recording(Umg801& client, const uint32_t& id, const NodeId& nodeId) :
	m_client(client),
	m_id(id),
	m_nodeId(nodeId),
	m_dataId(),
	m_getRangeId(),
	m_countByRangeId(),
	m_readByStartAndCountIdId(),
	m_configIds() {}


UA_StatusCode Recording::getNodeIds() {
	auto recordChilds = m_client.getHierarichalNodes(m_nodeId);

	m_dataId = recordChilds["Data"];
	if(m_dataId.isNull()) {
		std::cerr << "NodeId of Node 'Data' of Recording"<<m_id<<" not found!" << std::endl;
		return UA_STATUSCODE_BADINTERNALERROR;
	}
	for(const auto c : recordChilds) {
		if(OpcUaUtil::isPrefix(c.first,"RecordingConfiguration")) {
			m_configIds.emplace(OpcUaUtil::getIdSuffix(c.first),c.second);
		}
	}
	m_getRangeId = m_client.browsePathToNodeId(NodeId(0,UA_NS0ID_BASEOBJECTTYPE),
			{{2,"RecordingType", UA_NS0ID_HASSUBTYPE},
			 {2,"Data", UA_NS0ID_HASCOMPONENT},
			 {2,"GetRange", UA_NS0ID_HASCOMPONENT}});
	if(m_getRangeId.isNull()) {
		std::cerr << "Node BaseObjectType/RecordingType/Data/GetRange not found!" << std::endl;
		return UA_STATUSCODE_BADINTERNALERROR;
	}
	m_countByRangeId = m_client.browsePathToNodeId(NodeId(0,UA_NS0ID_BASEOBJECTTYPE),
			{{2,"RecordingType", UA_NS0ID_HASSUBTYPE},
			 {2,"Data", UA_NS0ID_HASCOMPONENT},
			 {2,"CountByRange", UA_NS0ID_HASCOMPONENT}});
	if(m_countByRangeId.isNull()) {
		std::cerr << "Node BaseObjectType/RecordingType/Data/CountByRange not found!" << std::endl;
		return UA_STATUSCODE_BADINTERNALERROR;
	}
	m_readByStartAndCountIdId = m_client.browsePathToNodeId(NodeId(0, UA_NS0ID_BASEOBJECTTYPE),
				{{2,"RecordingType", UA_NS0ID_HASSUBTYPE},
				 {2,"Data", UA_NS0ID_HASCOMPONENT},
				 {2,"ReadByStartAndCount", UA_NS0ID_HASCOMPONENT}});
	if(m_readByStartAndCountIdId.isNull()) {
		std::cerr << "Node BaseObjectType/RecordingType/Data/ReadByStartAndCount not found!" << std::endl;
		return UA_STATUSCODE_BADINTERNALERROR;
	}
	return UA_STATUSCODE_GOOD;
}

uint32_t Recording::getId() const {
	return m_id;
}

UA_StatusCode Recording::getRange(UA_DateTime& startTime, UA_DateTime& endTime) const {
	UA_StatusCode retval = UA_STATUSCODE_GOOD;

	size_t outputSize;
	UA_Variant *output;
	retval = m_client.clientCall(m_dataId, m_getRangeId, 0, nullptr, &outputSize, &output);
	if(retval != UA_STATUSCODE_GOOD) {
		std::cerr << "Method call to GetRange of Recording"<<m_id<<" was unsuccessful: " << UA_StatusCode_name(retval) << std::endl;
	} else {
		startTime = *(UA_DateTime*)output[0].data;
		endTime = *(UA_DateTime*)output[1].data;
		UA_Array_delete(output, outputSize, &UA_TYPES[UA_TYPES_VARIANT]);
	}

	return retval;
}

int Recording::countByRange(const UA_DateTime& startTime, const UA_DateTime& endTime) const {
	int retval = 0;
	UA_StatusCode status = UA_STATUSCODE_GOOD;

	UA_Variant input[2] = {0};
	size_t outputSize;
	UA_Variant *output;
	UA_Variant_setScalarCopy(&input[0], &startTime, &UA_TYPES[UA_TYPES_DATETIME]);
	UA_Variant_setScalarCopy(&input[1], &endTime, &UA_TYPES[UA_TYPES_DATETIME]);
	status = m_client.clientCall(m_dataId, m_countByRangeId, 2, input, &outputSize, &output);
	if(status != UA_STATUSCODE_GOOD) {
		std::cerr << "Method call to CountByRange of Recording"<<m_id<<" was unsuccessful: " << UA_StatusCode_name(status) << std::endl;
		retval = -1;
	} else {
		retval = *(UA_UInt32*)output[0].data;
		UA_Array_delete(output, outputSize, &UA_TYPES[UA_TYPES_VARIANT]);
	}
	return retval;
}

UA_StatusCode Recording::readByStartAndCount(const UA_DateTime& startTime, const uint32_t& count) const {
	UA_StatusCode retval = UA_STATUSCODE_GOOD;
	int remain = count;
	UA_DateTime nextStartTime = startTime;
	std::list<std::pair<uint32_t, records::RecordedData>> data;
	while(remain > 0) {
		UA_Variant input[2] = {0};
		size_t outputSize;
		UA_Variant *output;
		UA_Variant_setScalarCopy(&input[0], &nextStartTime, &UA_TYPES[UA_TYPES_DATETIME]);
		UA_Variant_setScalarCopy(&input[1], &count, &UA_TYPES[UA_TYPES_UINT32]);
		retval = m_client.clientCall(m_dataId, m_readByStartAndCountIdId, 2, input, &outputSize, &output);
		for(auto& inp : input) {
			UA_Variant_clear(&inp);
		}
		if(retval != UA_STATUSCODE_GOOD) {
			std::cerr << "Method call 'ReadByStartAndCount' was unsuccessful " << UA_StatusCode_name(retval) << std::endl;
			break;
		} else {
			if(nextStartTime >= *(UA_DateTime*)output[1].data) {
				std::cerr << "Warning: Read LastDateTime is not bigger than start-time from last read! Stopping here!";
				remain = 0;
			}
			nextStartTime = *(UA_DateTime*)output[1].data;
			const size_t recordingPoints = output[2].arrayDimensions[0];
			UA_ExtensionObject* e = (UA_ExtensionObject*)output[2].data;
			for(size_t i=0; i<recordingPoints; i++) {
				UA_RecordingPoint* p = (UA_RecordingPoint*)e[i].content.decoded.data;
				UA_ByteString* b = &(p->data);
				/* The received Data is stored in a google protobuf structure.
				 * Parse the received Byte-String as protobuf here!
				 */
				records::RecordedData protobuf;
				if(!protobuf.ParseFromArray(b->data, b->length)) {
					std::cerr << "Failed to decode protobuffer of Recording-Point " << i << "(Recording" << m_id << ")" << std::endl;
				} else {
					data.emplace_back(p->configId, protobuf);
				}
			}
			UA_Array_delete(output, outputSize, &UA_TYPES[UA_TYPES_VARIANT]);
			if(recordingPoints == 0) {
				std::cerr << "Warning: no recording points returned while expecting " << remain << " more points! Stopping here!" << std::endl;
				remain = 0;
			}
			remain -= recordingPoints;
		}
	}

	retval = decodeData(data);

	return retval;
}


template<typename T>
void Recording::printProtobuf(RecordingConfiguration& cfg, const T& protobuf, size_t& index, const std::string& browsename) const {
	std::cout << "\"" << browsename << "\" : { ";
	if(cfg.algorithm == UA_RECORDINGALGORITHM_AVERAGE) {
		std::cout << "\"avg\" : " << protobuf.avgvalue(index) << " ";
	} else {
		std::cout << "\"sample\" : " << protobuf.sample(index) << " ";
	}
	if(cfg.extremals.minimum) {
		std::cout << ", \"min\" : " << protobuf.minvalue(index) << " ";
		if(cfg.extremals.timestamps) {
			std::cout << ", \"min_time\" : " << protobuf.mintimestamp(index) << " ";
		}
	}
	if(cfg.extremals.maximum) {
		std::cout << ", \"max\" : " << protobuf.maxvalue(index) << " ";
		if(cfg.extremals.timestamps) {
			std::cout << ", \"max_time\" : " << protobuf.maxtimestamp(index) << " ";
		}
	}
	std::cout << " }, " << std::endl;

	/* Here we increment the index of the specific type-array in the protobuffer,
	 * so the next call will fetch the next element.
	 */
	index++;
}

UA_StatusCode Recording::decodeData(std::list<std::pair<uint32_t, records::RecordedData>>& data) const {
	UA_StatusCode retval = UA_STATUSCODE_GOOD;
	std::map<uint32_t, RecordingConfiguration> configs;

	for(const auto& d : data) {
		// Get RecordingConfiguration from configs. Read from device if not known yet.
		auto it = configs.find(d.first);
		if(it == configs.end()) {
			RecordingConfiguration cfg;
			retval = readRecordingConfiguration(d.first, cfg);
			if(retval != UA_STATUSCODE_GOOD) {
				return retval;
			}
			it = configs.emplace(d.first, cfg).first;
		}
		RecordingConfiguration& cfg = it->second;

		// Convert and print timestamp. Protobuffer holds time in Seconds UTC (POSIX time)
		std::time_t time = d.second.starttimeutc();
		std::cout << "{ \"time\" : \"" << std::put_time(std::localtime(&time), "%Y-%m-%d %X") << "\"" << std::endl;

		size_t counts[8] = {0};

		/* The protobuffer holds arrays for each possible datatype that can be stored.
		 * Depending of the configuration, the values are stored to the type-belonging arrays
		 * in the same order as in configuration. To match values, we iterate throught the values in
		 * configuration and fetch the next value from the array of the protobuffer.
		 */
		for(const auto& var : cfg.values) {
			if(var.info.status != UA_REFERENCESTATUS_AVAILABLE) {
				// ignore values that are not available.
				counts[var.info.typeInfo.dataType]++;
				continue;
			}
			std::string name = var.browsePath;
			if(var.info.typeInfo.array) {
				name += "[" + std::to_string(var.info.value.arrayIndex) + "]";
			}

			switch(var.info.typeInfo.dataType) {
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_BOOLEAN:
				printProtobuf(cfg, d.second.bool_(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_INT32:
				printProtobuf(cfg, d.second.sint32(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_UINT32:
				printProtobuf(cfg, d.second.uint32(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_INT64:
				printProtobuf(cfg, d.second.sint64(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_UINT64:
				printProtobuf(cfg, d.second.uint64(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_FLOAT:
				printProtobuf(cfg, d.second.float_(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_DOUBLE:
				printProtobuf(cfg, d.second.double_(), counts[var.info.typeInfo.dataType], name); break;
			case UA_RecordingDataType::UA_RECORDINGDATATYPE_UNDEFINED:
			default:
				std::cerr << "Found UNDEFINED datatype; this should _not_ happen!";
			}
		}

		std::cout << "}," << std::endl;

	}

	return retval;
}

UA_StatusCode Recording::readRecordingConfiguration(const uint32_t& id, RecordingConfiguration& cfg) const {
	UA_StatusCode retval = UA_STATUSCODE_GOOD;
	const auto cfgIter = m_configIds.find(id);
	if(cfgIter == m_configIds.end()) {
		std::cerr << "Failed to get " << "RecordingConfiguration"+std::to_string(id) << " from node " << m_nodeId.toString() <<": NodeId not found!"<< std::endl;
		return UA_STATUSCODE_BADNOTFOUND;
	}
	auto nodes = m_client.getHierarichalNodes(cfgIter->second);
	cfg.id = id;
	cfg.algorithm = m_client.getVariantValue<UA_RecordingAlgorithm>(nodes["Algorithm"]);
	cfg.extremals = m_client.getVariantValue<UA_RecordingExtremals>(nodes["Extremals"]);
	cfg.interval_seconds = m_client.getVariantValue<UA_UInt32>(nodes["Interval"]);
	for(const auto& v : m_client.getVariantArrayValue<UA_RecordingValueInfo>(nodes["Values"])) {
		std::string browsepath = "";
		if(v.status == UA_REFERENCESTATUS_AVAILABLE) {
			auto p = m_client.lookup(NodeId(v.value.node), UA_TAG_RECORDABLE);
			if(p) {
				browsepath = *p;
			}
		}
		cfg.values.emplace_back(RecordingValueInfo(v, browsepath));
	}

	// Print configuration information
	std::cout << "RecordingConfig" << cfg.id << ": Algorithm=";
	if(cfg.algorithm == UA_RECORDINGALGORITHM_SAMPLE) std::cout << "Sample; Extremals:";
	else std::cout << "Average; Extremals:";
	if(cfg.extremals.maximum) std::cout << "max,";
	if(cfg.extremals.minimum) std::cout << "min,";
	if(cfg.extremals.timestamps) std::cout << "timestmaps";
	std::cout << "; Interval=" << cfg.interval_seconds << "sec ";
	std::cout << "; Value-Count=" << cfg.values.size() << std::endl;

	return retval;
}

