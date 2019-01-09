/*
 *   Copyright (c) 2016 - 2019 Oleh Kulykov <info@resident.name>
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


#ifndef __LIBNHR_H__
#define __LIBNHR_H__ 1


#include <stdio.h>


#define NHR_VERSION_MAJOR 0
#define NHR_VERSION_MINOR 5
#define NHR_VERSION_PATCH 3


// check windows
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define NHR_OS_WINDOWS 1
#endif


// extern
#if defined(__cplusplus) || defined(_cplusplus)
#define NHR_EXTERN extern "C"
#else
#define NHR_EXTERN extern
#endif


// attribute
#if defined(__GNUC__)
#if (__GNUC__ >= 4)
#if defined(__cplusplus) || defined(_cplusplus)
#define NHR_ATTRIB __attribute__((visibility("default")))
#else
#define NHR_ATTRIB __attribute__((visibility("default")))
#endif
#endif
#endif


// check attrib and define empty if not defined
#if !defined(NHR_ATTRIB)
#define NHR_ATTRIB
#endif


// dll api
#if defined(NHR_OS_WINDOWS)
#if defined(NHR_BUILD)
#define NHR_DYLIB_API __declspec(dllexport)
#else
#define NHR_DYLIB_API __declspec(dllimport)
#endif
#endif


// check dll api and define empty if not defined
#if !defined(NHR_DYLIB_API)
#define NHR_DYLIB_API
#endif


// combined lib api
#define NHR_API(return_type) NHR_EXTERN NHR_ATTRIB NHR_DYLIB_API return_type


// types

/**
 @brief Boolean type as unsigned byte type.
 */
typedef unsigned char nhr_bool;
#define nhr_true 1
#define nhr_false 0


/**
 @brief Type of all public objects.
 */
typedef void * nhr_handle;


/**
 @brief Request object handle.
 */
typedef struct nhr_request_struct * nhr_request;


/**
 @brief Request object handle.
 */
typedef struct nhr_response_struct * nhr_response;


/**
 @brief Mutex object handle.
 */
typedef nhr_handle nhr_mutex;


/**
 @brief Thread object handle.
 */
typedef struct nhr_thread_struct * nhr_thread;


/**
 @brief Callback type on request reveiced response.
 @warning After trigering callback, request object will be automaticaly released.
 It's recommened set to `NULL` request variable inside this callback.
 @param request Request object.
 @param response Response object.
 */
typedef void (*nhr_on_request_recvd_response)(nhr_request request, nhr_response response);


/**
 @brief Callback type of thread function.
 @param user_object User object provided during thread creation.
 */
typedef void (*nhr_thread_funct)(void * user_object);


/**
 @brief Error codes.
 */
typedef enum _nhr_error_code {

	nhr_error_code_none = 0,

	nhr_error_code_missed_parameter,

	nhr_error_code_failed_connect_to_host,

	nhr_error_code_timeout
} nhr_error_code;


/**
 @brief Supported HTTP methods.
 */
typedef enum _nhr_method {
	/**
	 @brief Request method `GET`.
	 @note To disable `GET` functionality define flag `NHR_NO_GET`. By default enable.
	 */
	nhr_method_GET = 1,


	/**
	 @brief Request method `POST`.
	 @note To disable `POST` functionality define flag `NHR_NO_POST`. By default enable.
	 */
	nhr_method_POST = 2,
//	nhr_method_PUT,
//	nhr_method_DELETE,
//	nhr_method_HEAD,
//	nhr_method_CONNECT,
//	nhr_method_OPTIONS,
//	nhr_method_TRACE,
//	nhr_method_PATCH
} nhr_method;


/**
 @brief Callback type on socket error ocupared.
 @warning After trigering callback, request object will be automaticaly released.
 It's recommened set to `NULL` request variable in your logic.
 @param request Request object.
 @param response Response object.
 */
typedef void (*nhr_on_request_error)(nhr_request request, nhr_error_code error_code);


