#include "mbed.h"
#include "http_request.h"
#include "network-helper.h"
#include "mbed_mem_trace.h"

#include "mbed_fault_handler.h"
mbed_fault_context_t mbed_fault_context;

void dump_response(HttpResponse* res) {
  printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

  printf("Headers:\n");
  for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
    printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
  }
  printf("\nBody (%d bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

int main() {
  // Connect to the network with the default networking interface
  // if you use WiFi: see mbed_app.json for the credentials
  NetworkInterface* net = connect_to_default_network_interface();
  TCPSocket socket;
  SocketAddress adr;
  nsapi_error_t result;
  HttpRequest* get_req;
  HttpResponse* get_res;
  HttpRequest* post_req;
  HttpResponse* post_res;
  const char body[] = "{\"hello\":\"world\"}";
  
  if (!net) {
    printf("Cannot connect to the network, see serial output\n");
    goto end;
  }
  
  result = net->gethostbyname("httpbin.org", &adr);
  if (result != NSAPI_ERROR_OK ) {
    printf("Error! net->gethostbyname returned: %d\n", result);
    goto end;
  }
  adr.set_port(80);

  result = socket.open(net);
  if (result != NSAPI_ERROR_OK) {
    printf("Error! socket.open(net) returned: %d\n", result);
    goto end;
  }

  result = socket.connect(adr);
  if (result != NSAPI_ERROR_OK) {
    printf("Error! socket.connect(adr) Failed (%d).\n", result);
    goto end;
  }


  // Do a GET request to httpbin.org
  // By default the body is automatically parsed and stored in a buffer, this is memory heavy.
  // To receive chunked response, pass in a callback as last parameter to the constructor.
  get_req = new HttpRequest(&socket, HTTP_GET, "http://httpbin.org/get");

  get_res = get_req->send();
  if (!get_res) {
    printf("HttpRequest failed (error code %d)\n", get_req->get_error());
  } else {
    printf("\n----- HTTP GET response -----\n");
    dump_response(get_res);
  }
  delete get_req;

  // POST request to httpbin.org
  post_req = new HttpRequest(&socket, HTTP_POST, "http://httpbin.org/post");
  post_req->set_header("Content-Type", "application/json");

  post_res = post_req->send(body, strlen(body));
  if (!post_res) {
    printf("HttpRequest failed (error code %d)\n", post_req->get_error());
  } else {
    printf("\n----- HTTP POST response -----\n");
    dump_response(post_res);
  }
  delete post_req;

  end:
  while(1) thread_sleep_for(1000);
}
