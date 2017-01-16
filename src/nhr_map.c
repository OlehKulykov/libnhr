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


#include "nhr_map.h"

_nhr_map_node * nhr_map_create(void) {
	return (_nhr_map_node *)nhr_malloc_zero(sizeof(struct _nhr_map_node_struct));
}

_nhr_map_node * nhr_map_last(_nhr_map_node * map) {
	_nhr_map_node *cur = map, *last = map;
	while (cur) {
		last = cur;
		cur = cur->next;
	}
	return last;
}

_nhr_map_node * nhr_map_append(_nhr_map_node * map) {
	_nhr_map_node * last = nhr_map_last(map);
	if (last->value_type) {
		last->next = (_nhr_map_node *)nhr_malloc_zero(sizeof(struct _nhr_map_node_struct));
		return last->next;
	}
	return last;
}

void nhr_map_delete(_nhr_map_node * map) {
	_nhr_map_node *cur = map, *last;
	while (cur) {
		last = cur;
		cur = cur->next;
		nhr_string_delete(last->key);
		if (last->value_type <= NHR_MAP_VALUE_STRING) {
			nhr_free(last->value.data);
		}
		nhr_free(last);
	}
}

void nhr_map_delete_clean(_nhr_map_node ** map) {
	if (map) {
		nhr_map_delete(*map);
		*map = NULL;
	}
}
