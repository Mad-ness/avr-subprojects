#include <stdlib.h>
#include <lev.h>
#include <levhttp.h>
#include <string.h>
#include <evhttp.h>
//#include <b64.h>
// #include <jansson.h>
#include <ghairdefs.h>
// #include <data.h>
#include <iostream>
#include <string>
#include <ghair.h>
#include <unistd.h>
#include <packetmanager.h>
#include <routemanager.h>
//#include <nlohmann/json.hpp>
#include <proxy-api.h>

//#define ERROR_RESPONSE_SIZE 1024
//#define MAX_JSON_BUFFER_SIZE 1024


extern void printHandlers();
//void install_callbacks();
//void aadProxyCallback( const char *path, const URLParams_t &params, CallbackProxy_t cb );

using namespace lev;
//using json = nlohmann::json;
 
static int cnt = 0;
static EvEvent event_print;
static EvEvent checkRadio;

static PacketManager all_packets;
// PB11 = 43 - Chip Enable (CE)
// PH07 - 231 - Chip Select (CSN)

const int CEpin = SUNXI_GPB(10);
const int CSNpin = SUNXI_GPB(11);

GHAir air(CEpin, CSNpin, (byte*)"1Node", (byte*)"2Node");
RouteManager route_mg;


inline void logstr(std::string msg) {
#ifdef DEBUG
    std::cout << msg << std::endl;
#endif
}

/**
 * The function is called every time the data available in the receiver providing the air.loop()
 * is called.
 * A new packet is not processed anyhow, it is placed in the Packet Queue.
 */
static
void onIncomingAirPacket(AirPacket *pkt) {
#if defined(DEBUG)
    printf("In cmd: 0x%02x, resp: %d, datalen: %d (bytes)\n", pkt->getCommand(), pkt->isResponse(), pkt->length);
#endif 
    all_packets.updateWithResponse(*pkt);
}

static 
void onPrint(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    //all_packets.print();
    evreq.sendReply(200, "OK");
    route_mg.printInQueue();
}

/*
 * Check the radio module on incoming data
 */
static
void processIncomingQueue( evutil_socket_t socket, short id, void *data ) {
    air.loop();  // send responses, if any
}

static 
void onPing(struct evhttp_request *req, void *arg) {
    EvHttpRequest hreq(req);
    hreq.sendReply(200, "OK");
}

// static
void onPOSTRequest(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    size_t buffer_len = evbuffer_get_length(evhttp_request_get_input_buffer(req));
#ifdef DEBUG
    std::cout << "Received " << buffer_len << " bytes" << std::endl;
#endif
    std::cout << "Buffer len: " << buffer_len << std::endl;
    struct evbuffer *in_evb = evhttp_request_get_input_buffer(req);
    char *buffer_data;
    buffer_data = (char*)malloc(buffer_len);
    memset(buffer_data, ' ', sizeof(buffer_data));
    size_t copied_size = evbuffer_copyout(in_evb, buffer_data, buffer_len); 
    evbuffer_drain(in_evb, buffer_len);

/*
    // this is a trick
    // buffer_data sometimes comes with extra bytes
    // than it actually should have so the raw content has unprinable chars at the end.
    // here we copy as many chars as we actually received
    // (the problem might be in memory leaks)
    string s_buf;
    int i = 0;
    while ( i < buffer_len ) {
        s_buf += buffer_data[i++];
    }

    free(buffer_data);
    try {
        j = json::parse(s_buf);
    } catch (json::parse_error &e) {
        std::cout << e.what() << std::endl;
        string resp;
        resp += "{";
        resp += "\"status\":\"error\"";
        resp += "\"msg\":\"";
        resp += e.what(); 
        resp += "\"";
        evreq.output().printf(resp.c_str());
        evreq.sendReply(HTTP_BADREQUEST, "Bad JSON");
        return;
    }
    AirPacket airpkt;
    airpkt.command = j["cmd"];
    airpkt.address = j["addr"];
    airpkt.length = j["len"];
    if ( j["data"].is_number() ) {
        memcpy(&airpkt.data, &j["data"], airpkt.length);
    } else {
        strcpy((char*)airpkt.data, j["data"].get<std::string>().c_str());
    }

    UserPacket &pkt = all_packets.addRequest(UserPacket("testclient", airpkt));
    j["status"] = "accepted";
    j["packet_id"] = pkt.packetId();
    j["when_accepted"] = pkt.timeAddInQueue();
    j["is_response"] = pkt.airpacket().isResponse();
    evreq.output().printf(j.dump().c_str());
    std::cout << j.dump() << std::endl;
*/
    evreq.sendReply(200, "Request accepted");

    return;
}

