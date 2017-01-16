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


#include "nhr_common.h"
#include "../libnhr.h"

const char * k_nhr_request_http_ver = "1.1";
const char * k_nhr_content_type = "Content-Type";
const char * k_nhr_application_x_www_form_urlencoded = "application/x-www-form-urlencoded";
const char * k_nhr_transfer_encoding = "Transfer-Encoding";
//const char * k_nhr_content_encoding = "Content-Encoding";
//const char * k_nhr_gzip_deflate = "gzip, deflate";
//char * k_nhr_gzip = "gzip";
//char * k_nhr_deflate = "deflate";
const char * k_nhr_CRLF = "\r\n";
const char * k_nhr_double_CRLF = "\r\n\r\n";
const char * k_nhr_content_length = "Content-Length";
const char * k_nhr_chunked = "chunked";
//const char * k_nhr_GET = "GET";
//const char * k_nhr_POST = "POST";
