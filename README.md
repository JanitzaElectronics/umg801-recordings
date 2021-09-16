# UMG801 Read Recordings Example
Example project for reading recorded data from UMG801 via OPC-UA

## Dependencies
This project requires the following packages for building:
* Basic Build environment (Cmake, Make, Gnu C++ Compiler)
* Open62521 OPC-UA Library (https://github.com/open62541/open62541)
* Google Protobuf Library including ProtoC-Compiler (https://github.com/protocolbuffers/protobuf)

## Building
Assuming all Dependencies are available in the build-system, the project can easily be built with
```
mkdir build
cd build/
cmake ..
make
```
