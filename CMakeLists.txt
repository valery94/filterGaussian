cmake_minimum_required (VERSION 3.2 FATAL_ERROR) 

set(PROJECT_NAME gaussianFilter)
set(EXECUTABLE_NAME gauss3D)

set(SOURCE_ROOT gaussClient/src)
set(INCLUDE_ROOT gaussClient/include)
set(THIRDPARTY_ROOT thirdparty)
set(LIBRARY_GAUSS_FOLDER filterLib)
set(LIBRARY_NAME_GAUSS filter)


project(${PROJECT_NAME})

include_directories(${INCLUDE_ROOT})
include_directories(${LIBRARY_GAUSS_FOLDER}/include)
include_directories(${THIRDPARTY_ROOT}/easylogging)
include_directories(${THIRDPARTY_ROOT}/rapidjson) 

include_directories($ENV{AMDAPPSDKROOT}/include)
link_directories($ENV{AMDAPPSDKROOT}/lib/x86_64/)


file(GLOB HEADERS ${INCLUDE_ROOT}/*.h)
source_group("include" FILES ${HEADERS})

file(GLOB SOURCES ${SOURCE_ROOT}/*.cpp)
source_group("src" FILES ${SOURCES})

file(GLOB SOURCES_CPU ${SOURCE_ROOT}/cpu/*.cpp)
source_group("src\\cpu" FILES ${SOURCES_CPU})

file(GLOB SOURCES_GPU ${SOURCE_ROOT}/gpu/*.*)
source_group("src\\gpu" FILES ${SOURCES_GPU})


add_executable(
    ${EXECUTABLE_NAME}
    ${HEADERS}
    ${SOURCES}
    ${SOURCES_CPU}
    ${SOURCES_GPU} 
)

if(WIN32)

message("Making for WINDOWS")
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(LIBRARY OpenCL.lib)
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY})

elseif(UNIX)

message("Making for UNIX")
set(CMAKE_CXX_FLAGS "-std=c++0x")
set(LIBRARY libOpenCL.so)
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY})

endif()

add_subdirectory(${LIBRARY_GAUSS_FOLDER})	
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME_GAUSS})
