/*
 * CustomUaTypes.hpp
 *
 *  Created on: 31.05.2022
 *   Copyright: 2017 by Janitza electronics GmbH
 */

#ifndef SRC_CUSTOMUATYPES_HPP_
#define SRC_CUSTOMUATYPES_HPP_

#include <open62541/types.h>

/**
 * ReferenceStatus
 * ^^^^^^^^^^^^^^^
 * Describes the status of a referenced. */
typedef enum {
    UA_REFERENCESTATUS_UNDEFINED = 0,
    UA_REFERENCESTATUS_NOTFOUND = 1,
    UA_REFERENCESTATUS_INVALID = 2,
    UA_REFERENCESTATUS_AVAILABLE = 3,
    UA_REFERENCESTATUS_MISSING = 4,
    __UA_REFERENCESTATUS_FORCE32BIT = 0x7fffffff
} UA_ReferenceStatus;

/**
 * RecordingDataType
 * ^^^^^^^^^^^^^^^^^
 * Represents the different data types for recordable values. */
typedef enum {
    UA_RECORDINGDATATYPE_UNDEFINED = 0,
    UA_RECORDINGDATATYPE_BOOLEAN = 1,
    UA_RECORDINGDATATYPE_INT32 = 2,
    UA_RECORDINGDATATYPE_UINT32 = 3,
    UA_RECORDINGDATATYPE_INT64 = 4,
    UA_RECORDINGDATATYPE_UINT64 = 5,
    UA_RECORDINGDATATYPE_FLOAT = 6,
    UA_RECORDINGDATATYPE_DOUBLE = 7,
    __UA_RECORDINGDATATYPE_FORCE32BIT = 0x7fffffff
} UA_RecordingDataType;

/**
 * RecordingAlgorithm
 * ^^^^^^^^^^^^^^^^^^
 * Describes the algorithm to use for value calculation. */
typedef enum {
    UA_RECORDINGALGORITHM_SAMPLE = 0,
    UA_RECORDINGALGORITHM_AVERAGE = 1,
    __UA_RECORDINGALGORITHM_FORCE32BIT = 0x7fffffff
} UA_RecordingAlgorithm;

/**
 * Tag
 * ^^^
 * Different tags that are available for a lookup function. */
typedef enum {
    UA_TAG_MEASUREMENT = 0,
    UA_TAG_ENERGY = 1,
    UA_TAG_RECORDABLE = 2,
    UA_TAG_FIELDBUS = 3,
    __UA_TAG_FORCE32BIT = 0x7fffffff
} UA_Tag;

/**
 * RecordingExtremals
 * ^^^^^^^^^^^^^^^^^^
 * Holds the value kinds to include into recordings. */
typedef struct {
    UA_Boolean minimum;
    UA_Boolean maximum;
    UA_Boolean timestamps;
} UA_RecordingExtremals;

/**
 * RecordingValue
 * ^^^^^^^^^^^^^^
 * Represents the configurable part of a recordable value to an OPC-UA node. */
typedef struct {
    UA_NodeId node;
    UA_Int32 arrayIndex;
} UA_RecordingValue;

/**
 * RecordingTypeInfo
 * ^^^^^^^^^^^^^^^^^
 * Holds the recording type information for a node. */
typedef struct {
    UA_RecordingDataType dataType;
    UA_Boolean array;
    UA_UInt16 arraySize;
} UA_RecordingTypeInfo;

/**
 * RecordingValueInfo
 * ^^^^^^^^^^^^^^^^^^
 * Represents a recording value with derived informations to an OPC-UA node with options. */
typedef struct {
    UA_ReferenceStatus status;
    UA_RecordingValue value;
    UA_RecordingTypeInfo typeInfo;
} UA_RecordingValueInfo;

/**
 * RecordingPoint
 * ^^^^^^^^^^^^^^
 * Represents a single recording point. */
typedef struct {
    UA_UInt32 configId;
    UA_ByteString data;
} UA_RecordingPoint;

/**
 * LookupInfo
 * ^^^^^^^^^^
 * Holds the result of a node lookup request. */
typedef struct {
    UA_NodeId nodeId;
    UA_String browsePath;
} UA_LookupInfo;

