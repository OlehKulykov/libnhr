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


#ifndef __NHR_COMMON_H__
#define __NHR_COMMON_H__ 1

#include <stdio.h>

/* check os */
/* gcc -dM -E - < /dev/null */

/* check windows */
#if !defined(NHR_OS_WINDOWS)
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define NHR_OS_WINDOWS 1
#endif
#endif
/* end check windows */

/* check Apple */
#if !defined(NHR_OS_APPLE)
#if defined(__APPLE__) || defined(__MACH__)
#define NHR_OS_APPLE 1
#endif
#endif

/* check Unix */
#if !defined(NHR_OS_UNIX)
#if defined(__unix) || defined(unix) || defined(__unix__)
#define NHR_OS_UNIX 1
#endif
#endif

/* check Linux */
#if !defined(NHR_OS_LINUX)
#if defined(__linux__) || defined(__linux)
#define NHR_OS_LINUX 1
#endif
#endif

/* check Android */
#if !defined(NHR_OS_ANDROID)
#if defined(__ANDROID__) || defined(ANDROID)
#define NHR_OS_ANDROID 1
#endif
#endif

#if !defined(NHR_EXTERN)
#if defined(__cplusplus) || defined(_cplusplus)
#define NHR_EXTERN extern "C"
#else
#define NHR_EXTERN extern
#endif
#endif


/* check debug */
#if defined(DEBUG) || defined(_DEBUG)
#define NHR_DEBUG 1
#endif


/* check not debug, release */
#if defined(NDEBUG) || defined(_NDEBUG) || defined(RELEASE) || defined(_RELEASE)
#if defined(NHR_DEBUG)
#undef NHR_DEBUG
#endif
#endif


// Settings

#if defined(NHR_HAVE_ZLIB_H) && !defined(NHR_NO_GZIP)
#define NHR_GZIP 1
#endif



// internal constants

NHR_EXTERN const char * k_nhr_request_http_ver; // "1.1"
NHR_EXTERN const char * k_nhr_content_type; // "Content-Type"
NHR_EXTERN const char * k_nhr_application_x_www_form_urlencoded; // "application/x-www-form-urlencoded"
NHR_EXTERN const char * k_nhr_transfer_encoding; // "Transfer-Encoding"

NHR_EXTERN const char * k_nhr_CRLF; // "\r\n"
#define k_nhr_CRLF_length 2

NHR_EXTERN const char * k_nhr_double_CRLF; // "\r\n\r\n";
#define k_nhr_double_CRLF_length 4

NHR_EXTERN const char * k_nhr_content_length; // "Content-Length";

NHR_EXTERN const char * k_nhr_chunked; // "chunked"
#define k_nhr_chunked_length 7

#define k_nhr_gzip_length 4
#define k_nhr_deflate_length 7

#endif

