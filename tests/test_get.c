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


#include "libnhr_public_tests.h"

#if !defined(NHR_NO_GET)

static nhr_request test_get_request = NULL;
static const char * test_get_param_name1 = "test_get_param_name1";
static const char * test_get_param_value1 = "test_get_param_value1";
static int test_get_error = 0;
static nhr_bool test_get_working = 0;

static void test_get_on_error(nhr_request request, nhr_error_code error_code) {
	printf("\nResponce error: %i", (int)error_code);
	test_get_error = error_code;


	test_get_working = nhr_false;
}

static int test_get_parse_body(const char * body, unsigned long test_number) {
	cJSON * json = cJSON_ParseWithOpts(body, NULL, 0);
	cJSON * args = json ? cJSON_GetObjectItem(json, "args") : NULL;
	cJSON * headers = json ? cJSON_GetObjectItem(json, "headers") : NULL;
	cJSON * param1 = args ? cJSON_GetObjectItem(args, test_get_param_name1) : NULL;
	cJSON * deflated = json ? cJSON_GetObjectItem(json, "deflated") : NULL;
	cJSON * gzipped = json ? cJSON_GetObjectItem(json, "gzipped") : NULL;

	switch (test_number) {
		case 1:
			if (args && headers && param1 && param1->valuestring) {
				if (strcmp(test_get_param_value1, param1->valuestring) == 0) return 0;
				return 9;
			}
			break;

		case 2:
			if (deflated && deflated->valueint) return 0; // bool `true`
			break;

		case 3:
			if (gzipped && gzipped->valueint) return 0; // bool `true`
			break;

		default:
			break;
	}

	return 12;
}

static void test_get_log_body(const char * body, const unsigned int body_len) {
	if (!body || body_len == 0) return;
	int i;
	for (i = 0; i < body_len; i++) {
		printf("%c", body[i]);
	}
}

static void test_get_on_response(nhr_request request, nhr_response responce) {
	char * body = nhr_response_get_body(responce);
	unsigned int body_len = nhr_response_get_body_length(responce);
	test_get_error = 1;
	unsigned long test_number = (unsigned long)nhr_request_get_user_object(request);
	printf("\nResponce #%lu:\n", test_number);
	test_get_log_body(body, body_len);
	if (test_number == 0) {
		test_get_error = 10;
		test_get_working = nhr_false;
		return;
	}

	if (test_number == 4) { // status code 418
		printf("\nGet status code: %i, need 418", (int)nhr_response_get_status_code(responce));
		test_get_error = nhr_response_get_status_code(responce) == 418 ? 0 : 14;
		test_get_working = nhr_false;
		return;
	}

	if (test_number == 5) {
		printf("\nGet body_len: %u, need 1024", body_len);
		test_get_error = body_len == 1024 ? 0 : 15;
		test_get_working = nhr_false;
		return;
	}

	if (body && body_len) {
		test_get_error = test_get_parse_body(body, test_number);
	} else {
		test_get_error = 5;
	}

	test_get_working = nhr_false;
}

static int test_get_number(unsigned long number) {

	test_get_request = nhr_request_create();

	switch (number) {
		case 1: nhr_request_set_url(test_get_request, "http", "httpbin.org", "/get", 80); break;
		case 2: nhr_request_set_url(test_get_request, "http", "httpbin.org", "/deflate", 80); break;
		case 3: nhr_request_set_url(test_get_request, "http", "httpbin.org", "/gzip", 80); break;
		case 4: nhr_request_set_url(test_get_request, "http", "httpbin.org", "/status/418", 80); break;
		case 5: nhr_request_set_url(test_get_request, "http", "httpbin.org", "/range/1024", 80); break;
		default:
			break;
	}


	nhr_request_set_method(test_get_request, nhr_method_GET);
	nhr_request_set_timeout(test_get_request, 10);

	nhr_request_set_user_object(test_get_request, (void*)number);

	nhr_request_add_header_field(test_get_request, "Cache-control", "no-cache");
	nhr_request_add_header_field(test_get_request, "Accept-Charset", "utf-8");
	nhr_request_add_header_field(test_get_request, "Accept", "application/json");
	nhr_request_add_header_field(test_get_request, "Connection", "close");
	nhr_request_add_header_field(test_get_request, "User-Agent", "CMake tests");

	switch (number) {
		case 1:
			nhr_request_add_parameter(test_get_request, test_get_param_name1, test_get_param_value1);
			break;

		case 2:
			nhr_request_add_header_field(test_get_request, "Accept-Encoding", k_nhr_deflate);
			break;

		case 3:
			nhr_request_add_header_field(test_get_request, "Accept-Encoding", k_nhr_gzip);
			break;

		case 5:
			// chunked, total 1024, chunk_size 375, delay 1
			nhr_request_set_timeout(test_get_request, 10 + 1024/375); // increase default delay
			nhr_request_add_parameter(test_get_request, "duration", "1");
			nhr_request_add_parameter(test_get_request, "chunk_size", "375");
		default:
			break;
	}

	nhr_request_set_on_recvd_responce(test_get_request, &test_get_on_response);
	nhr_request_set_on_error(test_get_request, &test_get_on_error);
	test_get_working = nhr_request_send(test_get_request);

	if (test_get_working) test_get_error = 0;
	else test_get_error = 4;

	while (test_get_working) {
		nhr_thread_sleep(20);
	}

	return test_get_error;
}

int test_get(void) {
	int ret = 0;

	ret += test_get_number(1); // plain responce

#if !defined(NHR_NO_GZIP)
	ret += test_get_number(2); // deflate responce
	ret += test_get_number(3); // gziped responce
#endif

	ret += test_get_number(4); // status code 418

#if !defined(NHR_NO_CHUNKED)
	ret += test_get_number(5); // chunked, total 1024, chunk_size 375, delay 1
#endif

	return ret;
}
#endif

#if !defined(XCODE)
int main(int argc, char* argv[]) {

	int ret = 0;
#if !defined(NHR_NO_GET)
	ret += test_get();
	assert(ret == 0);
#endif

	return ret;
}
#endif
