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


#include "nhr_response.h"
#include "nhr_memory.h"
#include "nhr_string.h"
#include "nhr_gz.h"
#include <string.h>
#include <ctype.h>

#if defined(NHR_GZIP)
void nhr_response_ungzip(_nhr_response * r) {
	void * decompressed = NULL;
	size_t decompressed_size = 0;

	if (!r->body || r->body_len <= 0) {
		return;
	}
	
	if (r->content_encoding & NHR_CONTENT_ENCODING_GZIP) {
		decompressed = nhr_gz_decompress(r->body, r->body_len, &decompressed_size, NHR_GZ_METHOD_GZIP);
	} else if (r->content_encoding & NHR_CONTENT_ENCODING_DEFLATE) {
		decompressed = nhr_gz_decompress(r->body, r->body_len, &decompressed_size, NHR_GZ_METHOD_DEFLATE);
	}
	if (decompressed && decompressed_size > 0) {
		nhr_free(r->body);
		r->body = decompressed;
		r->body_len = decompressed_size;
	} else {
		nhr_free(decompressed);
	}
}
#endif

void nhr_response_fix_body_len(_nhr_response * r) {
	if (r->content_length > 0 && r->body_len > r->content_length) {
		// some trash was copyed, ignore it.
		r->body_len = r->content_length;
	}
}

char * nhr_response_find_http_field_value(char * received, const char * field) {
	char * sub = strstr(received, field);
	if (!sub) {
		return NULL;
	}

	sub += strlen(field);
	while (!isalnum(*sub)) {
		sub++;
	}

	return sub;
}

#if !defined(NHR_NO_RECV_CHUNKS)
void nhr_response_read_chunks(_nhr_response * r, char * str) {
	unsigned int chunk_len = 0;
	while ((nhr_sscanf(str, "%X", &chunk_len) == 1) || (nhr_sscanf(str, "%x", &chunk_len) == 1)) {
		while (isxdigit(*str)) str++;
		if (chunk_len == 0) {
			if (strncmp(str, k_nhr_double_CRLF, k_nhr_double_CRLF_length) == 0) {
				r->is_all_chunks_processed = nhr_true;
			}
			break;
		}
		str += k_nhr_CRLF_length;
		nhr_response_add_body_data(r, str, chunk_len);
		str += chunk_len;
		str += k_nhr_CRLF_length;
		chunk_len = 0;
	}
}
#endif

void nhr_response_parse_body(_nhr_response * r, char * received, const size_t received_len) {
#if defined(NHR_DEBUG_LOG)
	size_t log_index = 0;
	printf("\n----[ RESPONCE HEADER ]----\n");
	for (log_index = 0; log_index < received_len; log_index++) {
		printf("%c", received[log_index]);
	}
	printf("\n---------------------------\n");
#endif
	size_t skiped = 0;
	char * sub = strstr(received, k_nhr_double_CRLF);
	if (!sub) {
		return;
	}

	skiped = sub - received;
	if (skiped > k_nhr_double_CRLF_length) {
		sub += k_nhr_double_CRLF_length;
		skiped -= k_nhr_double_CRLF_length;

		if (r->transfer_encoding & NHR_TRANSFER_ENCODING_CHUNKED) {
#if !defined(NHR_NO_RECV_CHUNKS)
			nhr_response_read_chunks(r, sub);
#endif
		} else {
			r->body_len = received_len - skiped;
			r->body_size = r->content_length > r->body_len ? r->content_length + 32 : r->body_len;
			r->body = nhr_malloc(r->body_size);
			memcpy(r->body, sub, r->body_len);
		}
		nhr_response_fix_body_len(r);
	}
}

void nhr_response_parse_status_code(_nhr_response * r, char * received) {
	int code = 0;
	char * value = strstr(received, " ");
	if (!value) {
		return;
	}

	if (nhr_sscanf(value, " %i", &code) == 1 && code > 0) {
		r->status_code = code;
	}
}

void nhr_response_parse_content_length(_nhr_response * r, char * received) {
	unsigned long length = 0;
	char * value = nhr_response_find_http_field_value(received, k_nhr_content_length);
	if (!value) {
		return;
	}

	if (nhr_sscanf(value, "%lu", &length) == 1 && length > 0) {
		r->content_length = (size_t)length;
	}
}

