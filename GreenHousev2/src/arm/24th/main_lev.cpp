#include <stdlib.h>
#include <lev.h>
#include <levhttp.h>
//#include <jansson.hpp>
#include <string.h>
#include <evhttp.h>
#include <b64/b64.h>
#include <jansson.h>
#include <ghairdefs.h>

#define ERROR_RESPONSE_SIZE 1024
#define MAX_JSON_BUFFER_SIZE 1024

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
#ifdef DEBUG
    fprintf(stdout, "Received %lu bytes\n", buffer_len);
    fflush(stdout);
#endif
    struct evbuffer *in_evb = evhttp_request_get_input_buffer(req);
    char buffer_data[MAX_JSON_BUFFER_SIZE];
    memset(buffer_data, 0, buffer_len);
    evbuffer_copyout(in_evb, buffer_data, buffer_len);

    // buffer_data contains the POST data passed in the json format.

    json_error_t json_error;
    json_t *json_root = json_loads(buffer_data, 0, &json_error);
    // free(buffer_data);

    if ( json_root == NULL ) {
        puts("Passed incorrect JSON data");
        puts(json_error.text);
        evreq.cancel();
        return;
    }

    json_t *js_func = json_object_get(json_root, "func");
    json_t *js_addr = json_object_get(json_root, "addr");
    json_t *js_len = json_object_get(json_root, "len");
    json_t *js_data = json_object_get(json_root, "data");

    AirPacket air_packet;

    bool packet_ready = true;


    if ( json_is_integer(js_func) && json_is_integer(js_addr) && json_is_integer(js_len) && json_is_string(js_data) ) {
        air_packet.command = json_integer_value(js_func);
        air_packet.address = json_integer_value(js_addr);
        air_packet.length = json_integer_value(js_len);

        char base64_data[MAX_JSON_BUFFER_SIZE];
        memset(base64_data, 0, json_string_length(js_data));
        strcpy(base64_data, json_string_value(js_data));
#ifdef DEBUG
        fprintf(stdout, "Raw data: %s\n", base64_data);
        fflush(stdout);
#endif
        size_t decoded_len;
        byte *decoded_data = b64_decode_ex(base64_data, strlen(base64_data), &decoded_len);

        memcpy(air_packet.data, decoded_data, decoded_len);

        free(decoded_data);
    } else {
        packet_ready = false;
    }

    free(js_func);
    free(js_addr);
    free(js_len);
    free(js_data);
#ifdef DEBUG
    fprintf(stdout, "Func: 0x%u, Addr: 0x%u, Len: 0x%u, Data: %s\n", air_packet.command, air_packet.address, air_packet.length, air_packet.data);
    fflush(stdout);
#endif
    
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

