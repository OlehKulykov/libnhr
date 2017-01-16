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

#define NHR_CONNECT_RETRY_DELAY 200
#define NHR_CONNECT_ATTEMPS 5
#define NHR_WORK_THREAD_DELAY 5

#ifndef NHR_OS_WINDOWS
#define	WSAEWOULDBLOCK	EAGAIN
#define	WSAEINPROGRESS	EINPROGRESS
#endif

static void nhr_request_work_th_func(void * user_object) {
	_nhr_request * r = (_nhr_request *)user_object;

	while (nhr_request_get_command(r) < NHR_COMMAND_END) {
		nhr_mutex_lock(r->work_mutex);
		switch (nhr_request_get_command(r)) {
			case NHR_COMMAND_CONNECT_TO_HOST: nhr_request_connect_to_host(r); break;
			case NHR_COMMAND_SEND_RAW_REQUEST: nhr_request_send_raw_request(r); break;
			case NHR_COMMAND_START_WAITING_RAW_RESPONCE: nhr_request_start_waiting_raw_responce(r); break;
			case NHR_COMMAND_WAIT_RAW_RESPONCE: nhr_request_wait_raw_responce(r); break;
			default: break;
		}
		nhr_mutex_unlock(r->work_mutex);

		switch (nhr_request_get_command(r)) {
			case NHR_COMMAND_INFORM_RESPONCE:
				nhr_request_set_command(r,NHR_COMMAND_END);
				if (r->on_recvd_responce) {
					r->on_recvd_responce(r, r->responce);
				}
				break;
			case NHR_COMMAND_INFORM_ERROR:
				nhr_request_set_command(r, NHR_COMMAND_END);
				if (r->on_error) {
					r->on_error(r, r->error_code);
				}
				break;
			default: break;
		}

		nhr_thread_sleep(NHR_WORK_THREAD_DELAY);
	}

	r->work_thread = NULL;
	nhr_request_close(r);
	nhr_request_delete(r);
}

// c89 standard
#define NHR_RECV_BUFF_SIZE (1 << 16)

nhr_bool nhr_request_recv(_nhr_request * r) {
	int error_number = -1, len = -1;
	char buff[NHR_RECV_BUFF_SIZE];

	do {
		len = (int)recv(r->socket, buff, NHR_RECV_BUFF_SIZE, 0);
#if defined(NHR_OS_WINDOWS)
		error_number = WSAGetLastError();
#else
		error_number = errno;
#endif
		if (len > 0) {
			r->last_time = time(NULL);
			if (r->responce) {
				nhr_response_append(r->responce, buff, len);
			} else {
				r->responce = nhr_response_create(buff, len);
			}
		}
	} while (len > 0);

	if (error_number != WSAEWOULDBLOCK && error_number != WSAEINPROGRESS) {
		nhr_request_close(r);
		return nhr_false;
	}
	return nhr_true;
}

void nhr_request_wait_raw_responce(_nhr_request * r) {
	nhr_bool is_finished = nhr_false;
	if (nhr_request_recv(r)) {
		is_finished = r->responce ? r->responce->is_finished : nhr_false;
		if (is_finished) {
			nhr_request_set_command(r, NHR_COMMAND_INFORM_RESPONCE);
		}
		if (!nhr_request_check_timeout(r)) {
			return; // error already exists
		}
	} else if (r) {
		nhr_request_set_command(r, NHR_COMMAND_INFORM_RESPONCE);
	} else {
		r->error_code = nhr_error_code_failed_connect_to_host;
		nhr_request_set_command(r, NHR_COMMAND_INFORM_ERROR);
	}
}

void nhr_request_start_waiting_raw_responce(_nhr_request * r) {
	r->last_time = time(NULL);
	nhr_request_set_command(r, NHR_COMMAND_WAIT_RAW_RESPONCE);
}

void nhr_request_send_raw_request(_nhr_request * r) {
	char * header = NULL;
	size_t header_size = 0;
	switch (r->method) {
#if !defined(NHR_NO_GET)
		case nhr_method_GET: header = nhr_request_create_header_GET(r, &header_size); break;
#endif

#if !defined(NHR_NO_POST)
		case nhr_method_POST: header = nhr_request_create_header_POST(r, &header_size); break;
#endif

		default:
			assert(0); //TODO: unsupported method
			break;
	}

	if (nhr_request_send_buffer(r, header, header_size)) {
		nhr_request_set_command(r, NHR_COMMAND_START_WAITING_RAW_RESPONCE);
	} else {
		nhr_request_close(r);
		if (nhr_request_check_timeout(r)) {
			r->error_code = nhr_error_code_failed_connect_to_host;
			nhr_request_set_command(r, NHR_COMMAND_INFORM_ERROR);
		}
	}
	nhr_free(header);
}

nhr_bool nhr_request_send_buffer(_nhr_request * r, const void * data, const size_t data_size) {
	int sended = -1, error_number = -1;
	r->error_code = nhr_error_code_none;

#if defined(NHR_OS_WINDOWS)
	sended = send(r->socket, (const char *)data, data_size, 0);
	error_number = WSAGetLastError();
#else
	sended = (int)send(r->socket, data, (int)data_size, 0);
	error_number = errno;
#endif

	if (sended > 0) {
		return nhr_true;
	}
	if (error_number > 0) {
		return nhr_false;
	}
	return nhr_true;
}

