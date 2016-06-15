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

nhr_request test_get_request = NULL;
const char * test_get_param_name1 = "test_get_param_name1";
const char * test_get_param_value1 = "test_get_param_value1";
int test_get_error = 0;
nhr_bool test_get_working = 0;

static void test_get_on_error(nhr_request request, nhr_error_code error_code) {
	printf("\nResponce error: %i", (int)error_code);
	test_get_error = error_code;
	test_get_working = nhr_false;
}

static int test_get_parse_body(const char * body) {
	cJSON * json = cJSON_ParseWithOpts(body, NULL, 0);
	cJSON * args = json ? cJSON_GetObjectItem(json, "args") : NULL;
	cJSON * headers = json ? cJSON_GetObjectItem(json, "headers") : NULL;
	cJSON * param1 = NULL;
	if (args && headers && (args->type & cJSON_Object) && (headers->type & cJSON_Object)) {
		param1 = cJSON_GetObjectItem(args, test_get_param_name1);
		if (!param1 || !(param1->type & cJSON_String) || !param1->valuestring) return 8;
		if (strcmp(test_get_param_value1, param1->valuestring) != 0) return 9;
	} else {
		return 7;
	}
	return 0;
}

static void test_get_on_response(nhr_request request, nhr_response responce) {
	printf("\nResponce:\n");
	char * body = nhr_response_get_body(responce);
	unsigned int body_len = nhr_response_get_body_length(responce);
	int i;
	test_get_error = 1;

	if (body && body_len)
	{
		test_get_error = test_get_parse_body(body);
		for (i = 0; i < body_len; i++) {
			printf("%c", body[i]);
		}
	} else {
		test_get_error = 5;
	}
	test_get_working = nhr_false;
}

int test_get(void) {

	test_get_request = nhr_request_create();
	nhr_request_set_url(test_get_request, "http", "httpbin.org", "/get", 80);
	nhr_request_set_method(test_get_request, nhr_method_GET);
	nhr_request_set_timeout(test_get_request, 10);

	nhr_request_add_header_field(test_get_request, "Cache-control", "no-cache");
	nhr_request_add_header_field(test_get_request, "Accept-Charset", "utf-8");

	nhr_request_add_parameter(test_get_request, test_get_param_name1, test_get_param_value1);

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
