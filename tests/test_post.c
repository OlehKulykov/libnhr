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


#include "libnhr_public_tests.h"

#if !defined(NHR_NO_POST) && !defined(NHR_APPVEYOR_CI)

static nhr_request test_post_request = NULL;
static int test_post_error = 0;
static nhr_bool test_post_working = 0;

static void test_post_on_error(nhr_request request, nhr_error_code error_code) {
	printf("\nResponse error: %i", (int)error_code);
	test_post_error = error_code;


	test_post_working = nhr_false;
}

static int test_post_parse_body(const char * body, unsigned long test_number) {

	return 0;
}

static void test_post_log_body(const char * body, const unsigned int body_len) {
	unsigned int i;
	if (!body || body_len == 0) return;
	for (i = 0; i < body_len; i++) {
		printf("%c", body[i]);
	}
}

static void test_post_on_response(nhr_request request, nhr_response response) {
	char * body = nhr_response_get_body(response);
	unsigned int body_len = nhr_response_get_body_length(response);
	unsigned long test_number = (unsigned long)nhr_request_get_user_object(request);
	test_post_error = 1;

	printf("\nResponse #%lu:\n", test_number);
	test_post_log_body(body, body_len);
	if (test_number == 0) {
		test_post_error = 10;
		test_post_working = nhr_false;
		return;
	}

	if (nhr_response_get_status_code(response) != 200) {
		test_post_error = 15;
		test_post_working = nhr_false;
		return;
	}

	if (body && body_len) {
		test_post_error = test_post_parse_body(body, test_number);
	} else {
		test_post_error = 5;
	}

	test_post_working = nhr_false;
}

static int test_post_number(unsigned long number) {

	test_post_request = nhr_request_create();

	switch (number) {
		case 1:
        case 2:
            nhr_request_set_url(test_post_request, "http", "httpbin.org", "/post", 80); break;
		default:
			break;
	}


	nhr_request_set_method(test_post_request, nhr_method_POST);
	nhr_request_set_timeout(test_post_request, 10);

	nhr_request_set_user_object(test_post_request, (void*)number);

	nhr_request_add_header_field(test_post_request, "Cache-control", "no-cache");
	nhr_request_add_header_field(test_post_request, "Accept-Charset", "utf-8");
	nhr_request_add_header_field(test_post_request, "Accept", "application/json");
	nhr_request_add_header_field(test_post_request, "Connection", "close");
	nhr_request_add_header_field(test_post_request, "User-Agent", "CMake tests");

	switch (number) {
        case 1:
            nhr_request_add_parameter(test_post_request, "name", "Url%20encoded%20name%20value");
			break;
            
#if !defined(NHR_NO_POST_DATA)
        case 2: {
            nhr_request_add_parameter(test_post_request, "name", "Some name");
            
            const char * textFile = "If the user selected a second (image) file, the user agent might construct the parts as follows";
            nhr_request_add_data_parameter(test_post_request, "text", "file1.txt", textFile, strlen(textFile));
        }
#endif
            
		default:
			break;
	}

	nhr_request_set_on_recvd_response(test_post_request, &test_post_on_response);
	nhr_request_set_on_error(test_post_request, &test_post_on_error);
	test_post_working = nhr_request_send(test_post_request);

	if (test_post_working) test_post_error = 0;
	else test_post_error = 4;

	while (test_post_working) {
		nhr_thread_sleep(20);
	}

	nhr_thread_sleep(834); // just delay between requests

	return test_post_error;
}

int test_post(void) {
	int ret = 0;

	ret += test_post_number(1); // plain response
    
#if !defined(NHR_NO_POST_DATA)
    ret += test_post_number(2);
#endif
    
	return ret;
}
#endif

#if !defined(XCODE)
int main(int argc, char* argv[]) {

	int ret = 0;
#if !defined(NHR_NO_POST) && !defined(NHR_APPVEYOR_CI)
	ret += test_post();
	assert(ret == 0);
#endif

	return ret;
}
#endif