// String constant "GET"
#define k_nhr_GET "GET"


// String constant "POST"
#define k_nhr_POST "POST"


/**
 @brief String constant "Content-Encoding"
 Can be used as HTTP header name.
 */
#define k_nhr_content_encoding "Content-Encoding"


/**
 @brief String constant "gzip, deflate"
 Can be used as HTTP header value.
 */
#define k_nhr_gzip_deflate "gzip, deflate"


/// String constant "gzip"
#define k_nhr_gzip "gzip"


/// String constant "deflate"
#define k_nhr_deflate "deflate"


// request

/**
 @brief Create new request.
 @note Default timeout interval is `30` seconds.
 @return Request handler or NULL on error.
 */
NHR_API(nhr_request) nhr_request_create(void);


/**
 @brief Set request connect URL.
 @note This method is required, before send the request via `nhr_request_send`.
 @param request The request object. If request is `NULL` do nothing.
 @param scheme Connect URL scheme, "http"
 @param scheme Connect URL host, "api.ipify.org"
 @param scheme Connect URL path started with '/' character, "/" - for empty, "/path"
 @param scheme Connect URL port.
 @code
 nhr_request_set_url(request, "http", "api.ipify.org", 80, "/");
 @endcode
 */
NHR_API(void) nhr_request_set_url(nhr_request request,
								  const char * scheme,
								  const char * host,
								  const char * path,
								  const unsigned short port);

/**
 @brief Send request.
 @param request The request object. If request is `NULL` returns `nhr_false`.
 @return `nhr_true` if request exists, scheme, host, path, method are setted and request sucessfully started,
 otherwice `nhr_false`
 */
NHR_API(nhr_bool) nhr_request_send(nhr_request request);


/**
 @brief Get error code from request.
 @param request The request object. If request is `NULL` returns `nhr_error_code_none`.
 */
NHR_API(nhr_error_code) nhr_request_get_error_code(nhr_request request);


/**
 @brief Set request method.
 @note This method is required, before send the request via `nhr_request_send`.
 @param request The request object. If request is `NULL` do nothing.
 */
NHR_API(void) nhr_request_set_method(nhr_request request, nhr_method method);


/**
 @brief Set on request received response callback.
 @note This method is required, before send the request via `nhr_request_send`.
 @warning After trigering callback, request object will be automaticaly released.
 It's recommened set to `NULL` request variable inside this callback.
 Called from non `main` thread.
 @param request The request object. If request is `NULL` do nothing.
 @param callback On request received response callback. Called from non `main` thread.
 */
NHR_API(void) nhr_request_set_on_recvd_response(nhr_request request, nhr_on_request_recvd_response callback);


/**
 @brief Set on request error ocupared callback.
 @note This method is required, before send the request via `nhr_request_send`.
 @warning After trigering callback, request object will be automaticaly released.
 It's recommened set to `NULL` request variable inside this callback.
 Called from non `main` thread.
 @param request The request object. If request is `NULL` do nothing.
 @param callback On request error ocupared callback. Called from non `main` thread.
 */
NHR_API(void) nhr_request_set_on_error(nhr_request request, nhr_on_request_error callback);


/**
 @brief Add request HTTP header field and it's value.
 @param request The request object. If request is `NULL` do nothing.
 @param name The HTTP header field name. `NULL` or empty value is ignored.
 @param value Value of the header field. `NULL` or empty value is ignored.
 @note If value is `k_nhr_deflate` or `k_nhr_gzip` post body will be compressed
 with appropriate method.
 */
NHR_API(void) nhr_request_add_header_field(nhr_request request, const char * name, const char * value);


/**
 @brief Add request parameter and it's value.
 @param request The request object. If request is `NULL` do nothing.
 @param name Parameter name. `NULL` or empty value is ignored.
 @param value Value of the parameter. Should be URL encoded. `NULL` or empty value is ignored.
 @warning Add parameters only after method was setted via `nhr_request_set_method`.
 @warning Except `POST`request with data parameters, this value should be URL encoded.
 */
