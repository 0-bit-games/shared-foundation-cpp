cmake_minimum_required(VERSION 3.22)

project(foundation DESCRIPTION "Foundation is my own \"standard library\" for C++.")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if (WIN32)
	set(CMAKE_CXX_FLAGS "/std:c++20 /W4 /WX /EHsc /wd4291")
	set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-vla-extension")
	set(CMAKE_CXX_FLAGS "-std=c++20")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(NOT LIBRARY_LINK_TYPE)
	if(WIN32)
		set(LIBRARY_LINK_TYPE STATIC)
	else()
		set(LIBRARY_LINK_TYPE SHARED)
	endif()
endif()

add_library(foundation ${LIBRARY_LINK_TYPE}
	src/exceptions/exception.cpp
	src/memory/allocator.cpp
	src/memory/object.cpp
	src/parallelism/mutex.cpp
	src/parallelism/thread.cpp
	src/platform/thread.cpp
	src/serialization/json.cpp
	src/system/endian.cpp
	src/system/environment.cpp
	src/types/date.cpp
	src/types/duration.cpp
	src/types/hashable.cpp
	src/types/null.cpp
	src/types/string.cpp
	src/types/type.cpp
	src/types/unicode.cpp
	src/types/url.cpp
	src/types/uuid.cpp)

set(ARCH_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_PROCESSOR}")

set_target_properties(foundation
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${ARCH_OUTPUT_DIR}/bin")

target_compile_features(foundation PRIVATE cxx_std_20)
