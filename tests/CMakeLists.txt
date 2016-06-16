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

include(CheckLibraryExists)

include_directories(${PROJECT_BINARY_DIR})

link_directories(${PROJECT_BINARY_DIR})

remove_definitions(-DCMAKE_BUILD)


add_library(cJSON STATIC cJSON/cJSON.h cJSON/cJSON.c)

if(NHR_OPT_APPVEYOR_CI)
	add_definitions(-DNHR_APPVEYOR_CI)
endif()


if(MSVC)
	# Suppress warning about "deprecated" functions
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)


add_executable(test_libnhr_creation test_libnhr_creation.c)
target_link_libraries(test_libnhr_creation nhr)
add_test(test_libnhr_creation test_libnhr_creation)

add_executable(test_libnhr_creation_static test_libnhr_creation.c)
target_link_libraries(test_libnhr_creation_static nhr_static)
set_property(TARGET test_libnhr_creation_static APPEND PROPERTY COMPILE_FLAGS -DLIBNHR_STATIC)
add_test(test_libnhr_creation_static test_libnhr_creation_static)

if(WIN32)
	target_link_libraries(test_libnhr_creation_static ws2_32)
else()
	target_link_libraries(test_libnhr_creation m)
	target_link_libraries(test_libnhr_creation_static m)
endif()



add_executable(test_gz_creation test_gz_creation.c)
target_link_libraries(test_gz_creation nhr)
add_test(test_gz_creation test_gz_creation)

add_executable(test_gz_creation_static test_gz_creation.c)
target_link_libraries(test_gz_creation_static nhr_static)
set_property(TARGET test_gz_creation_static APPEND PROPERTY COMPILE_FLAGS -DLIBNHR_STATIC)
add_test(test_gz_creation_static test_gz_creation_static)

if(WIN32)
	target_link_libraries(test_gz_creation_static ws2_32)
else()
	target_link_libraries(test_gz_creation m)
	target_link_libraries(test_gz_creation_static m)
endif()



add_executable(test_get test_get.c)
target_link_libraries(test_get nhr)
target_link_libraries(test_get cJSON)
add_test(test_get test_get)

add_executable(test_get_static test_get.c)
target_link_libraries(test_get_static nhr_static)
target_link_libraries(test_get_static cJSON)
set_property(TARGET test_get_static APPEND PROPERTY COMPILE_FLAGS -DLIBNHR_STATIC)
add_test(test_get_static test_get_static)

if(WIN32)
	target_link_libraries(test_get_static ws2_32)
else()
	target_link_libraries(test_get m)
	target_link_libraries(test_get_static m)
endif()



add_executable(test_post test_post.c)
target_link_libraries(test_post nhr)
target_link_libraries(test_post cJSON)
add_test(test_post test_post)

add_executable(test_post_static test_post.c)
target_link_libraries(test_post_static nhr_static)
target_link_libraries(test_post_static cJSON)
set_property(TARGET test_post_static APPEND PROPERTY COMPILE_FLAGS -DLIBNHR_STATIC)
add_test(test_post_static test_post_static)

if(WIN32)
	target_link_libraries(test_post_static ws2_32)
else()
	target_link_libraries(test_post m)
	target_link_libraries(test_post_static m)
endif()


if(NHR_HAVE_PTHREAD_H)
	target_link_libraries(test_libnhr_creation_static pthread)
	target_link_libraries(test_gz_creation_static pthread)
	target_link_libraries(test_get_static pthread)
	target_link_libraries(test_post_static pthread)
endif()

if(NHR_LINK_ZLIB)
	target_link_libraries(test_libnhr_creation_static ${ZLIB_LIBRARIES})
	target_link_libraries(test_gz_creation_static ${ZLIB_LIBRARIES})
	target_link_libraries(test_get_static ${ZLIB_LIBRARIES})
	target_link_libraries(test_post_static ${ZLIB_LIBRARIES})
endif()

#install(TARGETS test_libnhr_creation DESTINATION bin)

