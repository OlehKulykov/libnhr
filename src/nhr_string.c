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


#include "nhr_string.h"
#include "nhr_memory.h"

char * nhr_string_copy(const char * str) {
	return str ? nhr_string_copy_len(str, strlen(str)) : NULL;
}

char * nhr_string_copy_len(const char * str, const size_t len) {
	char * s = (str && len > 0) ? (char *)nhr_malloc(len + 1) : NULL;
	if (s) {
		memcpy(s, str, len);
		s[len] = 0;
		return s;
	}
	return NULL;
}

char nhr_string_last_char(const char * str) {
	const size_t len = str ? strlen(str) : 0;
	return len > 0 ? str[len - 1] : 0;
}

void nhr_string_delete(char * str) {
	nhr_free(str);
}

void nhr_string_delete_clean(char ** str) {
	if (str) {
		nhr_free(*str);
		*str = NULL;
	}
}