void nhr_response_log_unprocessed(const char * key, const char * value) {
	printf("\nWARNING: libnhr unprocessed \"%s\" : \"%s\".", key, value);
}

void nhr_response_parse_transfer_encoding(_nhr_response * r, char * received) {
	char * encoding = nhr_response_find_http_field_value(received, k_nhr_transfer_encoding);
	if (!encoding) {
		return;
	}

	if (strstr(encoding, k_nhr_chunked)) {
#if defined(NHR_NO_RECV_CHUNKS)
		nhr_response_log_unprocessed(k_nhr_transfer_encoding, encoding);
#else
		r->transfer_encoding |= NHR_TRANSFER_ENCODING_CHUNKED;
#endif
	}
}

void nhr_response_parse_content_encoding(_nhr_response * r, char * received) {
	char * encoding = nhr_response_find_http_field_value(received, k_nhr_content_encoding);
	if (!encoding) {
		return;
	}

	if (strstr(encoding, k_nhr_gzip)) {
#if defined(NHR_GZIP)
		r->content_encoding |= NHR_CONTENT_ENCODING_GZIP;
#else
		nhr_response_log_unprocessed(k_nhr_transfer_encoding, encoding);
#endif
	}
	if (strstr(encoding, k_nhr_deflate)) {
#if defined(NHR_GZIP)
		r->content_encoding |= NHR_CONTENT_ENCODING_DEFLATE;
#else
		nhr_response_log_unprocessed(k_nhr_transfer_encoding, encoding);
#endif
	}
}

nhr_bool nhr_response_is_finished_receiving(_nhr_response * r) {
#if !defined(NHR_NO_RECV_CHUNKS)
	if (r->transfer_encoding & NHR_TRANSFER_ENCODING_CHUNKED) {
		return r->is_all_chunks_processed;
	}
#endif
	return (r->content_length > 0) ? r->content_length == r->body_len : nhr_false;
}

void nhr_response_check_is_finished(_nhr_response * r) {
	if (!nhr_response_is_finished_receiving(r)) {
		return;
	}

	if (r->content_encoding & NHR_CONTENT_ENCODING_GZIP ||
		r->content_encoding & NHR_CONTENT_ENCODING_DEFLATE) {
#if defined(NHR_GZIP)
		nhr_response_ungzip(r);
#endif
	}
	r->is_finished = nhr_true;
}

_nhr_response * nhr_response_create(void * received, const size_t received_len) {
	_nhr_response * r = (_nhr_response *)nhr_malloc_zero(sizeof(struct _nhr_response_struct));

	nhr_response_parse_status_code(r, received);

	if (r->status_code != 0) {
		nhr_response_parse_content_length(r, received);
		nhr_response_parse_transfer_encoding(r, received);
		nhr_response_parse_content_encoding(r, received);
		nhr_response_parse_body(r, received, received_len);
	}
	nhr_response_check_is_finished(r);
	return r;
}

void nhr_response_add_body_data(_nhr_response * r, void * data, const size_t data_size) {
	const size_t required = r->body_len + data_size;
	char * body = NULL;

	if (required > r->body_size) {
		r->body = nhr_realloc(r->body, required);
		r->body_size = required;
	}

	body = (char *)r->body;
	body += r->body_len;
	memcpy(body, data, data_size);
	r->body_len += data_size;
	nhr_response_fix_body_len(r);
}

void nhr_response_append(_nhr_response * r, void * received, const size_t received_len) {
	if (r->transfer_encoding & NHR_TRANSFER_ENCODING_CHUNKED) {
#if !defined(NHR_NO_RECV_CHUNKS)
		nhr_response_read_chunks(r, (char *)received);
#endif
	} else {
		nhr_response_add_body_data(r, received, received_len);
	}
	nhr_response_check_is_finished(r);
}

void nhr_response_delete(_nhr_response * r) {
	if (r) {
		nhr_free(r->body);
	}
	nhr_free(r);
}

// public
unsigned short nhr_response_get_status_code(nhr_response responce) {
	_nhr_response * r = (_nhr_response *)responce;
	return r ? r->status_code : 0;
}

void* nhr_response_get_body(nhr_response responce) {
	_nhr_response * r = (_nhr_response *)responce;
	return r ? r->body : NULL;
}

unsigned int nhr_response_get_body_length(nhr_response responce) {
	_nhr_response * r = (_nhr_response *)responce;
	return r ? (unsigned int)r->body_len : 0;
}