/* RecordingExtremals */
static UA_DataTypeMember RecordingExtremals_members[3] = {
{
    UA_TYPENAME("Minimum") /* .memberName */
    &UA_TYPES[UA_TYPES_BOOLEAN], /* .memberTypeIndex */
    0, /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("Maximum") /* .memberName */
	&UA_TYPES[UA_TYPES_BOOLEAN], /* .memberTypeIndex */
    offsetof(UA_RecordingExtremals, maximum) - offsetof(UA_RecordingExtremals, minimum) - sizeof(UA_Boolean), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("Timestamps") /* .memberName */
	&UA_TYPES[UA_TYPES_BOOLEAN], /* .memberTypeIndex */
    offsetof(UA_RecordingExtremals, timestamps) - offsetof(UA_RecordingExtremals, maximum) - sizeof(UA_Boolean), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
}};

/* RecordingExtremals */
static const UA_DataType UA_RecordingExtremalsType = {
    UA_TYPENAME("RecordingExtremals") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {1028LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {1094LU}}, /* .binaryEncodingId */
    sizeof(UA_RecordingExtremals), /* .memSize */
    UA_DATATYPEKIND_STRUCTURE, /* .typeKind */
    true, /* .pointerFree */
    false, /* .overlayable */
    3, /* .membersSize */
    RecordingExtremals_members  /* .members */
};


/* RecordingPoint */
static UA_DataTypeMember RecordingPoint_members[2] = {
{
    UA_TYPENAME("ConfigId") /* .memberName */
    &UA_TYPES[UA_TYPES_UINT32], /* .memberType */
    0, /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("Data") /* .memberName */
	&UA_TYPES[UA_TYPES_BYTESTRING], /* .memberType */
    offsetof(UA_RecordingPoint, data) - offsetof(UA_RecordingPoint, configId) - sizeof(UA_UInt32), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
}};

/* RecordingPoint */
static const UA_DataType UA_RecordingPointType = {
    UA_TYPENAME("RecordingPoint") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {3018LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {5021LU}}, /* .binaryEncodingId */
    sizeof(UA_RecordingPoint), /* .memSize */
    UA_DATATYPEKIND_STRUCTURE, /* .typeKind */
    false, /* .pointerFree */
    false, /* .overlayable */
    2, /* .membersSize */
    RecordingPoint_members  /* .members */
};

/* RecordingDataType */
static const UA_DataType UA_RecordingDataTypeType = {
    UA_TYPENAME("RecordingDataType") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {1030LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {0}}, /* .binaryEncodingId */
    sizeof(UA_RecordingDataType), /* .memSize */
    UA_DATATYPEKIND_ENUM, /* .typeKind */
    true, /* .pointerFree */
    UA_BINARY_OVERLAYABLE_INTEGER, /* .overlayable */
    0, /* .membersSize */
    nullptr  /* .members */
};

/* ReferenceStatus */
static const UA_DataType UA_ReferenceStatusType = {
    UA_TYPENAME("ReferenceStatus") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {3004LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {0}}, /* .binaryEncodingId */
    sizeof(UA_ReferenceStatus), /* .memSize */
    UA_DATATYPEKIND_ENUM, /* .typeKind */
    true, /* .pointerFree */
    UA_BINARY_OVERLAYABLE_INTEGER, /* .overlayable */
    0, /* .membersSize */
    nullptr  /* .members */
};

/* RecordingValue */
static UA_DataTypeMember RecordingValue_members[2] = {
{
    UA_TYPENAME("Node") /* .memberName */
	&UA_TYPES[UA_TYPES_NODEID], /* .memberType */
    0, /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("ArrayIndex") /* .memberName */
    &UA_TYPES[UA_TYPES_INT32], /* .memberType */
    offsetof(UA_RecordingValue, arrayIndex) - offsetof(UA_RecordingValue, node) - sizeof(UA_NodeId), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
}};

/* RecordingValue */
static const UA_DataType UA_RecordingValueType = {
    UA_TYPENAME("RecordingValue") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {3010LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {5087LU}}, /* .binaryEncodingId */
    sizeof(UA_RecordingValue), /* .memSize */
    UA_DATATYPEKIND_STRUCTURE, /* .typeKind */
    false, /* .pointerFree */
    false, /* .overlayable */
    2, /* .membersSize */
    RecordingValue_members  /* .members */
};

