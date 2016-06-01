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

#define NHR_CONNECT_RETRY_DELAY 200
#define NHR_CONNECT_ATTEMPS 5

#ifndef NHR_OS_WINDOWS
#define	WSAEWOULDBLOCK	EAGAIN
#define	WSAEINPROGRESS	EINPROGRESS
#endif

static void nhr_request_work_th_func(void * user_object)
{
	_nhr_request * r = (_nhr_request *)user_object;
	while (r->command < COMMAND_END)
	{
		nhr_mutex_lock(r->work_mutex);
		switch (r->command)
		{
			case COMMAND_CONNECT_TO_HOST: nhr_request_connect_to_host(r); break;
			case COMMAND_SEND_RAW_REQUEST: nhr_request_send_raw_request(r); break;
			case COMMAND_WAIT_RAW_RESPONCE: nhr_request_wait_raw_responce(r); break;
			default: break;
		}
		nhr_mutex_unlock(r->work_mutex);

		switch (r->command)
		{
			case COMMAND_INFORM_RESPONCE:
				if (r->on_recvd_responce) r->on_recvd_responce(r, r->responce);
				r->command = COMMAND_END;
				break;
			case COMMAND_INFORM_ERROR:
				if (r->on_error) r->on_error(r, r->error_code);
				r->command = COMMAND_END;
				break;
			default: break;
		}

		nhr_thread_sleep(5);
	}

	nhr_request_close(r);
	r->work_thread = NULL;
	nhr_request_delete(r);
}

// c89 standard
#define NHR_RECV_BUFF_SIZE (1 << 16)

nhr_bool nhr_request_recv(_nhr_request * r)
{
	int is_reading = 1, error_number = -1, len = -1;
	char buff[NHR_RECV_BUFF_SIZE];

	while (is_reading)
	{
		len = (int)recv(r->socket, buff, NHR_RECV_BUFF_SIZE, 0);
#if defined(RWS_OS_WINDOWS)
		error_number = WSAGetLastError();
#else
		error_number = errno;
#endif
		if (len > 0)
		{
			if (r->responce) nhr_response_append(r->responce, buff, len);
			else r->responce = nhr_response_create(buff, len);
		}
		else
		{
			is_reading = 0;
		}
	}

	if (error_number != WSAEWOULDBLOCK && error_number != WSAEINPROGRESS)
	{
		nhr_request_close(r);
		return nhr_false;
	}
	return nhr_true;
}

void nhr_request_wait_raw_responce(_nhr_request * r)
{
	if (nhr_request_recv(r))
	{
		if (nhr_response_is_finished(r->responce))
		{
			r->command = COMMAND_INFORM_RESPONCE;
		}
	}
	else
	{
		if (r)
		{
			r->command = COMMAND_INFORM_RESPONCE;
		}
		else
		{
			r->error_code = nhr_error_code_failed_connect_to_host;
			r->command = COMMAND_INFORM_ERROR;
		}
	}
}

size_t nhr_request_create_header_GET(_nhr_request * r, char ** header)
{
	size_t buff_size = 0;
	size_t writed = 0;
	char * buff = NULL;

	buff_size = strlen(r->path);
	buff_size += strlen(r->host);
	if (r->http_headers) buff_size += strlen(r->http_headers);
	if (r->parameters) buff_size += strlen(r->parameters);
	buff_size += 1 << 8; // extra size for formatting strings

	buff = (char *)nhr_malloc(buff_size);

	if (r->parameters) writed = nhr_sprintf(buff, buff_size, "%s %s?%s HTTP/%s\r\n", k_nhr_GET, r->path, r->parameters, k_nhr_request_http_ver);
	else writed = nhr_sprintf(buff, buff_size, "%s %s HTTP/%s\r\n", k_nhr_GET, r->path, k_nhr_request_http_ver);

	if (r->port == 80) writed += nhr_sprintf(buff + writed, buff_size - writed, "Host: %s\r\n", r->host);
	else writed += nhr_sprintf(buff + writed, buff_size - writed, "Host: %s:%i\r\n", r->host, (int)r->port);

	if (r->http_headers)
	{
		writed += nhr_sprintf(buff + writed, buff_size - writed, "%s\r\n\r\n", r->http_headers);
	}
	else
	{
		memcpy(buff + writed, k_nhr_CRLF, k_nhr_CRLF_length);
		writed += k_nhr_CRLF_length;
	}

	*header = buff;
	return writed;
}

void nhr_request_send_raw_request(_nhr_request * r)
{
	char * header = NULL;
	size_t header_size = 0;
	switch (r->method)
	{
		case nhr_method_GET:
			header_size = nhr_request_create_header_GET(r, &header);
			break;

		default:
			assert(0); //TODO: unsupported method
			break;
	}

	if (nhr_request_send_buffer(r, header, header_size))
	{
		r->command = COMMAND_WAIT_RAW_RESPONCE;
	}
	else
	{
		nhr_request_close(r);
		r->error_code = nhr_error_code_failed_connect_to_host;
		r->command = COMMAND_INFORM_ERROR;
	}
	nhr_free(header);
}

