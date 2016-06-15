

#import "ViewController.h"
#include "libnhr.h"
#include "nhr_gz.h"

@interface ViewController ()

@end

@implementation ViewController

static void onError(nhr_request request, nhr_error_code error_code) {
	printf("\nResponce error: %i", (int)error_code);
}

static void onResponse(nhr_request request, nhr_response responce) {
	printf("\nResponce:\n");
	char * body = nhr_response_get_body(responce);
	unsigned int bodyLength = nhr_response_get_body_length(responce);
	if (body && bodyLength)
	{
		FILE * f = fopen("/Volumes/Data/1/responce", "w+b");
		for (int i = 0; i < bodyLength; i++) {
			printf("%c", body[i]);
			if (f) fputc(body[i], f);
		}
		if (f) fclose(f);
	}
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	nhr_request request = nhr_request_create();

//	nhr_request_set_url(request, "http", "api.ipify.org", "/", 80);
//	nhr_request_set_url(request, "http", "isithackday.com", "/arrpi.php", 80);
	nhr_request_set_url(request, "http", "www.tutorialspoint.com", "/http/http_header_fields.htm", 80);
//	nhr_request_set_url(request, "http", "httpbin.org", "/deflate", 80);
//	nhr_request_set_url(request, "http", "httpbin.org", "/gzip", 80);
//	nhr_request_set_url(request, "http", "requestb.in", "/1agvbet1", 80);

//	nhr_request_set_method(request, nhr_method_GET);
	nhr_request_set_method(request, nhr_method_POST);

	nhr_request_add_header_field(request, "Cache-control", "no-cache");
	nhr_request_add_header_field(request, "User-Agent", "iOS");
//	nhr_request_add_header_field(request, "Accept", "text/html");
	nhr_request_add_header_field(request, "Accept-Charset", "utf-8");
//	nhr_request_add_header_field(request, "Accept-Encoding", k_nhr_gzip_deflate);
//	nhr_request_add_header_field(request, k_nhr_content_encoding, k_nhr_deflate);
	nhr_request_add_header_field(request, k_nhr_content_encoding, k_nhr_gzip);

	nhr_request_add_parameter(request, "format", "json");
	nhr_request_add_parameter(request, "text", "Hello%20world");

	nhr_request_set_on_recvd_responce(request, &onResponse);
	nhr_request_set_on_error(request, &onError);
	nhr_request_send(request);
}

- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning];
	// Dispose of any resources that can be recreated.
}

@end
