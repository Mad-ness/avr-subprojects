#include <stdlib.h>
#include <lev.h>
#include <levhttp.h>
//#include <jansson.hpp>
#include <string.h>
#include <evhttp.h>
#include <b64/b64.h>
#include <jansson.h>

#define ERROR_RESPONSE_SIZE 1024

using namespace lev;

static int cnt = 0;
static EvEvent event_print;

/*
Example of a message that comes at the http port:
{
    "command": <unsigned int>,
    "address": <unsigned int>,
    "datalength": <unsigned int>,
    "data": <base64 string>
}
The "data" value while converting to a data type should not exceed AIR_MAX_DATA_SIZE bytes (see in the ghairdefs.h).
*/

static 
void onPing(struct evhttp_request *req, void *arg) {
    EvHttpRequest hreq(req);
    hreq.sendReply(200, "OK");
}

static
void onPOSTRequest(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    size_t buffer_len = evbuffer_get_length(evhttp_request_get_input_buffer(req));
    fprintf(stdout, "Received %d bytes\n", buffer_len);
    fflush(stdout);
    struct evbuffer *in_evb = evhttp_request_get_input_buffer(req);
    char buffer_data[400];
    memset(buffer_data, 0, buffer_len);
    evbuffer_copyout(in_evb, buffer_data, buffer_len);

    // buffer_data contains the POST data passed in the json format.

    json_error_t json_error;
    json_t *json_root = json_loads(buffer_data, 0, &json_error);
    // free(buffer_data);

    if ( json_root == NULL ) {
        puts("Passed incorrect JSON data");
        evreq.cancel();
        return;
    }

    json_t *js_func = json_object_get(json_root, "func");
    json_t *js_addr = json_object_get(json_root, "addr");
    json_t *js_len = json_object_get(json_root, "len");
    json_t *js_data = json_object_get(json_root, "data");

    int value_func = 0x0;
    if ( 1 ) {
    // if ( json_is_integer(js_func) ) {
        value_func = json_integer_value(js_func);
        puts("Decoding the function value");
    }
    int value_addr = 0x0;
    if ( 1 ) {
    // if ( json_is_integer(js_addr) ) {
        value_addr = json_integer_value(js_addr);
        puts("Decoding the address value");
    } 
    int value_len = 0x0;
    if ( 1 ) {
    // if ( json_is_integer(js_len) ) {
        value_len = json_integer_value(js_len);
        puts("Decoding the len value");
    }
    const char *value_data;
    if ( json_is_string(js_data) ) {
        value_data = json_string_value(js_data);
    }
    fprintf(stdout, "Func: 0x%u, Addr: 0x%u, Len: 0x%u, Data: %s\n", value_func, value_addr, value_len, value_data);
    fflush(stdout);

    free(js_func);
    free(js_addr);
    free(js_len);
    free(js_data);
    free(json_root);
    evreq.sendReply(200, "OK Good Json");
}

static
void onHttpRequest(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    evreq.output().printf("Received URI: %s\n", evreq.uriStr());
    const evhttp_cmd_type cmd_type = evhttp_request_get_command( req );
    if ( cmd_type & EVHTTP_REQ_GET ) {
        evreq.sendReply(200, "GOOD");
    } else if ( cmd_type & EVHTTP_REQ_POST ) {
        onPOSTRequest(req, arg);
    }
}

static
void onHttpHello(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    evreq.output().printf("<h1>Hello, World!</h1>");
    char msg[] = "{ \"command\": \"4\", \"address\": \"10\" }";
    evreq.output().printf(msg);
    evreq.sendReply(200, "OK");
}

static
void onTimeout(evutil_socket_t socket, short id, void *data) {
    puts("Timeout expired!");
    //fprintf(stdout, "Timeout expired!\n");
    //fflush(stdout);
}

int main(int argc, char **argv) {
    printf("Starting the server.\n");
    EvBaseLoop base;
    EvHttpServer http(base);

    //EvBaseLoop evbase;

    event_print.newTimer(onTimeout, base.base());
    event_print.start(3000);

    http.addRoute("/test", onHttpRequest);
    http.addRoute("/ping", onPing);
    http.addRoute("/hello", onHttpHello);

    http.bind("127.0.0.1", 8080);
    base.loop();

    return 0;
}

