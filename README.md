# libnhr - Tiny Network HTTP Request cross platform C library.

[![Build Status](https://travis-ci.org/OlehKulykov/libnhr.svg?branch=master)](https://travis-ci.org/OlehKulykov/libnhr)
[![Build status](https://ci.appveyor.com/api/projects/status/fqggl0utd7gqguoc/branch/master?svg=true)](https://ci.appveyor.com/project/OlehKulykov/libnhr/branch/master)


### Features
* Single header library interface ```libnhr.h``` with public methods
* Thread safe
* Send/receive logic in background thread


### TODO/DONE

- [x] **GET**.
  - [x] Send url encoded request, default.
- [ ] **POST**
  - [x] Send url encoded request, default.
  - [x] Send deflate compressed url encoded request.
  - [x] Send gzip compressed url encoded request.
  - [ ] Send chuncked request body.
  - [ ] Send file/data.
- [x] **Responce**
  - [x] Process responce with chunked transfer encoding.
  - [x] Process deflate compressed content encoding.
  - [x] Process gzip compressed content encoding.


### Build

All features are **enabled by default**. But it's possible to **disable**. See table below:

| Preprocessor   | CMake (Boolean value) | Description                                                                                  |
|----------------|-----------------------|----------------------------------------------------------------------------------------------|
| NHR_NO_GET     | NHR_OPT_NO_GET        | Send GET requests.                                                                           |
| NHR_NO_POST    | NHR_OPT_NO_POST       | Send POST requests.                                                                          |
| NHR_NO_CHUNKED | NHR_OPT_NO_CHUNKED    | Process received responce with chunked transfer encoding, e.g. "Transfer-Encoding: chunked". |
| NHR_NO_GZIP    | NHR_OPT_NO_GZIP       | Post gzip or deflate compressed url encoded parameters.                                      |
|                |                       | Process gzip or deflate compressed responce body.                                            |


#### Build with CMake
> Use (install or update) latest [CMake] build system, need version 2.8 or later.

```sh
cd libnhr
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

> Example how to disable, for instance ```POST``` method, with CMake.
> ```sh
> cmake -DCMAKE_BUILD_TYPE=Release -DNHR_OPT_NO_POST:BOOL=ON ..
> ```


### Build for Android with [Android NDK]
 * Download and install [Android NDK].
 * Navigate to installed [Android NDK] folder.
 * Execute **ndk-build** script with project path parameter:

```sh
cd <path_to_Android_NDK>
./ndk-build NDK_PROJECT_PATH=<path_to_libnhr>/builds/android
```

> Replace ```<path_to_Android_NDK>``` and ```<path_to_libnhr>``` with actual paths.

> To disable some method(s) need to add **Preprocessor** flag from table to your ```Android.mk``` or gradle ndk section.

#### Use directly source
Without build system, just use single header ```libnhr.h``` and all content from source folder ```src```.


#### Example
##### Create and store request object handle
```c
// Define variable or field for the request
nhr_request _request = NULL;
............
// Create request object
_request = nhr_request_create();
```

##### Set request url and method
```c
// Combined url: "http://api.ipify.org"
nhr_request_set_url(_request, "http", "api.ipify.org", "/", 80);
// or
// Combined url: "http://isithackday.com/arrpi.php"
nhr_request_set_url(_request, "http", "isithackday.com", "/arrpi.php", 80);

nhr_request_set_method(_request, nhr_method_GET); // GET
// or
//nhr_request_set_method(_request, nhr_method_POST); // POST
```

##### Optionally add HTTP headers and/or parameters
```c
// Add HTTP headers
nhr_request_add_header_field(_request, "Cache-control", "no-cache");
nhr_request_add_header_field(_request, "Accept-Charset", "utf-8");

// Optional: in a case of POST, send gzip or deflate compressed url encoded parameters
nhr_request_add_header_field(request, k_nhr_content_encoding, k_nhr_gzip); // gzip
nhr_request_add_header_field(request, k_nhr_content_encoding, k_nhr_deflate); // deflate
............
// Add request parameters
nhr_request_add_parameter(_request, "format", "json");
nhr_request_add_parameter(_request, "text", "Hello%20world");
```

##### Set request callbacks
```c
static void on_request_error(nhr_request request, nhr_error_code error_code) {
	_request = NULL; // Clean up previously stored request variable or field
	//TODO: process `error_code`
}

static void on_request_response(nhr_request request, nhr_response responce) {
	_request = NULL; // Clean up previously stored request variable or field
	char * body = nhr_response_get_body(responce);
	unsigned int body_length = nhr_response_get_body_length(responce);
	if (body && body_length) {
		//TODO: process responce body data
	}
}
..................
// Set request callbacks
nhr_request_set_on_recvd_responce(_request, &on_request_response);
nhr_request_set_on_error(_request, &on_request_error);
```

##### Send request
```c
nhr_request_send(_request);
```


### License

The MIT License (MIT)

Copyright (c) 2016 Kulykov Oleh <info@resident.name>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


[CMake]:http://www.cmake.org
[Android NDK]:https://developer.android.com/tools/sdk/ndk/index.html
