cmake_minimum_required(VERSION 2.6)

project(BusinessCardRecognition)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

file(GLOB	SOURCE_FILE_CPP	./src/*.cpp)
file(GLOB	HEADER_FILE_HPP	./header/*.hpp)

find_package(Threads	REQUIRED)
find_package(OpenCV		REQUIRED)
if(${OpenCV_VERSION} 	VERSION_LESS	2.4.1)
	message(FATAL_ERROR	"OpenCV version is not compatible : ${OpenCV_VERSION}")
endif()
find_package(Boost COMPONENTS locale REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})
find_package(BISON 2.4 REQUIRED)
set(json_SOURCE_DIR ${CMAKE_SOURCE_DIR}/json)
set(json_BINARY_DIR ${CMAKE_BINARY_DIR}/json)
set(BISON_OUTPUT ${json_BINARY_DIR}/json.tab.cc)
set(BISON_DEFINES ${json_BINARY_DIR}/json.tab.hh)
include_directories(${json_SOURCE_DIR} ${json_BINARY_DIR})
if(BISON_FOUND)
	add_custom_command(
		OUTPUT ${BISON_OUTPUT}
		COMMAND ${BISON_EXECUTABLE}
		--defines=${BISON_DEFINES}
		--output=${BISON_OUTPUT}
		${json_SOURCE_DIR}/json.y
		DEPENDS ${json_SOURCE_DIR}/json.y
		COMMENT "Generating json.tab.hh, json.tab.cc (parser)"
	)
endif(BISON_FOUND)
find_package(FLEX 2.5 REQUIRED)
set(FLEX_OUTPUT ${json_BINARY_DIR}/lex.yy.cc)
if(FLEX_FOUND)
	add_custom_command(
		OUTPUT ${FLEX_OUTPUT}
		COMMAND ${FLEX_EXECUTABLE}
		--outfile=${FLEX_OUTPUT}
		${json_SOURCE_DIR}/json.l
		DEPENDS ${json_SOURCE_DIR}/json.l
	COMMENT "Generating lex.yy.cc (lexer)"
	)
	set_source_files_properties(${FLEX_OUTPUT} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-register"    )
endif(FLEX_FOUND)
# add the library
add_library(json ./json/json_st.cc ${BISON_OUTPUT} ${FLEX_OUTPUT})


add_executable(${PROJECT_NAME}
main.cpp
${SOURCE_FILE_CPP}
${HEADER_FILE_HPP}
)
target_link_libraries(	${PROJECT_NAME}		${OpenCV_LIBS}				)
target_link_libraries(	${PROJECT_NAME}		${CMAKE_THREAD_LIBS_INIT}	)
target_link_libraries(	${PROJECT_NAME}		${Boost_LIBRARIES}			)
target_link_libraries(	${PROJECT_NAME}		json						)
