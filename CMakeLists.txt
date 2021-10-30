cmake_minimum_required(VERSION 3.10)
project(myRDT)



SET(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ADD_EXECUTABLE(myRDT ${SRC_LIST} include/SRSender.h include/SRReceiver.h src/SRSender.cpp src/SRReceiver.cpp include/TCPSender.h include/TCPReceiver.h src/TCPSender.cpp src/TCPReceiver.cpp)
FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES(myRDT  ${NETSIM_LIB})