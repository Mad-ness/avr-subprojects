#include <stdlib.h>
#include <lev.h>
#include <levhttp.h>
#include <string.h>
#include <evhttp.h>
#include <b64.h>
#include <jansson.h>
#include <ghairdefs.h>
#include <data.h>
#include <iostream>
#include <string>



#define ERROR_RESPONSE_SIZE 1024
#define MAX_JSON_BUFFER_SIZE 1024

using namespace lev;
 
static int cnt = 0;
static EvEvent event_print;
static EvEvent checkRadio;

static DataCollector all_data;

static 
void onPrint(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    all_data.printContent();
    evreq.sendReply(200, "OK");
}

/*
 * Check the radio module on incoming data
 */
static
void onCheckFromRadio( evutil_socket_t socket, short id, void *data ) {
    //
    AirPacket *pkt;
    if ( pkt != NULL ) {
	if ( pkt->isResponse() ) {
            switch ( pkt->getCommand() ) {
     
                case AIR_CMD_IN_PING: { // mark the remote board as alive
                    break;
                }
            }
	// this is not a response, the remote boards requests this one
	} else {
            // it responds on the pings automatically
            // other handlers can be described here
	}
    }
}

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

    //AirPacket air_packet;

    bool packet_ready = true;


    if ( json_is_integer(js_func) && json_is_integer(js_addr) && json_is_integer(js_len) && json_is_string(js_data) ) {

        HttpRequest_t datapkt;
        datapkt.packet.address = json_integer_value(js_addr);
        datapkt.packet.length = json_integer_value(js_len);
	datapkt.packet.command = json_integer_value(js_func);

        char base64_data[MAX_JSON_BUFFER_SIZE];
        memset(base64_data, 0, json_string_length(js_data));
        strcpy(base64_data, json_string_value(js_data));
        size_t decoded_len;
        byte *decoded_data = b64_decode_ex(base64_data, strlen(base64_data), &decoded_len);

        memcpy(&datapkt.packet.data, decoded_data, decoded_len);

        free(decoded_data);

        //memcpy(&datapkt.packet, &air_packet, sizeof(air_packet));

#ifdef DEBUG
        std::cout << "Func: " << (int)datapkt.packet.command << ", " 
                  << "Addr: " << (int)datapkt.packet.address << ", " 
                  << "Len: " << (int)datapkt.packet.length << ", "
                  << "Data: " << datapkt.packet.data;
	std::cout << "Raw data: " << base64_data << std::endl;;
#endif 
        strcpy(datapkt.id, "super");
        all_data.addRequest(datapkt);
    } else {
        packet_ready = false;
    }

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

    // all_data
    for ( auto it = all_data.data().cbegin(); it != all_data.data().cend(); it++ ) {
        if ( it->responded == 0 ) {
            // send request to the remote board using the RF24 (GHAir lib)
            break; // send only one request per event
        }
    }
}


int main(int argc, char **argv) {
    printf("Starting the server.\n");
    EvBaseLoop base;
    EvHttpServer http(base);


    event_print.newTimer(onTimeout, base.base());
    event_print.start(3000);

    checkRadio.newTimer(onCheckFromRadio, base.base());
    checkRadio.start(10);



    http.addRoute("/test", onHttpRequest);
    http.addRoute("/ping", onPing);
    http.addRoute("/hello", onHttpHello);
    http.addRoute("/print", onPrint);


    http.bind("127.0.0.1", 8080);
    base.loop();

    return 0;
}

