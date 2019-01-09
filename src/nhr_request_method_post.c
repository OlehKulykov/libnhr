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


#include "nhr_request.h"
#include <stdlib.h>
#include <time.h>

#if !defined(NHR_NO_POST) // POST functionality

#include "nhr_gz.h"

char * nhr_request_create_url_encoded_parameters_POST(nhr_request r, size_t * parameters_len) {
    size_t params_len = 0;
    char content_length[24];
#if defined(NHR_GZIP)
    size_t zgip_parameters_size = 0;
    void * zgip_parameters = NULL;
#endif
    
    char * params = r->parameters ? nhr_request_url_encoded_parameters(r->parameters, &params_len) : NULL;
    if (!params) {
        *parameters_len = 0;
        return NULL;
    }
    
#if defined(NHR_GZIP)
    if (r->is_gziped || r->is_deflated) {
        zgip_parameters = nhr_gz_compress(params, params_len, &zgip_parameters_size, r->is_gziped ? NHR_GZ_METHOD_GZIP : NHR_GZ_METHOD_DEFLATE);
        if (zgip_parameters && zgip_parameters_size) {
            nhr_free(params);
            params_len = zgip_parameters_size;
            params = zgip_parameters;
        } else {
            nhr_free(zgip_parameters);
        }
    }
#endif
    
    nhr_sprintf(content_length, 24, "%lu", (unsigned long)params_len);
    nhr_request_add_header_field(r, k_nhr_content_type, k_nhr_application_x_www_form_urlencoded);
    nhr_request_add_header_field(r, k_nhr_content_length, content_length);
    
    *parameters_len = params_len;
    return params;
}

#if !defined(NHR_NO_POST_DATA) // POST DATA functionality

void nhr_request_generate_new_boundary(nhr_request r) {
    static const char charset[62] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char boundary[NHR_POST_BOUNDARY_LEN];
    size_t i;
    srand((unsigned)time(NULL));
    for (i = 0; i < NHR_POST_BOUNDARY_LEN; i++) {
        boundary[i] = charset[rand() % 62];
    }
    nhr_string_delete_clean(&r->boundary);
    r->boundary = nhr_string_copy_len(boundary, NHR_POST_BOUNDARY_LEN);
}

void * nhr_request_create_data_parameters_POST(nhr_request r, size_t * parameters_len) {
    size_t params_len = 0, buff_size = 0, writed = 0;
    char content_length[24];
    char content_type[NHR_POST_BOUNDARY_LEN + 32]; // "multipart/form-data; boundary=BOUNDARY"
    _nhr_map_node * cur = NULL;
    char * buff = NULL;
    nhr_bool is_first = nhr_true;
    
    cur = r->parameters;
    while (cur) {
        params_len += strlen(cur->key) + cur->value_size + cur->reserved_size;
        params_len += NHR_POST_BOUNDARY_LEN + 2; // "--BOUNDARY"
        params_len += k_nhr_double_CRLF_length;
        
        switch (cur->value_type) {
            case NHR_MAP_VALUE_DATA:
                // "Content-Disposition: form-data; name=\"\"; filename=\"\"" + "\r\n"
                // "Content-Type: application/octet-stream" + "\r\n"
                // "Content-Transfer-Encoding: binary" + "\r\n\r\n"
                params_len += 124 + k_nhr_double_CRLF_length * 2;
                // contents of file
                break;
                
            case NHR_MAP_VALUE_STRING:
            case NHR_MAP_VALUE_STATIC_STRING:
                params_len += 40 + k_nhr_double_CRLF_length; // "Content-Disposition: form-data; name=\"\"" + "\r\n\r\n"
                // text param
                break;
                
            default:
                break;
        }
        cur = cur->next;
    }
    
    params_len += NHR_POST_BOUNDARY_LEN + 4; // "--BOUNDARY--"
    
    buff_size = params_len + 4;
    buff = (char *)nhr_malloc(buff_size);
    
    cur = r->parameters;
    while (cur) {
        if (is_first) {
            is_first = nhr_false;
        } else {
            memcpy(buff + writed, k_nhr_CRLF, k_nhr_CRLF_length); // "\r\n"
            writed += k_nhr_CRLF_length;
        }
        
        writed += nhr_sprintf(buff + writed, buff_size - writed, "--%s\r\n", r->boundary);
        
        switch (cur->value_type) {
            case NHR_MAP_VALUE_DATA:
                writed += nhr_sprintf(buff + writed, buff_size - writed, "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n", cur->key, cur->reserved.string);
                
                memcpy(buff + writed, "Content-Type: application/octet-stream\r\n", 40);
                writed += 40;
                
                memcpy(buff + writed, "Content-Transfer-Encoding: binary\r\n\r\n", 37);
                writed += 37;
                
                // contents of file
                memcpy(buff + writed, cur->value.data, cur->value_size);
                writed += cur->value_size;
                break;
                
            case NHR_MAP_VALUE_STRING:
            case NHR_MAP_VALUE_STATIC_STRING:
                writed += nhr_sprintf(buff + writed, buff_size - writed, "Content-Disposition: form-data; name=\"%s\"\r\n\r\n", cur->key);
                // text param
                memcpy(buff + writed, cur->value.string, cur->value_size);
                writed += cur->value_size;
                break;
                
            default:
                break;
        }
        cur = cur->next;
    }
    
    writed += nhr_sprintf(buff + writed, buff_size - writed, "\r\n--%s--", r->boundary);
    
    nhr_sprintf(content_type, (NHR_POST_BOUNDARY_LEN + 32), "multipart/form-data; boundary=%s", r->boundary);
    nhr_request_add_header_field(r, k_nhr_content_type, content_type);
    
    nhr_sprintf(content_length, 24, "%lu", (unsigned long)writed);
    nhr_request_add_header_field(r, k_nhr_content_length, content_length);
    
    buff[writed] = 0;
    
    *parameters_len = writed;
    return buff;
};

