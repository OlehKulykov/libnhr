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

#if !defined(NHR_NO_GET) || !defined(NHR_NO_POST)

size_t nhr_request_map_strings_length(_nhr_map_node * map, const size_t iteration_increment) {
	size_t len = 0;
	_nhr_map_node *cur = map;
	while (cur) {
		len += strlen(cur->key) + strlen(cur->value.string) + iteration_increment;
		cur = cur->next;
	}
	return len;
}

char * nhr_request_http_headers(_nhr_map_node * map, size_t * length) {
	const size_t buff_size = nhr_request_map_strings_length(map, 4); // "\r\n" + ": "
	size_t writed = 0;
	_nhr_map_node * cur = map;
	char * buff = NULL;

	if (buff_size == 0) {
		return NULL;
	}
	buff = (char *)nhr_malloc(buff_size); // null terminated included
	
	while (cur) {
		writed += nhr_sprintf(buff + writed, buff_size - writed, writed > 0 ? "\r\n%s: %s" : "%s: %s", cur->key, cur->value.string);
		cur = cur->next;
	}
	assert(writed < buff_size);
	buff[writed] = 0;
	*length = writed;
	return buff;
}

char * nhr_request_url_encoded_parameters(_nhr_map_node * map, size_t * length) {
	size_t buff_size = nhr_request_map_strings_length(map, 2); // `&` + `=`
	size_t writed = 0;
	_nhr_map_node * cur = map;
	char * buff = NULL;

	if (buff_size == 0) {
		return NULL;
	}
	buff = (char *)nhr_malloc(buff_size); // null terminated included
	
	while (cur) {
		writed += nhr_sprintf(buff + writed, buff_size - writed, writed > 0 ? "&%s=%s" : "%s=%s", cur->key, cur->value.string);
		cur = cur->next;
	}
	buff[writed] = 0;
	*length = writed;
	return buff;
}

#endif
