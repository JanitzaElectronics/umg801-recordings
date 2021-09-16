cmake_minimum_required (VERSION 2.6)

project (umg801-recordings)

#
#Dependencies
#

# open62541
if( NOT DEFINED LIBOPEN62541_LIBRARY )
        find_library( LIBOPEN62541_LIBRARY open62541 )
endif( )
if( NOT DEFINED LIBOPEN62541_INCLUDE )
        find_path( LIBOPEN62541_INCLUDE open62541 )
endif( )

if( LIBOPEN62541_LIBRARY AND EXISTS "${LIBOPEN62541_LIBRARY}" )
        message( STATUS "${Green}Found open62541 library at: ${LIBOPEN62541_LIBRARY}${Reset}" )
else( )
        message( SEND_ERROR "${Red}Failed to locate open62541 library.${Reset}" )
endif( )
if( LIBOPEN62541_INCLUDE AND EXISTS "${LIBOPEN62541_INCLUDE}" )
        message( STATUS "${Green}Found open62541 include at: ${LIBOPEN62541_INCLUDE}${Reset}" )
else( )
        message( SEND_ERROR "${Red}Failed to locate open62541 include.${Reset}" )
endif( )

# protobuf
set(Protobuf_USE_STATIC_LIBS OFF)
find_package( Protobuf REQUIRED )

if( NOT Protobuf_PROTOC_EXECUTABLE STREQUAL "NOTFOUND" )
	message(STATUS "Found Protobuf compiler at: ${Protobuf_PROTOC_EXECUTABLE}")
else()
	message(SEND_ERROR "Failed to locate Protobuf compiler!")
endif()
if( NOT Protobuf_LIBRARY STREQUAL "NOTFOUND" AND EXISTS "${Protobuf_LIBRARY}" )
	message(STATUS "Found Protobuf library at: ${Protobuf_LIBRARY}")
else()
	message(SEND_ERROR "Failed to locate Protobuf library!")
endif()
if( NOT Protobuf_INCLUDE_DIR STREQUAL "NOTFOUND" AND EXISTS "${Protobuf_INCLUDE_DIR}" )
	message(STATUS "Found Protobuf includes at: ${Protobuf_INCLUDE_DIR}")
else()
	message(SEND_ERROR "Failed to locate Protobuf includes!")
endif()

#Generate cpp protobuf code
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS src/records.proto)

# Libraries
set( TARGET_LIBS ${LIBOPEN62541_LIBRARY} ${PROTOBUF_LIBRARIES})

# System includes
set( TARGET_SYSTEM_INCLUDE_DIRS
        ${LIBOPEN62541_INCLUDE}
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB_RECURSE BUILD_SRC_FILES_CPP "src/*.cpp" )

add_executable(${PROJECT_NAME} ${BUILD_SRC_FILES_CPP} ${PROTO_SRCS} ${PROTO_HDRS})

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS ON
        VERSION "1.0" )
		
target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${TARGET_LIBS} )

target_include_directories(
        ${PROJECT_NAME} SYSTEM PRIVATE
        ${TARGET_SYSTEM_INCLUDE_DIRS})
