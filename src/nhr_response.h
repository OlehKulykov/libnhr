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


#ifndef __NHR_RESPONCE_H__
#define __NHR_RESPONCE_H__ 1

#include "../libnhr.h"
#include "nhr_common.h"

typedef struct _nhr_response_struct {
	void * body;
	size_t body_len; // stored
	size_t body_size; // allocated

	size_t content_length;

	unsigned short status_code;
	unsigned char transfer_encoding;
	unsigned char content_encoding;

#if !defined(NHR_NO_RECV_CHUNKS)
	nhr_bool is_all_chunks_processed;
#endif

	nhr_bool is_finished; // all data received & processed
} _nhr_response;

void nhr_response_add_body_data(_nhr_response * r, void * data, const size_t data_size);

_nhr_response * nhr_response_create(void * received, const size_t received_len);

void nhr_response_append(_nhr_response * r, void * received, const size_t received_len);

void nhr_response_delete(_nhr_response * r);

//nhr_bool nhr_response_is_finished(_nhr_response * r);

#define NHR_TRANSFER_ENCODING_CHUNKED 1

#define NHR_CONTENT_ENCODING_GZIP 1
#define NHR_CONTENT_ENCODING_DEFLATE 1 << 1

#endif
