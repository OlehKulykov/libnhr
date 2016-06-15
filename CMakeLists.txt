#
#   Copyright (c) 2016 Kulykov Oleh <info@resident.name>
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to deal
#   in the Software without restriction, including without limitation the rights
#   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#   copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#   THE SOFTWARE.
#

cmake_minimum_required(VERSION 2.8)


project(libnhr)


set(PACKAGE "libnhr")
set(CPACK_PACKAGE_NAME "${PACKAGE}")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR "info@resident.name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE} ${PACKAGE_VERSION}")
set(SOVERSION "0.3.2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(VERSION "${CPACK_PACKAGE_VERSION}")


include(CheckLibraryExists)
include(CheckFunctionExists)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)

include(CheckTypeSize)
include(CheckSymbolExists)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# COMMAND LINE OPTIONS
option(NHR_OPT_SHARED "Build shared lib" ON)
option(NHR_OPT_STATIC "Build static lib" ON)
option(NHR_OPT_NO_GET "Build without GET method support" OFF)
option(NHR_OPT_NO_POST "Build without POST method support" OFF)
option(NHR_OPT_NO_CHUNKED "Build without processing responce with chunked transfer encoding" OFF)
option(NHR_OPT_NO_GZIP "Build without gzip encoding/decoding support" OFF)
option(NHR_OPT_TESTS "Build libnhr tests" ON)
option(NHR_OPT_APPVEYOR_CI "Build with appveyor ci" OFF)

# C with -fPIC
check_c_compiler_flag("-fPIC" WITH_FPIC_C)
if(WITH_FPIC_C)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif(WITH_FPIC_C)


# CXX with -fPIC
check_cxx_compiler_flag("-fPIC" WITH_FPIC_CXX)
if(WITH_FPIC_CXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(WITH_FPIC_CXX)


if(WIN32)
	add_definitions(-DWIN32)
	add_definitions(-D_WIN32)
	set(NHR_OS_WINDOWS 1)
endif()


if(MINGW)
	set(NHR_COMPILER_MINGW 1)
endif()


add_definitions(-DCMAKE_BUILD)

if(NHR_OPT_APPVEYOR_CI)
	add_definitions(-DNHR_APPVEYOR_CI)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

message("Looking for zlib...")
find_package(ZLIB)
if (ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIRS})
	message("zlib include dirs: ${ZLIB_INCLUDE_DIRS}")
	message("zlib libraries: ${ZLIB_LIBRARIES}")
else()
	message("zlib not found")
endif()


check_include_file("netdb.h" NHR_HAVE_NETDB_H)
check_include_file("sys/socket.h" NHR_HAVE_SYS_SOCKET_H)
check_include_file("sys/types.h" NHR_HAVE_SYS_TYPES_H)
check_include_file("netinet/tcp.h" NHR_HAVE_NETINET_TCP_H)
check_include_file("unistd.h" NHR_HAVE_UNISTD_H)
check_include_file("fcntl.h" NHR_HAVE_FCNTL_H)
check_include_file("pthread.h" NHR_HAVE_PTHREAD_H)
check_include_file("zlib.h" NHR_HAVE_ZLIB_H)


message(CMAKE_EXTRA_INCLUDE_FILES ": " ${CMAKE_EXTRA_INCLUDE_FILES})

# To check the size of a primitive type:
check_type_size("void*" NHR_SIZEOF_VOIDP_T)


# functions
#check_function_exists(usleep NHR_HAVE_FUNCTION_USLEEP)

#if(NHR_HAVE_PTHREAD_H)
#check_function_exists(pthread_attr_setscope NHR_HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE)
#check_function_exists(pthread_attr_setstacksize NHR_HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE)
#check_function_exists(pthread_cancel NHR_HAVE_FUNCTION_PTHREAD_CANCEL)
#check_function_exists(pthread_setname_np NHR_HAVE_FUNCTION_PTHREAD_SETNAME_NP)
#check_function_exists(pthread_set_name_np NHR_HAVE_FUNCTION_PTHREAD_SET_NAME_NP)
#endif()


if((NOT NHR_HAVE_PTHREAD_H) AND (NOT WIN32))
	message(FATAL_ERROR "Can't build libnhr without any threading support")
endif()


if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
	set(CMAKE_INSTALL_LIBDIR lib)
endif()

# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_BINARY_DIR})


set(LIBNHR_SOURCES src/nhr_common.c
		src/nhr_map.c
		src/nhr_memory.c
		src/nhr_request_private.c
		src/nhr_request_public.c
		src/nhr_response.c
		src/nhr_string.c
		src/nhr_thread.c)


set(NHR_HAVE_ANY_METHOD FALSE)
set(NHR_LINK_ZLIB FALSE)

if (NHR_OPT_NO_GET)
	add_definitions(-DNHR_NO_GET)
else()
	set(NHR_HAVE_ANY_METHOD TRUE)
	list(APPEND LIBNHR_SOURCES src/nhr_request_method_get.c)
endif()

if (NHR_OPT_NO_POST)
	add_definitions(-DNHR_NO_POST)
else()
	set(NHR_HAVE_ANY_METHOD TRUE)
	list(APPEND LIBNHR_SOURCES src/nhr_request_method_post.c)
endif()

if (NHR_HAVE_ANY_METHOD)
	list(APPEND LIBNHR_SOURCES src/nhr_request_method_common.c)
endif()

if (NHR_OPT_NO_CHUNKED)
	add_definitions(-DNHR_NO_CHUNKED)
endif()

if (NHR_OPT_NO_GZIP)
	add_definitions(-DNHR_NO_GZIP)
elseif (NHR_HAVE_ZLIB_H)
	set(NHR_LINK_ZLIB TRUE)
	add_definitions(-DNHR_HAVE_ZLIB_H)
	list(APPEND LIBNHR_SOURCES src/nhr_gz.c)
endif()


set(LIBNHR_HEADERS libnhr.h)


add_definitions(-DNHR_BUILD)


if(NHR_OPT_SHARED)
	add_library(nhr SHARED ${LIBNHR_SOURCES} ${LIBNHR_HEADERS})
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_property(TARGET nhr PROPERTY PREFIX "lib")
	set_property(TARGET nhr PROPERTY IMPORT_PREFIX "lib")
	endif()
endif()

if(NHR_OPT_STATIC)
	add_library(nhr_static STATIC ${LIBNHR_SOURCES} ${LIBNHR_HEADERS})
	set_property(TARGET nhr_static APPEND PROPERTY COMPILE_FLAGS -DLIBNHR_STATIC)
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_target_properties(nhr_static PROPERTIES PREFIX "lib")
	endif()
endif()



if (NHR_HAVE_PTHREAD_H)
	target_link_libraries(nhr pthread)
endif()

if (NHR_LINK_ZLIB)
	target_link_libraries(nhr ${ZLIB_LIBRARIES})
endif()


if(WIN32)
	target_link_libraries(nhr ws2_32)
endif()


install(TARGETS nhr
		DESTINATION lib)

install(TARGETS nhr_static
		DESTINATION lib)

install(FILES libnhr.h 
		DESTINATION include)


#if(NHR_OPT_TESTS)
#	enable_testing()
#	add_subdirectory(tests)
#
#	# This must always be last!
#	include(CPack)
#endif()