#endif // end POST DATA functionality

char * nhr_request_create_header_POST(nhr_request r, size_t * header_size) {
    size_t buff_size = 0, writed = 0, headers_len = 0, parameters_len = 0;
    char * buff = NULL, *headers = NULL;
    void * parameters = NULL;
    
    buff_size = strlen(r->path);
    buff_size += strlen(r->host);
    
#if defined(NHR_NO_POST_DATA)
    parameters = nhr_request_create_url_encoded_parameters_POST(r, &parameters_len);
#else
    parameters = r->is_have_data_parameter ? nhr_request_create_data_parameters_POST(r, &parameters_len) : nhr_request_create_url_encoded_parameters_POST(r, &parameters_len);
#endif
    if (parameters) buff_size += parameters_len;
    
    headers = r->http_headers ? nhr_request_http_headers(r->http_headers, &headers_len) : NULL;
    if (headers) buff_size += headers_len;
    
    buff_size += 1 << 8; // extra size for formatting strings
    
    buff = (char *)nhr_malloc(buff_size);
    
    writed = nhr_sprintf(buff, buff_size, "%s %s HTTP/%s\r\n", k_nhr_POST, r->path, k_nhr_request_http_ver);
    
    if (r->port == 80) {
        writed += nhr_sprintf(buff + writed, buff_size - writed, "Host: %s\r\n", r->host);
    } else {
        writed += nhr_sprintf(buff + writed, buff_size - writed, "Host: %s:%i\r\n", r->host, (int)r->port);
    }
    
    if (headers) {
        writed += nhr_sprintf(buff + writed, buff_size - writed, "%s\r\n\r\n", headers);
    } else {
        memcpy(buff + writed, k_nhr_CRLF, k_nhr_CRLF_length);
        writed += k_nhr_CRLF_length;
    }
    
    if (parameters_len > 0) {
        memcpy(buff + writed, parameters, parameters_len);
        writed += parameters_len;
        memcpy(buff + writed, k_nhr_double_CRLF, k_nhr_double_CRLF_length);
        writed += k_nhr_double_CRLF_length;
    }
    
    nhr_free(headers);
    nhr_free(parameters);
    buff[writed] = 0;
    
//    printf("\n\nHEADER POST: \n%s", buff);
    
    *header_size = writed;
    return buff;
}

#endif // end POST functionality
