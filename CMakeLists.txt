cmake_minimum_required(VERSION 3.6)

set(CMAKE_SUPPRESS_REGENERATION true)

project(FluidSimulation)

message(STATUS "CMAKE BINRARY DIR :" ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE BINRARY DIR :" ${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "CMAKE_CXX_COMPILER_ID :" ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_SYSTEM_VERSION :" ${CMAKE_SYSTEM_VERSION})
message(STATUS "CMAKE_C_COMPILER_ID :" ${CMAKE_C_COMPILER_ID})

if(COMPILER_INFO)
message(STATUS "MSVC12 :" ${MSVC12})
message(STATUS "MSVC14 :" ${MSVC14})
message(STATUS "APPLE :" ${APPLE})
endif()

add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

if(WIN32)
	set(CMAKE_CXX_FLAGS "-MP8 /EHsc /W3")
else()
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ -g3 -Wall -O0")
endif()

find_package(OpenGL REQUIRED)

add_subdirectory(ParticleTracking)