/*
 *   Copyright (c) 2016 - 2017 Kulykov Oleh <info@resident.name>
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


#include "nhr_request.h"
#include "nhr_common.h"
#include "nhr_memory.h"
#include "nhr_string.h"

#if !defined(NHR_OS_WINDOWS)
#include <signal.h>
#endif

#if !defined(NHR_OS_WINDOWS)
void nhr_request_handle_sigpipe(int signal_number) {
	printf("\nlibnhr handle sigpipe %i", signal_number);
	return;
}
#endif

nhr_request nhr_request_create(void) {
	_nhr_request * r = (_nhr_request *)nhr_malloc_zero(sizeof(struct _nhr_request_struct));

#if !defined(NHR_OS_WINDOWS)
	signal(SIGPIPE, nhr_request_handle_sigpipe);
#endif

	r->socket = NHR_INVALID_SOCKET;
	r->command = NHR_COMMAND_NONE;
	r->work_mutex = nhr_mutex_create_recursive();
	r->command_mutex = nhr_mutex_create_recursive();
	r->timeout = 30;

	return r;
}

void nhr_request_set_url(nhr_request request,
						 const char * scheme,
						 const char * host,
						 const char * path,
						 const unsigned short port) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		nhr_string_delete(r->scheme);
		r->scheme = nhr_string_copy(scheme);
		
		nhr_string_delete(r->host);
		r->host = nhr_string_copy(host);
		
		nhr_string_delete(r->path);
		r->path = nhr_string_copy(path);
		
		r->port = port;
	}
}

void nhr_request_set_method(nhr_request request, nhr_method method) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		r->method = method;
	}
}

nhr_bool nhr_request_send(nhr_request request) {
	_nhr_request * r = (_nhr_request *)request;
	if (!r) {
		return nhr_false;
	}

	if (!r->scheme || !r->host || !r->path || !r->method || !r->on_recvd_responce || !r->on_error) {
		r->error_code = nhr_error_code_missed_parameter;
		return nhr_false;
	}

	r->error_code = nhr_error_code_none;
	return nhr_request_create_start_work_thread(r);
}

void nhr_request_set_on_recvd_responce(nhr_request request, nhr_on_request_recvd_responce callback) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		r->on_recvd_responce = callback;
	}
}

void nhr_request_set_on_error(nhr_request request, nhr_on_request_error callback) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		r->on_error = callback;
	}
}

void nhr_request_add_header_field(nhr_request request, const char * name, const char * value) {
	_nhr_request * r = (_nhr_request *)request;
	size_t name_len = 0, value_len = 0;
	_nhr_map_node * last = NULL;
	if (!r) {
		return;
	}

	name_len = name ? strlen(name) : 0;
	value_len = value ? strlen(value) : 0;
	if (name_len == 0 || value_len == 0) {
		return;
	}
	if (!r->http_headers) {
		r->http_headers = nhr_map_create();
	}
	
	last = nhr_map_append(r->http_headers);
	last->key = nhr_string_copy(name);
	last->value.string = nhr_string_copy(value);
	last->value_type = NHR_MAP_VALUE_STRING;
#if defined(NHR_GZIP)
	if (strcmp(value, k_nhr_gzip) == 0) {
		r->is_gziped = nhr_true;
	}
	if (strcmp(value, k_nhr_deflate) == 0) {
		r->is_deflated = nhr_true;
	}
#endif
}

void nhr_request_add_parameter(nhr_request request, const char * name, const char * value) {
	_nhr_request * r = (_nhr_request *)request;
	size_t name_len = 0, value_len = 0;
	_nhr_map_node * last = NULL;
	if (!r) {
		return;
	}

	name_len = name ? strlen(name) : 0;
	value_len = value ? strlen(value) : 0;
	if (name_len == 0 || value_len == 0) {
		return;
	}

	assert(r->method); //!!! set method

	if (!r->parameters) {
		r->parameters = nhr_map_create();
	}
	
	last = nhr_map_append(r->parameters);
	last->key = nhr_string_copy(name);
	last->value.string = nhr_string_copy(value);
	last->value_type = NHR_MAP_VALUE_STRING;
}

nhr_error_code nhr_request_get_error_code(nhr_request request) {
	_nhr_request * r = (_nhr_request *)request;
	return r ? r->error_code : nhr_error_code_none;
}

void nhr_request_set_user_object(nhr_request request, void * user_object) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		r->user_object = user_object;
	}
}

void* nhr_request_get_user_object(nhr_request request) {
	_nhr_request * r = (_nhr_request *)request;
	return r ? r->user_object : NULL;
}

void nhr_request_set_timeout(nhr_request request, const unsigned int seconds) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		r->timeout = (time_t)seconds;
	}
}

unsigned int nhr_request_get_timeout(nhr_request request) {
	_nhr_request * r = (_nhr_request *)request;
	return r ? (unsigned int)r->timeout	: 0;
}

void nhr_request_cancel(nhr_request request) {
	_nhr_request * r = (_nhr_request *)request;
	if (r) {
		if (r->work_thread) {
			nhr_request_set_command(r, NHR_COMMAND_END);
			return; // automaticaly deleted before exit work thread function.
		}
		
		nhr_request_close(r);
		nhr_request_delete(r);
	}
}