static
void onHttpRequest(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    evreq.output().printf("Received URI: %s\n", evreq.uriStr());
    const evhttp_cmd_type cmd_type = evhttp_request_get_command( req );
    if ( cmd_type & EVHTTP_REQ_GET ) {
        evreq.sendReply(200, "ONLY POST ACCEPTED");
    } else if ( cmd_type & EVHTTP_REQ_POST ) {
        onPOSTRequest(req, arg);
    }
}

static
void onHttpDefault(struct evhttp_request *req, void *arg) {
    EvHttpRequest evreq(req);
    std::cout << "Default handler:: Received URI: " << evreq.uriStr() << std::endl;

    string route_msg;
    route_mg.accept(evreq.uriStr(), &route_msg);
    // std::cout << "DEBUG: preparing to send a response to the client\n";
    string m;
    m += "{\"requested_url\":\"";
    m += evreq.uriStr();
    m += "\",";
    m += "\"result\":";
    m += route_msg;
    m += "}";
    evreq.output().printf(m.c_str());
    evreq.sendReply(202, "Request accepted");
}

/**
 * It goes over all items in the packet queue and 
 * sends one request per call to the remote board.
 */
static
void processOutgoingQueue(evutil_socket_t socket, short id, void *data) {
    bool is_packet = false;
    UserPacket &pkt = all_packets.nextPacket(&is_packet);
    if ( is_packet ) {
        bool has_sent = air.sendPacket(pkt.airpacket());
        if ( has_sent ) {
            pkt.attemptedToSend(has_sent);
#if defined(DEBUG)
            std::cout << "Packet has been shipped out: " << pkt.str() << std::endl;
#endif 
        } else { 
#if defined(DEBUG)
            std::cout << "Packet has not been shipped: " << pkt.str() << std::endl;
#endif 
        }
    }
}

void usage() {
    std::cout <<
    "Usage: <ip-address> <port>\n"
    "    - ip-address - a local IP address which will be listened to\n"
    "    - port       - a local port to bind on\n"
    "\n"
    "Example:\n"
    "    cmd 127.0.0.1 8080 - run a server on localhost:8080\n"
    << std::endl;

}


void info() {
    std::cout << "Registered handlers:" << std::endl;
    printHandlers();
}


int main(int argc, char **argv) {

    install_callbacks();

    if ( argc == 1 ) {
        usage();
        info();
        return 0;
    }

    air.setup();
    air.setHandler(onIncomingAirPacket); // set handler for incoming packets over radio
    int isConnected = false;
    for ( int i = 0; i < 100; i++ ) {
        if ( ! air.rf24()->isChipConnected() ) {
#ifdef DEBUG
            logstr("Chip is not connected. Checking again ..."); 
#endif
            sleep(1);
        } else {
            isConnected = true;
            break;
        }
    }
    if ( ! isConnected ) {
        return 1;
    }

    air.rf24()->printDetails();

    printf("Starting the server.\n");
    EvBaseLoop base;
    EvHttpServer http(base);

    EvEvent inQueue, outQueue;

    logstr("Installing queue processors");
    inQueue.newTimer(processIncomingQueue, base.base());
    outQueue.newTimer(processOutgoingQueue, base.base());

    inQueue.start(10);
    outQueue.start(200);

    logstr("Adding routes");
    http.addRoute("/request", onHttpRequest);
    http.addRoute("/ping", onPing);
    http.addRoute("/print", onPrint);
    http.addRoute("/debug", onPrint);
    http.setDefaultRoute(onHttpDefault);

    logstr("Running the http server");
    http.bind(argv[1], atoi(argv[2]));
    logstr("Entering in the loop");
    base.loop();

    return 0;
}

