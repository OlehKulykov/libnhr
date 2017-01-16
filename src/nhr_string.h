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


#ifndef __NHR_STRING_H__
#define __NHR_STRING_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nhr_common.h"

#if defined(_MSC_VER)
#define nhr_sprintf(s,l,f,...) sprintf_s(s,l,f,__VA_ARGS__)
#define nhr_sscanf(s,f,...) sscanf_s(s,f,__VA_ARGS__)
#define nhr_strerror(e) strerror(e)
#else
#define nhr_sprintf(s,l,f,...) sprintf(s,f,__VA_ARGS__)
#define nhr_sscanf(s,f,...) sscanf(s,f,__VA_ARGS__)
#define nhr_strerror(e) strerror(e)
#endif

char * nhr_string_copy(const char * str);

char * nhr_string_copy_len(const char * str, const size_t len);

char nhr_string_last_char(const char * str);

void nhr_string_delete(char * str);

void nhr_string_delete_clean(char ** str);

#endif
