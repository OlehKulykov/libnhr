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
	struct nhr_request_struct * r = (struct nhr_request_struct *)nhr_malloc_zero(sizeof(struct nhr_request_struct));

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
	if (request) {
		nhr_string_delete(request->scheme);
		request->scheme = nhr_string_copy(scheme);
		
		nhr_string_delete(request->host);
		request->host = nhr_string_copy(host);
		
		nhr_string_delete(request->path);
		request->path = nhr_string_copy(path);
		
		request->port = port;
	}
}

void nhr_request_set_method(nhr_request request, nhr_method method) {
	if (request) {
		request->method = method;
	}
}

nhr_bool nhr_request_send(nhr_request request) {
	if (!request) {
		return nhr_false;
	}

	if (!request->scheme || !request->host || !request->path || !request->method || !request->on_recvd_response || !request->on_error) {
		request->error_code = nhr_error_code_missed_parameter;
		return nhr_false;
	}

	request->error_code = nhr_error_code_none;
	return nhr_request_create_start_work_thread(request);
}

void nhr_request_set_on_recvd_response(nhr_request request, nhr_on_request_recvd_response callback) {
	if (request) {
		request->on_recvd_response = callback;
	}
}

void nhr_request_set_on_error(nhr_request request, nhr_on_request_error callback) {
	if (request) {
		request->on_error = callback;
	}
}

void nhr_request_add_header_field(nhr_request request, const char * name, const char * value) {
	size_t name_len = 0, value_len = 0;
	_nhr_map_node * last = NULL;
	if (!request) {
		return;
	}

	name_len = name ? strlen(name) : 0;
	value_len = value ? strlen(value) : 0;
	if (name_len == 0 || value_len == 0) {
		return;
	}
	if (!request->http_headers) {
		request->http_headers = nhr_map_create();
	}
	
	last = nhr_map_append(request->http_headers);
	last->key = nhr_string_copy_len(name, name_len);
	last->value.string = nhr_string_copy_len(value, value_len);
    last->value_size = value_len;
	last->value_type = NHR_MAP_VALUE_STRING;
#if defined(NHR_GZIP)
	if (strcmp(value, k_nhr_gzip) == 0) {
		request->is_gziped = nhr_true;
	}
	if (strcmp(value, k_nhr_deflate) == 0) {
		request->is_deflated = nhr_true;
	}
#endif
}

void nhr_request_add_parameter(nhr_request request, const char * name, const char * value) {
	size_t name_len = 0, value_len = 0;
	_nhr_map_node * last = NULL;
	if (!request) {
		return;
	}

	name_len = name ? strlen(name) : 0;
	value_len = value ? strlen(value) : 0;
	if (name_len == 0 || value_len == 0) {
		return;
	}

	assert(request->method); //!!! set method

	if (!request->parameters) {
		request->parameters = nhr_map_create();
	}
	
	last = nhr_map_append(request->parameters);
	last->key = nhr_string_copy_len(name, name_len);
	last->value.string = nhr_string_copy_len(value, value_len);
    last->value_size = value_len;
	last->value_type = NHR_MAP_VALUE_STRING;
}

void nhr_request_add_data_parameter(nhr_request request, const char * name, const char * file_name, const void * data, const size_t data_size) {
#if !defined(NHR_NO_POST) // POST functionality
#if !defined(NHR_NO_POST_DATA) // POST DATA functionality
    size_t name_len = 0, file_name_len = 0;
    _nhr_map_node * last = NULL;
    if (!request || !data || data_size == 0) {
        return;
    }
    
    name_len = name ? strlen(name) : 0;
    file_name_len = file_name ? strlen(file_name) : 0;
    if (name_len == 0 || file_name_len == 0) {
        return;
    }
    
    assert(request->method); //!!! set method
    
    void * _data = nhr_malloc(data_size);
    if (!_data) {
        return;
    }
    
    memcpy(_data, data, data_size);
    
    if (!request->parameters) {
        request->parameters = nhr_map_create();
    }
    
    last = nhr_map_append(request->parameters);
    last->key = nhr_string_copy_len(name, name_len);
    last->reserved.string = nhr_string_copy_len(file_name, file_name_len);
    last->reserved_size = file_name_len;
    last->reserved_type = NHR_MAP_VALUE_STRING;
    last->value.data = _data;
    last->value_size = data_size;
    last->value_type = NHR_MAP_VALUE_DATA;

	request->is_have_data_parameter = nhr_true;
    if (!request->boundary) {
        nhr_request_generate_new_boundary(request);
    }
#endif // end POST functionality
#endif // end POST DATA functionality
}

nhr_error_code nhr_request_get_error_code(nhr_request request) {
	return request ? request->error_code : nhr_error_code_none;
}

void nhr_request_set_user_object(nhr_request request, void * user_object) {
	if (request) {
		request->user_object = user_object;
	}
}

void* nhr_request_get_user_object(nhr_request request) {
	return request ? request->user_object : NULL;
}

void nhr_request_set_timeout(nhr_request request, const unsigned int seconds) {
	if (request) {
		request->timeout = (time_t)seconds;
	}
}

unsigned int nhr_request_get_timeout(nhr_request request) {
	return request ? (unsigned int)request->timeout	: 0;
}

void nhr_request_cancel(nhr_request request) {
	if (request) {
		if (request->work_thread) {
			nhr_request_set_command(request, NHR_COMMAND_END);
			return; // automaticaly deleted before exit work thread function.
		}
		
		nhr_request_close(request);
		nhr_request_delete(request);
	}
}
