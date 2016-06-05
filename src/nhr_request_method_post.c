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

#if !defined(NHR_NO_POST)

char * nhr_request_create_header_POST(_nhr_request * r, size_t * header_size) {
	size_t buff_size = 0, writed = 0, headers_len = 0, parameters_len = 0;
	char * buff = NULL, *headers = NULL, *parameters = NULL;
	char content_length[24];

	buff_size = strlen(r->path);
	buff_size += strlen(r->host);

	parameters = r->parameters ? nhr_request_url_encoded_parameters(r->parameters, &parameters_len) : NULL;
	if (parameters) {
		buff_size += parameters_len;
		nhr_sprintf(content_length, 24, "%lu", (unsigned long)parameters_len);
		nhr_request_add_header_field(r, k_nhr_content_type, "application/x-www-form-urlencoded");
		nhr_request_add_header_field(r, k_nhr_content_length, content_length);
	}

	headers = r->http_headers ? nhr_request_http_headers(r->http_headers, &headers_len) : NULL;
	if (headers) buff_size += headers_len;

	buff_size += 1 << 8; // extra size for formatting strings

	buff = (char *)nhr_malloc(buff_size);

	writed = nhr_sprintf(buff, buff_size, "%s %s HTTP/%s\r\n", k_nhr_POST, r->path, k_nhr_request_http_ver);

	if (r->port == 80) writed += nhr_sprintf(buff + writed, buff_size - writed, "Host: %s\r\n", r->host);
	else writed += nhr_sprintf(buff + writed, buff_size - writed, "Host: %s:%i\r\n", r->host, (int)r->port);

	if (headers) {
		writed += nhr_sprintf(buff + writed, buff_size - writed, "%s\r\n\r\n", headers);
	} else {
		memcpy(buff + writed, k_nhr_CRLF, k_nhr_CRLF_length);
		writed += k_nhr_CRLF_length;
	}

	if (parameters) writed += nhr_sprintf(buff + writed, buff_size, "%s\r\n\r\n", parameters);

	nhr_free(headers);
	nhr_free(parameters);
	buff[writed] = 0;
	*header_size = writed;
	return buff;
}

#endif