nhr_bool nhr_request_send_buffer(_nhr_request * r, const void * data, const size_t data_size)
{
	int sended = -1, error_number = -1;
	r->error_code = nhr_error_code_none;

	//errno = -1;
#if defined(RWS_OS_WINDOWS)
	sended = send(r->socket, (const char *)data, data_size, 0);
	error_number = WSAGetLastError();
#else
	sended = (int)send(r->socket, data, (int)data_size, 0);
	error_number = errno;
#endif

	if (sended > 0) return nhr_true;

	if (error_number > 0) return nhr_false;

	return nhr_true;
}

struct addrinfo * nhr_request_connect_getaddr_info(_nhr_request * r)
{
	struct addrinfo hints;
	char portstr[16];
	struct addrinfo * result = NULL;
	int ret = 0, retry_number = 0, last_ret = 0;
#if defined(NHR_OS_WINDOWS)
	WSADATA wsa;
	memset(&wsa, 0, sizeof(WSADATA));
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		r->error_code = nhr_error_code_failed_connect_to_host;
		r->command = COMMAND_INFORM_ERROR;
		return NULL;
	}
#endif

	nhr_sprintf(portstr, 16, "%i", r->port);
	while (++retry_number < NHR_CONNECT_ATTEMPS)
	{
		result = NULL;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		ret = getaddrinfo(r->host, portstr, &hints, &result);
		if (ret == 0 && result) return result;

		if (ret != 0) last_ret = ret;
		if (result) freeaddrinfo(result);
		nhr_thread_sleep(NHR_CONNECT_RETRY_DELAY);
	}

#if defined(NHR_OS_WINDOWS)
	WSACleanup();
#endif

	r->error_code = nhr_error_code_failed_connect_to_host;
	r->command = COMMAND_INFORM_ERROR;
	return NULL;
}

void nhr_request_connect_to_host(_nhr_request * r)
{
	struct addrinfo * result = NULL;
	struct addrinfo * p = NULL;
	nhr_socket_t sock = NHR_INVALID_SOCKET;
	int retry_number = 0;
#if defined(NHR_OS_WINDOWS)
	unsigned long iMode = 0;
#endif

	result = nhr_request_connect_getaddr_info(r);
	if (!result) return;

	while ((++retry_number < NHR_CONNECT_ATTEMPS) && (sock == NHR_INVALID_SOCKET))
	{
		for (p = result; p != NULL; p = p->ai_next)
		{
			sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (sock != NHR_INVALID_SOCKET)
			{
				nhr_request_set_option(sock, SO_ERROR, 1); // When an error occurs on a socket, set error variable so_error and notify process
				nhr_request_set_option(sock, SO_KEEPALIVE, 1); // Periodically test if connection is alive

				if (connect(sock, p->ai_addr, p->ai_addrlen) == 0)
				{
					r->socket = sock;
#if defined(NHR_OS_WINDOWS)
					// If iMode != 0, non-blocking mode is enabled.
					iMode = 1;
					ioctlsocket(r->socket, FIONBIO, &iMode);
#else
					fcntl(r->socket, F_SETFL, O_NONBLOCK);
#endif
					break;
				}
				NHR_SOCK_CLOSE(sock);
			}
		}
		if (sock == NHR_INVALID_SOCKET) nhr_thread_sleep(NHR_CONNECT_RETRY_DELAY);
	}

	freeaddrinfo(result);

	if (r->socket == NHR_INVALID_SOCKET)
	{
#if defined(NHR_OS_WINDOWS)
		WSACleanup();
#endif
		r->error_code = nhr_error_code_failed_connect_to_host;
		r->command = COMMAND_INFORM_ERROR;
	}
	else
	{
		r->command = COMMAND_SEND_RAW_REQUEST;
	}
}

nhr_bool nhr_request_create_start_work_thread(_nhr_request * r)
{
	r->command = COMMAND_NONE;
	r->work_thread = nhr_thread_create(&nhr_request_work_th_func, r);
	if (r->work_thread)
	{
		r->command = COMMAND_CONNECT_TO_HOST;
		return nhr_true;
	}
	return nhr_false;
}

void nhr_request_close(_nhr_request * r)
{
	if (r->socket != NHR_INVALID_SOCKET)
	{
		NHR_SOCK_CLOSE(r->socket);
		r->socket = NHR_INVALID_SOCKET;
#if defined(NHR_OS_WINDOWS)
		WSACleanup();
#endif
	}
}

void nhr_request_delete(_nhr_request * r)
{
	nhr_request_close(r);

	nhr_string_delete_clean(&r->scheme);
	nhr_string_delete_clean(&r->host);
	nhr_string_delete_clean(&r->path);

	nhr_string_delete_clean(&r->http_headers);
	nhr_string_delete_clean(&r->parameters);

	nhr_mutex_delete(r->work_mutex);
	nhr_mutex_delete(r->send_mutex);

	nhr_response_delete(r->responce);

	nhr_free(r);
}

void nhr_request_set_option(nhr_socket_t s, int option, int value)
{
	setsockopt(s, SOL_SOCKET, option, (char *)&value, sizeof(int));
}
