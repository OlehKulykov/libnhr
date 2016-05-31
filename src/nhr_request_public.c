/*
 *   Copyright (c) 2016 Kulykov Oleh <info@resident.name>
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
#include "nhr_memory.h"
#include "nhr_string.h"

#if !defined(NHR_OS_WINDOWS)
void nhr_request_handle_sigpipe(int signal_number)
{
	printf("\nlibnhr handle sigpipe %i", signal_number);
	return;
}
#endif

nhr_request nhr_request_create(void)
{
	_nhr_request * r = (_nhr_request *)nhr_malloc_zero(sizeof(struct _nhr_request_struct));

#if !defined(NHR_OS_WINDOWS)
	signal(SIGPIPE, nhr_request_handle_sigpipe);
#endif

	r->socket = NHR_INVALID_SOCKET;
	r->command = COMMAND_NONE;
	r->work_mutex = nhr_mutex_create_recursive();

	return r;
}

void nhr_request_set_url(nhr_request request,
						 const char * scheme,
						 const char * host,
						 const char * path,
						 const unsigned short port)
{
	_nhr_request * r = (_nhr_request *)request;
	if (!r) return;

	nhr_string_delete(r->scheme);
	r->scheme = nhr_string_copy(scheme);

	nhr_string_delete(r->host);
	r->host = nhr_string_copy(host);

	nhr_string_delete(r->path);
	r->path = nhr_string_copy(path);

	r->port = port;
}

void nhr_request_set_method(nhr_request request, nhr_method method)
{
	_nhr_request * r = (_nhr_request *)request;
	if (r) r->method = method;
}

nhr_bool nhr_request_send(nhr_request request)
{
	_nhr_request * r = (_nhr_request *)request;
	if (!r) return nhr_false;

	if (!r->scheme || !r->host || !r->path || !r->method)
	{
		r->error_code = nhr_error_code_missed_parameter;
		return nhr_false;
	}

	r->error_code = nhr_error_code_none;

	return nhr_request_create_start_work_thread(r);
}

void nhr_request_set_on_recvd_responce(nhr_request request, nhr_on_request_recvd_responce callback)
{
	_nhr_request * r = (_nhr_request *)request;
	if (r) r->on_recvd_responce = callback;
}

void nhr_request_set_on_error(nhr_request request, nhr_on_request_error callback)
{
	_nhr_request * r = (_nhr_request *)request;
	if (r) r->on_error = callback;
}

void nhr_request_add_header_field(nhr_request request, const char * name, const char * value)
{
	_nhr_request * r = (_nhr_request *)request;
	if (!r) return;

	const size_t nameLen = name ? strlen(name) : 0;
	const size_t valueLen = value ? strlen(value) : 0;
	if (nameLen == 0 || valueLen == 0) return;

	const size_t headersLen = r->http_headers ? strlen(r->http_headers) : 0;

	char * headers = nhr_string_extend(r->http_headers, nameLen + valueLen + 4);
	nhr_string_delete(r->http_headers);
	r->http_headers = headers;
	if (headersLen > 0)
	{
		headers += headersLen;
		headers += nhr_sprintf(headers, nameLen + valueLen, "\r\n%s: %s", name, value);
	}
	else
	{
		headers += nhr_sprintf(headers, nameLen + valueLen, "%s: %s", name, value);
	}
	*headers = 0;
}

void nhr_request_add_parameter(nhr_request request, const char * name, const char * value)
{
	_nhr_request * r = (_nhr_request *)request;
	if (!r) return;

	const size_t nameLen = name ? strlen(name) : 0;
	const size_t valueLen = value ? strlen(value) : 0;
	if (nameLen == 0 || valueLen == 0) return;

	assert(r->method); //!!! set method

	const size_t parametersLen = r->parameters ? strlen(r->parameters) : 0;
	char * parameters = nhr_string_extend(r->parameters, nameLen + valueLen + 4);
	nhr_string_delete(r->parameters);
	r->parameters = parameters;
	if (parametersLen > 0)
	{
		parameters += parametersLen;
		parameters += nhr_sprintf(parameters, nameLen + valueLen, "&%s=%s", name, value);
	}
	else
	{
		parameters += nhr_sprintf(parameters, nameLen + valueLen, "%s=%s", name, value);
	}
	*parameters = 0;
}

nhr_error_code nhr_request_get_error_code(nhr_request request)
{
	_nhr_request * r = (_nhr_request *)request;
	return r ? r->error_code : nhr_error_code_none;
}