NHR_API(void) nhr_request_add_parameter(nhr_request request, const char * name, const char * value);


/**
 @brief Add to `POST` request file parameter.
 @param request The request object. If request is `NULL` do nothing.
 @param name Parameter name. `NULL` or empty value is ignored.
 @param file_name File name of the data. Should not be `NULL`.
 @param data Some binary or text data. Should not be `NULL`.
 @param data_size Size of the provided data. Should not be `0`.
 @warning If `NHR_NO_POST` and `NHR_NO_POST_DATA` defined than this function do nothing.
 @warning Add parameters only after method was setted via `nhr_request_set_method` and only for `POST`.
 */
NHR_API(void) nhr_request_add_data_parameter(nhr_request request, const char * name, const char * file_name, const void * data, const size_t data_size);


/**
 @brief Assign some object with the request.
 @param request The request object. If request is `NULL` do nothing.
 @param user_object Any pointer.
 */
NHR_API(void) nhr_request_set_user_object(nhr_request request, void * user_object);


/**
 @brief Get assigned user object from request.
 @param request The request object. If request is `NULL` returns `NULL`.
 @return Pointer assigned by the `nhr_request_set_user_object` function or `NULL`.
 */
NHR_API(void*) nhr_request_get_user_object(nhr_request request);


/**
 @brief Set request timeout interval in seconds.
 @param request The request object. If request is `NULL` do nothing.
 @param seconds The timeout interval in seconds.
 */
NHR_API(void) nhr_request_set_timeout(nhr_request request, const unsigned int seconds);


/**
 @brief Get request timeout interval in seconds.
 Default value is `30` seconds.
 @param request The request object. If request is `NULL` returns `0`.
 @return Request timeout interval in seconds, or `0` if request is NULL.
 */
NHR_API(unsigned int) nhr_request_get_timeout(nhr_request request);


/**
 @brief Cancel request.
 Thread safe method.
 @param request The request object. If request is `NULL` do nothing.
 @warning Do not use request variable any more.
 Do not call `cancel` after any callback was trigered.
 */
NHR_API(void) nhr_request_cancel(nhr_request request);



// response

/**
 @brief Get HTTP status code of the response.
 @param response The response object. If response is `NULL` returns `0`.
 @return Status code of the request.
 */
NHR_API(unsigned short) nhr_response_get_status_code(nhr_response response);


/**
 @brief Get response received body data.
 @note To get body data length use `nhr_response_get_body_length` function.
 @param response The response object. If response is `NULL` returns `NULL`.
 @return Pointer to the received response data.
 */
NHR_API(void*) nhr_response_get_body(nhr_response response);


/**
 @brief Get response received body data length.
 @note To get body data use `nhr_response_get_body` function.
 @param response The response object. If response is `NULL` returns `0`.
 @return Length of the received response body data.
 */
NHR_API(unsigned int) nhr_response_get_body_length(nhr_response response);



// mutex

/**
 @brief Creates recursive mutex object.
 */
NHR_API(nhr_mutex) nhr_mutex_create_recursive(void);


/**
 @brief Lock mutex object.
 */
NHR_API(void) nhr_mutex_lock(nhr_mutex mutex);


/**
 @brief Unlock mutex object.
 */
NHR_API(void) nhr_mutex_unlock(nhr_mutex mutex);



/**
 @brief Unlock mutex object.
 */
NHR_API(void) nhr_mutex_delete(nhr_mutex mutex);



// thread

/**
 @brief Create thread object that start immidiatelly.
 @param thread_function Thre thread function.
 @param user_object Any object pointer that provide to the thread function.
 */
NHR_API(nhr_thread) nhr_thread_create(nhr_thread_funct thread_function, void * user_object);


/**
 @brief Pause current thread for a number of milliseconds.
 */
NHR_API(void) nhr_thread_sleep(const unsigned int millisec);

#endif
