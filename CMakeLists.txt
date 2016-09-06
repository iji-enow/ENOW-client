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

set(CMAKE_CXX_FLAGS "-O0")
set(CMAKE_C_FLAGS"-O0")

# The version number.
set (CLIENT_VERSION_MAJOR 1)
set (CLIENT_VERSION_MINOR 0)
# Source directory of the project
set (SOURCE_DIR ./)

# Header files
file(GLOB SOURCE_FILE_CPP ./src/*.cpp)
file(GLOB HEADER_FILE_CPP ./header/*.hpp)
file(GLOB SOURCE_FILE_C ./src/*.c)
file(GLOB HEADER_FILE_C ./header/*.h)

# wiringPi selection flag
option(USE_WIRINGPI
	"Use wiringPi library" OFF)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
if(USE_WIRINGPI)
	find_package(WiringPi REQUIRED)
	include_directories(${WIRINGPI_INCLUDE_DIRS})
endif(USE_WIRINGPI)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS locale)
include_directories(${Boost_INCLUDE_DIRS})
find_package(PahoMqtt REQUIRED)
include_directories(${LIBMQTT_INCLUDE_DIRS})

# add the executable
add_executable(MQTTClient ${HEADER_FILE_C} ${HEADER_FILE_CPP}  ${SOURCE_FILE_C} ${SOURCE_FILE_CPP} ./main.cpp)

if(USE_WIRINGPI)
	target_link_libraries(MQTTClient ${WIRINGPI_LIBRARIES})
endif(USE_WIRINGPI)
target_link_libraries(MQTTClient ${Boost_LOCALE_LIBRARY})
target_link_libraries(MQTTClient ${LIBMQTT_LIBRARIES})
target_link_libraries(MQTTClient ${CMAKE_THREAD_LIBS_INIT})

