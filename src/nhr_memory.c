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


#include "nhr_memory.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void * nhr_malloc(const size_t size) {
	if (size > 0) {
		void * mem = malloc(size);
		assert(mem);
		return mem;
	}
	return NULL;
}

void * nhr_malloc_zero(const size_t size) {
	void * mem = nhr_malloc(size);
	if (mem) {
		memset(mem, 0, size);
	}
	return mem;
}

void nhr_free(void * mem) {
	if (mem) {
		free(mem);
	}
}

void nhr_free_clean(void ** mem) {
	if (mem) {
		nhr_free(*mem);
		*mem = NULL;
	}
}

void * nhr_realloc(void * mem, const size_t new_size) {
	if (new_size > 0) {
		void * new_mem = realloc(mem, new_size);
		assert(new_mem);
		return new_mem;
	}
	return NULL;
}


