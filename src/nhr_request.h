/*
 *   Copyright (c) 2016 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#ifndef __NHR_REQUEST_H__
#define __NHR_REQUEST_H__ 1

#include "../libnhr.h"
#include "nhr_common.h"
#include "nhr_thread.h"
#include "nhr_memory.h"
#include "nhr_string.h"
#include "nhr_response.h"
#include "nhr_map.h"

#if defined(NHR_OS_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <assert.h>
#include <errno.h>
#include <time.h>

#if defined(NHR_OS_WINDOWS)
typedef SOCKET nhr_socket_t;
#define NHR_INVALID_SOCKET INVALID_SOCKET
#define NHR_SOCK_CLOSE(sock) closesocket(sock)
#else
typedef int nhr_socket_t;
#define NHR_INVALID_SOCKET -1
#define NHR_SOCK_CLOSE(sock) close(sock)
#endif

#if !defined(NHR_NO_POST) // POST functionality
#if !defined(NHR_NO_POST_DATA) // POST DATA functionality

#define NHR_POST_BOUNDARY_LEN 12

#endif // end POST DATA functionality
#endif // end POST functionality

struct nhr_request_struct {
	unsigned short port;

	nhr_socket_t socket;
	char * scheme;
	char * host;
	char * path;
	nhr_method method;
	
	nhr_thread work_thread;

	int command;
	time_t timeout;
	time_t last_time; // last succesful action time

	nhr_error_code error_code;

	void * user_object;

	_nhr_map_node * http_headers;
	_nhr_map_node * parameters;

	nhr_on_request_recvd_response on_recvd_response;
	nhr_on_request_error on_error;

	nhr_mutex work_mutex;
	nhr_mutex command_mutex;

	nhr_response response;

#if defined(NHR_GZIP)
	nhr_bool is_gziped;
	nhr_bool is_deflated;
#endif
    
#if !defined(NHR_NO_POST) 
#if !defined(NHR_NO_POST_DATA)
    char * boundary;
    nhr_bool is_have_data_parameter;
#endif
#endif

};

nhr_bool nhr_request_send_buffer(nhr_request r, const void * data, const size_t data_size);

void nhr_request_start_waiting_raw_response(nhr_request r);

void nhr_request_wait_raw_response(nhr_request r);

void nhr_request_send_raw_request(nhr_request r);

struct addrinfo * nhr_request_connect_getaddr_info(nhr_request r);

void nhr_request_connect_to_host(nhr_request r);

nhr_bool nhr_request_create_start_work_thread(nhr_request r);

void nhr_request_close(nhr_request r);

void nhr_request_delete(nhr_request r);

void nhr_request_set_option(nhr_socket_t s, int option, int value);

nhr_bool nhr_request_check_timeout(nhr_request r);

void nhr_request_set_command(nhr_request r, const int command);

int nhr_request_get_command(nhr_request r);

#define NHR_COMMAND_IDLE -1
#define NHR_COMMAND_NONE 0
#define NHR_COMMAND_CONNECT_TO_HOST 1
#define NHR_COMMAND_SEND_RAW_REQUEST 2
#define NHR_COMMAND_START_WAITING_RAW_RESPONSE 3
#define NHR_COMMAND_WAIT_RAW_RESPONSE 4

#define NHR_COMMAND_INFORM_RESPONSE 5
#define NHR_COMMAND_INFORM_ERROR 6

#define NHR_COMMAND_END 9999


// Methods

#if !defined(NHR_NO_GET) || !defined(NHR_NO_POST) // common functionality

size_t nhr_request_map_strings_length(_nhr_map_node * map, const size_t iteration_increment);

char * nhr_request_http_headers(_nhr_map_node * map, size_t * length);

char * nhr_request_url_encoded_parameters(_nhr_map_node * map, size_t * length);

#endif // end of the common functionality


#if !defined(NHR_NO_GET) // GET functionality

char * nhr_request_create_header_GET(nhr_request r, size_t * header_size);

#endif // end of the GET functionality


#if !defined(NHR_NO_POST) // POST functionality

#if !defined(NHR_NO_POST_DATA) // POST DATA functionality

void nhr_request_generate_new_boundary(nhr_request r);

void * nhr_request_create_data_parameters_POST(nhr_request r, size_t * parameters_len);

#endif // end POST DATA functionality

char * nhr_request_create_header_POST(nhr_request r, size_t * header_size);

#endif // end POST functionality

#endif


// request/response body
// https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
