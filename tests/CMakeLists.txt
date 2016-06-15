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

if(NHR_OPT_APPVEYOR_CI)
	add_definitions(-DNHR_APPVEYOR_CI)
endif()



add_executable(test_libnhr_creation test_libnhr_creation.c)
target_link_libraries(test_libnhr_creation nhr)
add_test(test_libnhr_creation test_libnhr_creation)

add_executable(test_libnhr_creation_static test_libnhr_creation.c)
target_link_libraries(test_libnhr_creation_static nhr_static)
add_test(test_libnhr_creation_static test_libnhr_creation_static)



add_executable(test_gz_creation test_gz_creation.c)
target_link_libraries(test_gz_creation nhr)
add_test(test_gz_creation test_gz_creation)

add_executable(test_gz_creation_static test_gz_creation.c)
target_link_libraries(test_gz_creation_static nhr_static)
add_test(test_gz_creation_static test_gz_creation_static)



add_executable(test_get test_get.c)
target_link_libraries(test_get nhr)
add_test(test_get test_get)

add_executable(test_get_static test_get.c)
target_link_libraries(test_get_static nhr_static)
add_test(test_get_static test_get_static)



if(NHR_HAVE_PTHREAD_H)
	target_link_libraries(test_libnhr_creation_static pthread)
	target_link_libraries(test_gz_creation_static pthread)
	target_link_libraries(test_get_static pthread)
endif()

if(NHR_LINK_ZLIB)
	target_link_libraries(test_libnhr_creation_static ${ZLIB_LIBRARIES})
	target_link_libraries(test_gz_creation_static ${ZLIB_LIBRARIES})
	target_link_libraries(test_get_static ${ZLIB_LIBRARIES})
endif()

#install(TARGETS test_libnhr_creation DESTINATION bin)

