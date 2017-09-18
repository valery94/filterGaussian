cmake_minimum_required (VERSION 3.2 FATAL_ERROR) 

set(PROJECT_NAME libraryFilterGaussian)

set(SOURCE_ROOT src/)
set(INCLUDE_ROOT include/)
set(THIRDPARTY_ROOT ../thirdparty)

project(${PROJECT_NAME})

include_directories(${INCLUDE_ROOT})
include_directories(${THIRDPARTY_ROOT}/easylogging)
include_directories(${THIRDPARTY_ROOT}/rapidjson)

include_directories($ENV{AMDAPPSDKROOT}/include)
link_directories($ENV{AMDAPPSDKROOT}/lib/x86_64/)

file(GLOB HEADERS ${INCLUDE_ROOT}/*.h)
file(GLOB SOURCES ${SOURCE_ROOT}/*.cpp)
file(GLOB SOURCES_CPU ${SOURCE_ROOT}/cpu/*.cpp)
file(GLOB SOURCES_GPU ${SOURCE_ROOT}/gpu/*.cpp)

add_library(filter STATIC 
	${HEADERS}
	${SOURCES}
	${SOURCES_CPU}
	${SOURCES_GPU})
