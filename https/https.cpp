#define MBED_CONF_MBED_TRACE_ENABLE 1

#include "mbed.h"
#include "mbed_trace.h"
#include "https_request.h"
#include "network-helper.h"

/* List of trusted root CA certificates
 * currently one: Amazon Root CA 1
 * To add more root certificates use e.g.
 *    openssl s_client -showcerts -connect httpbin.org:443
 * to get it just concatenate -----BEGIN CERTIFICATE----- ... -----END CERTIFICATE-----
 * to the string
 *
 */
  // Amazon Root CA 1
  const char SSL_CA_PEM[] =   "-----BEGIN CERTIFICATE-----\n"
  "MIIEkjCCA3qgAwIBAgITBn+USionzfP6wq4rAfkI7rnExjANBgkqhkiG9w0BAQsF\n"
  "ADCBmDELMAkGA1UEBhMCVVMxEDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNj\n"
  "b3R0c2RhbGUxJTAjBgNVBAoTHFN0YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4x\n"
  "OzA5BgNVBAMTMlN0YXJmaWVsZCBTZXJ2aWNlcyBSb290IENlcnRpZmljYXRlIEF1\n"
  "dGhvcml0eSAtIEcyMB4XDTE1MDUyNTEyMDAwMFoXDTM3MTIzMTAxMDAwMFowOTEL\n"
  "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
  "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
  "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
  "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
  "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
  "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
  "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
  "jgSubJrIqg0CAwEAAaOCATEwggEtMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/\n"
  "BAQDAgGGMB0GA1UdDgQWBBSEGMyFNOy8DJSULghZnMeyEE4KCDAfBgNVHSMEGDAW\n"
  "gBScXwDfqgHXMCs4iKK4bUqc8hGRgzB4BggrBgEFBQcBAQRsMGowLgYIKwYBBQUH\n"
  "MAGGImh0dHA6Ly9vY3NwLnJvb3RnMi5hbWF6b250cnVzdC5jb20wOAYIKwYBBQUH\n"
  "MAKGLGh0dHA6Ly9jcnQucm9vdGcyLmFtYXpvbnRydXN0LmNvbS9yb290ZzIuY2Vy\n"
  "MD0GA1UdHwQ2MDQwMqAwoC6GLGh0dHA6Ly9jcmwucm9vdGcyLmFtYXpvbnRydXN0\n"
  "LmNvbS9yb290ZzIuY3JsMBEGA1UdIAQKMAgwBgYEVR0gADANBgkqhkiG9w0BAQsF\n"
  "AAOCAQEAYjdCXLwQtT6LLOkMm2xF4gcAevnFWAu5CIw+7bMlPLVvUOTNNWqnkzSW\n"
  "MiGpSESrnO09tKpzbeR/FoCJbM8oAxiDR3mjEH4wW6w7sGDgd9QIpuEdfF7Au/ma\n"
  "eyKdpwAJfqxGF4PcnCZXmTA5YpaP7dreqsXMGz7KQ2hsVxa81Q4gLv7/wmpdLqBK\n"
  "bRRYh5TmOTFffHPLkIhqhBGWJ6bt2YFGpn6jcgAKUj6DiAdjd4lpFw85hdKrCEVN\n"
  "0FE6/V1dN2RMfjCyVSRCnTawXZwXgWHxyvkQAiSr6w10kY17RSlQOYiypok1JR4U\n"
  "akcjMS9cmvqtmg5iUaQqqcT5NJ0hGA==\n"
  "-----END CERTIFICATE-----\n";
  
void dump_response(HttpResponse* res) {
  printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

  printf("Headers:\n");
  for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
    printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
  }
  printf("\nBody (%u bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

int main() {
  NetworkInterface* network = connect_to_default_network_interface();
  if (!network) {
    printf("Cannot connect to the network, see serial output\n");
    return 1;
  }

  mbed_trace_init();

  // GET request
  printf("\n----- HTTPS GET request -----\n");
  HttpsRequest* get_req = new HttpsRequest(network, SSL_CA_PEM, HTTP_GET, "https://httpbin.org/get");
  HttpResponse* get_res = get_req->send();
  if (!get_res) {
    printf("HttpRequest failed (error code %d)\n", get_req->get_error());
  } else {
    printf("\n----- HTTPS GET response -----\n");
    dump_response(get_res);
  }
  delete get_req;

  
  // POST request to httpbin.org
  printf("\n----- HTTPS POST request -----\n");
  HttpsRequest* post_req = new HttpsRequest(network, SSL_CA_PEM, HTTP_POST, "https://httpbin.org/post");
  post_req->set_header("Content-Type", "application/json");
  const char body[] = "{\"hello\":\"world\"}";

  HttpResponse* post_res = post_req->send(body, strlen(body));
  if (!post_res) {
    printf("HttpRequest failed (error code %d)\n", post_req->get_error());
  } else {
    printf("\n----- HTTPS POST response -----\n");
    dump_response(post_res);
  }
  delete post_req;

  while(1) thread_sleep_for(1000);
}
