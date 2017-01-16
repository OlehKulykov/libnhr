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


#ifndef __NHR_MAP_H__
#define __NHR_MAP_H__ 1


#include "nhr_common.h"
#include "nhr_string.h"
#include "nhr_memory.h"

typedef struct _nhr_map_node_struct {
	char * key;
	union {
		void * data;
		char * string;
		int int_value;
		unsigned int uint_value;
	} value;

	struct _nhr_map_node_struct * next;

	size_t value_size;

	unsigned char value_type;
	char tag;
} _nhr_map_node;

_nhr_map_node * nhr_map_create(void);

_nhr_map_node * nhr_map_last(_nhr_map_node * map);

_nhr_map_node * nhr_map_append(_nhr_map_node * map);

void nhr_map_delete(_nhr_map_node * map);

void nhr_map_delete_clean(_nhr_map_node ** map);

// free value data
#define NHR_MAP_VALUE_DATA 1
#define NHR_MAP_VALUE_STRING 2

// ingore value
#define NHR_MAP_VALUE_STATIC_STRING 3
#define NHR_MAP_VALUE_INT 4
#define NHR_MAP_VALUE_UINT 5

#endif