/* RecordingTypeInfo */
static UA_DataTypeMember RecordingTypeInfo_members[3] = {
{
    UA_TYPENAME("DataType") /* .memberName */
    &UA_RecordingDataTypeType, /* .memberTypeIndex */
    0, /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("Array") /* .memberName */
    &UA_TYPES[UA_TYPES_BOOLEAN], /* .memberTypeIndex */
    offsetof(UA_RecordingTypeInfo, array) - offsetof(UA_RecordingTypeInfo, dataType) - sizeof(UA_RecordingDataType), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("ArraySize") /* .memberName */
	&UA_TYPES[UA_TYPES_UINT16], /* .memberTypeIndex */
    offsetof(UA_RecordingTypeInfo, arraySize) - offsetof(UA_RecordingTypeInfo, array) - sizeof(UA_Boolean), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
}};

/* RecordingTypeInfo */
static const UA_DataType UA_RecordingTypeInfoType = {
    UA_TYPENAME("RecordingTypeInfo") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {1031LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {1099LU}}, /* .binaryEncodingId */
    sizeof(UA_RecordingTypeInfo), /* .memSize */
    UA_DATATYPEKIND_STRUCTURE, /* .typeKind */
    true, /* .pointerFree */
    false, /* .overlayable */
    3, /* .membersSize */
    RecordingTypeInfo_members  /* .members */
};

/* RecordingValueInfo */
static UA_DataTypeMember RecordingValueInfo_members[3] = {
{
    UA_TYPENAME("Status") /* .memberName */
    &UA_ReferenceStatusType, /* .memberType */
    0, /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("Value") /* .memberName */
    &UA_RecordingValueType, /* .memberType */
    offsetof(UA_RecordingValueInfo, value) - offsetof(UA_RecordingValueInfo, status) - sizeof(UA_ReferenceStatus), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("TypeInfo") /* .memberName */
    &UA_RecordingTypeInfoType, /* .memberType */
    offsetof(UA_RecordingValueInfo, typeInfo) - offsetof(UA_RecordingValueInfo, value) - sizeof(UA_RecordingValue), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
}};

static const UA_DataType UA_RecordingValueInfoType = {
    UA_TYPENAME("RecordingValueInfo") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {3011LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {5089LU}}, /* .binaryEncodingId */
    sizeof(UA_RecordingValueInfo), /* .memSize */
    UA_DATATYPEKIND_STRUCTURE, /* .typeKind */
    false, /* .pointerFree */
    false, /* .overlayable */
    3, /* .membersSize */
    RecordingValueInfo_members  /* .members */
};

/* LookupInfo */
static UA_DataTypeMember LookupInfo_members[2] = {
{
    UA_TYPENAME("NodeId") /* .memberName */
    &UA_TYPES[UA_TYPES_NODEID], /* .memberTypeIndex */
    0, /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
},
{
    UA_TYPENAME("BrowsePath") /* .memberName */
    &UA_TYPES[UA_TYPES_STRING], /* .memberTypeIndex */
    offsetof(UA_LookupInfo, browsePath) - offsetof(UA_LookupInfo, nodeId) - sizeof(UA_NodeId), /* .padding */
    false, /* .isArray */
    false  /* .isOptional */
}};

/* LookupInfo */
static const UA_DataType UA_LookupInfoType = {
    UA_TYPENAME("LookupInfo") /* .typeName */
    {2, UA_NODEIDTYPE_NUMERIC, {3017LU}}, /* .typeId */
    {2, UA_NODEIDTYPE_NUMERIC, {5003LU}}, /* .binaryEncodingId */
    sizeof(UA_LookupInfo), /* .memSize */
    UA_DATATYPEKIND_STRUCTURE, /* .typeKind */
    false, /* .pointerFree */
    false, /* .overlayable */
    2, /* .membersSize */
    LookupInfo_members  /* .members */
};


#endif /* SRC_CUSTOMUATYPES_HPP_ */