struct addrinfo * nhr_request_connect_getaddr_info(_nhr_request * r) {
	struct addrinfo hints;
	char portstr[12];
	struct addrinfo * result = NULL;
	int ret = 0, retry_number = 0, last_ret = 0;
#if defined(NHR_OS_WINDOWS)
	WSADATA wsa;
	memset(&wsa, 0, sizeof(WSADATA));
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		r->error_code = nhr_error_code_failed_connect_to_host;
		nhr_request_set_command(r, NHR_COMMAND_INFORM_ERROR);
		return NULL;
	}
#endif

	nhr_sprintf(portstr, 12, "%i", r->port);
	while (++retry_number < NHR_CONNECT_ATTEMPS) {
		result = NULL;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		ret = getaddrinfo(r->host, portstr, &hints, &result);
		if (ret == 0 && result) {
			return result;
		}

		if (ret != 0) {
			last_ret = ret;
		}
		if (result) {
			freeaddrinfo(result);
		}
		nhr_thread_sleep(NHR_CONNECT_RETRY_DELAY);
	}

#if defined(NHR_OS_WINDOWS)
	WSACleanup();
#endif

	r->error_code = nhr_error_code_failed_connect_to_host;
	nhr_request_set_command(r, NHR_COMMAND_INFORM_ERROR);
	return NULL;
}

void nhr_request_connect_to_host(_nhr_request * r) {
	struct addrinfo * result = NULL;
	struct addrinfo * p = NULL;
	nhr_socket_t sock = NHR_INVALID_SOCKET;
	int retry_number = 0;
#if defined(NHR_OS_WINDOWS)
	unsigned long iMode = 0;
#endif

	r->last_time = time(NULL);
	result = nhr_request_connect_getaddr_info(r);
	if (!nhr_request_check_timeout(r)) {
		return; // error already exists
	}
	if (!result) {
		return; // error already exists
	}

	while ((++retry_number < NHR_CONNECT_ATTEMPS) && (sock == NHR_INVALID_SOCKET)) {
		for (p = result; p != NULL; p = p->ai_next) {
			r->last_time = time(NULL);
			sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (sock != NHR_INVALID_SOCKET) {
				nhr_request_set_option(sock, SO_ERROR, 1); // When an error occurs on a socket, set error variable so_error and notify process
				nhr_request_set_option(sock, SO_KEEPALIVE, 1); // Periodically test if connection is alive

				if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
					r->socket = sock;
#if defined(NHR_OS_WINDOWS)
					iMode = 1; // If iMode != 0, non-blocking mode is enabled.
					ioctlsocket(r->socket, FIONBIO, &iMode);
#else
					fcntl(r->socket, F_SETFL, O_NONBLOCK);
#endif
					break;
				}
				NHR_SOCK_CLOSE(sock);
			}
			if (!nhr_request_check_timeout(r)) {
				return; // error already exists
			}
		}
		if (sock == NHR_INVALID_SOCKET) {
			nhr_thread_sleep(NHR_CONNECT_RETRY_DELAY);
		}
	}

	freeaddrinfo(result);

	if (r->socket == NHR_INVALID_SOCKET) {
#if defined(NHR_OS_WINDOWS)
		WSACleanup();
#endif
		r->error_code = nhr_error_code_failed_connect_to_host;
		nhr_request_set_command(r, NHR_COMMAND_INFORM_ERROR);
	} else {
		nhr_request_set_command(r, NHR_COMMAND_SEND_RAW_REQUEST);
	}
}

nhr_bool nhr_request_create_start_work_thread(_nhr_request * r) {
	nhr_request_set_command(r, NHR_COMMAND_NONE);
	r->work_thread = nhr_thread_create(&nhr_request_work_th_func, r);
	if (r->work_thread) {
		nhr_request_set_command(r, NHR_COMMAND_CONNECT_TO_HOST);
		return nhr_true;
	}
	return nhr_false;
}

void nhr_request_close(_nhr_request * r) {
	if (r->socket != NHR_INVALID_SOCKET) {
		NHR_SOCK_CLOSE(r->socket);
		r->socket = NHR_INVALID_SOCKET;
#if defined(NHR_OS_WINDOWS)
		WSACleanup();
#endif
	}
}

void nhr_request_delete(_nhr_request * r) {
	nhr_request_close(r);

	nhr_string_delete_clean(&r->scheme);
	nhr_string_delete_clean(&r->host);
	nhr_string_delete_clean(&r->path);

	nhr_map_delete_clean(&r->http_headers);
	nhr_map_delete_clean(&r->parameters);

	nhr_mutex_delete(r->work_mutex);
	nhr_mutex_delete(r->command_mutex);

	nhr_response_delete(r->responce);

	nhr_free(r);
}

void nhr_request_set_option(nhr_socket_t s, int option, int value) {
	setsockopt(s, SOL_SOCKET, option, (char *)&value, sizeof(int));
}

nhr_bool nhr_request_check_timeout(_nhr_request * r) {
	if (time(NULL) - r->last_time < r->timeout) {
		return nhr_true;
	}
	r->error_code = nhr_error_code_timeout;
	nhr_request_set_command(r, NHR_COMMAND_INFORM_ERROR);
	return nhr_false;
}

void nhr_request_set_command(_nhr_request * r, const int command) {
	nhr_mutex_lock(r->command_mutex);
	r->command = command;
	nhr_mutex_unlock(r->command_mutex);
}

int nhr_request_get_command(_nhr_request * r) {
	int comand = NHR_COMMAND_NONE;
	nhr_mutex_lock(r->command_mutex);
	comand = r->command;
	nhr_mutex_unlock(r->command_mutex);
	return comand;
}

