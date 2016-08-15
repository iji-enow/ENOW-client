cmake_minimum_required (VERSION 2.6)
project (MQTTClient)

# Compiler version
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# The version number.
set (CLIENT_VERSION_MAJOR 1)
set (CLIENT_VERSION_MINOR 0)
# Source directory of the project
set (SOURCE_DIR ./)

# Header files
file(GLOB SOURCE_FILE ./src/*.cpp)
file(GLOB HEADER_FILE ./header/*.hpp)

# wiringPi selection flag
option(USE_WIRINGPI
	"Use wiringPi library" ON)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
if(USE_WIRINGPI)
	find_package(WiringPi REQUIRED)
	find_package(Threads REQUIRED)
	include_directories(${WIRINGPI_INCLUDE_DIRS})
endif(USE_WIRINGPI)

find_package(PahoMqtt REQUIRED)
include_directories(${LIBMQTT_INCLUDE_DIRS})
# add the executable
add_executable(MQTTClient ${HEADER_FILE} ${SOURCE_FILE} ./main.cpp)

if(USE_WIRINGPI)
	target_link_libraries(MQTTClient ${WIRINGPI_LIBRARIES})
	target_link_libraries(MQTTClient ${CMAKE_THREAD_LIBS_INIT}) 
endif(USE_WIRINGPI)

target_link_libraries(MQTTClient ${LIBMQTT_LIBRARIES})
