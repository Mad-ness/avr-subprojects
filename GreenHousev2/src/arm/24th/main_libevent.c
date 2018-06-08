/* Derived from sample/http-server.c in libevent source tree.
 * That file does not have a license notice, but generally libevent
 * is under the 3-clause BSD.
 *
 * Plus, some additional inspiration from:
 * http://archives.seul.org/libevent/users/Jul-2010/binGK8dlinMqP.bin
 * (which is a .c file despite the extension and mime type) */

/*
  A trivial https webserver using Libevent's evhttp.

  This is not the best code in the world, and it does some fairly stupid stuff
  that you would never want to do in a production webserver. Caveat hackor!

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <evhttp.h>

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif

#ifdef _WIN32
#define stat _stat
#define fstat _fstat
#define open _open
#define close _close
#define O_RDONLY _O_RDONLY
#endif

#include <jansson.h>

#define HEADER_BUFFER_SIZE 1024
#define ERROR_RESPONSE_SIZE 1024

void brokenPipe(int signum);
void jsonRequestHandler(struct evhttp_request *request, void *arg);

void sendErrorResponse(struct evhttp_request *request, char *errorText);
void sendJSONResponse(struct evhttp_request *request, json_t *requestJSON, struct event_base *base);

unsigned short serverPort = 10080;



void jsonRequestHandler(struct evhttp_request *request, void *arg) {

   struct event_base *base = (struct event_base *)arg;
   // Request
   struct evbuffer *requestBuffer;
   size_t requestLen;
   char *requestDataBuffer;
   const char *uri = evhttp_request_get_uri (request);

   json_t *requestJSON;
   json_error_t error;

   // Error buffer
   char errorText[ERROR_RESPONSE_SIZE];

   // Process Request
   requestBuffer = evhttp_request_get_input_buffer(request);
   requestLen = evbuffer_get_length(requestBuffer);

   requestDataBuffer = (char *)malloc(sizeof(char) * requestLen);
   memset(requestDataBuffer, 0, requestLen);
   evbuffer_copyout(requestBuffer, requestDataBuffer, requestLen);

   
   requestJSON = json_loadb(requestDataBuffer, requestLen, 0, &error);
   evhttp_cmd_type passed_cmd = evhttp_request_get_command( request );
   switch ( passed_cmd ) {
        case EVHTTP_REQ_POST:

            break;
        case EVHTTP_REQ_GET:

            break;
        default:
            printf("Other command received %d\n", passed_cmd);
            break;
   }

   free(requestDataBuffer);
   printf("%s\n", evhttp_request_uri(request));


   if (requestJSON == NULL) {
      snprintf(errorText, ERROR_RESPONSE_SIZE, "Input error: on line %d: %s\n", error.line, error.text);
      sendErrorResponse(request, errorText);
   } else {
      // Debug out
      requestDataBuffer = json_dumps(requestJSON, JSON_INDENT(3));
      printf("%s\n", requestDataBuffer);
      free(requestDataBuffer);

      sendJSONResponse(request, requestJSON, base);
      json_decref(requestJSON);
   }
   return;
}


/* Instead of casting between these types, create a union with all of them,
 * to avoid -Wstrict-aliasing warnings. */
typedef union
{ struct sockaddr_storage ss;
  struct sockaddr sa;
  struct sockaddr_in in;
  struct sockaddr_in6 i6;
} sock_hop;


/* This callback gets invoked when we get any http request that doesn't match
 * any other callback.  Like any evhttp server callback, it has a simple job:
 * it must eventually call evhttp_send_error() or evhttp_send_reply().
 */
static void
send_document_cb (struct evhttp_request *req, void *arg)
{ struct evbuffer *evb = NULL;
  const char *uri = evhttp_request_get_uri (req);
  struct evhttp_uri *decoded = NULL;

  if (evhttp_request_get_command (req) == EVHTTP_REQ_GET)
    {
      struct evbuffer *buf = evbuffer_new();
      if (buf == NULL) return;
      evbuffer_add_printf(buf, "Requested: %s\n", uri);
      evhttp_send_reply(req, HTTP_OK, "OK", buf);
      return;
    }

  /* We only handle POST requests. */
  if (evhttp_request_get_command (req) != EVHTTP_REQ_POST)
    { evhttp_send_reply (req, 200, "OK", NULL);
      return;
    }

  printf ("Got a POST request for <%s>\n", uri);

  /* Decode the URI */
  decoded = evhttp_uri_parse (uri);
  if (! decoded)
    { printf ("It's not a good URI. Sending BADREQUEST\n");
      evhttp_send_error (req, HTTP_BADREQUEST, 0);
      return;
    }

  evb = evbuffer_new ();

  evhttp_add_header (evhttp_request_get_output_headers (req),
                     "Content-Type", "application/x-yaml");

  evhttp_send_reply (req, 200, "OK", evb);

  if (evb)
    evbuffer_free (evb);
}



void event_handler(int fd, short event, void *arg) {
  if ( event & EV_TIMEOUT ) {
    printf("timeout\n");
  }
}



static int serve_some_http (void)
{ 
  struct event_base *base;
  struct evhttp *http;
  struct evhttp_bound_socket *handle;

  base = event_base_new ();
  if (! base)
    { fprintf (stderr, "Couldn't create an event_base: exiting\n");
      return 1;
    }

  /* Create a new evhttp object to handle requests. */
  http = evhttp_new (base);
  if (! http)
    { fprintf (stderr, "couldn't create evhttp. Exiting.\n");
      return 1;
    }

  /* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
   * We just hardcode a single curve which is reasonably decent.
   * See http://www.mail-archive.com/openssl-dev@openssl.org/msg30957.html */

  /* Find and set up our server certificate. */
  /* This is the callback that gets called when a request comes in. */
  // evhttp_set_gencb (http, send_document_cb, NULL);
  evhttp_set_gencb(http, jsonRequestHandler, (void *)base);

  /* Now we tell the evhttp what port to listen on */
  handle = evhttp_bind_socket_with_handle (http, "0.0.0.0", serverPort);
  if (! handle)
    { fprintf (stderr, "couldn't bind to port %d. Exiting.\n",
               (int) serverPort);
      return 1;
    }

  { /* Extract and display the address we're listening on. */
    sock_hop ss;
    evutil_socket_t fd;
    ev_socklen_t socklen = sizeof (ss);
    char addrbuf[128];
    void *inaddr;
    const char *addr;
    int got_port = -1;
    fd = evhttp_bound_socket_get_fd (handle);
    memset (&ss, 0, sizeof(ss));
    if (getsockname (fd, &ss.sa, &socklen))
      { perror ("getsockname() failed");
        return 1;
      }
    if (ss.ss.ss_family == AF_INET)
      { got_port = ntohs (ss.in.sin_port);
        inaddr = &ss.in.sin_addr;
      }
    else if (ss.ss.ss_family == AF_INET6)
      { got_port = ntohs (ss.i6.sin6_port);
        inaddr = &ss.i6.sin6_addr;
      }
    else
      { fprintf (stderr, "Weird address family %d\n", ss.ss.ss_family);
        return 1;
      }
    addr = evutil_inet_ntop (ss.ss.ss_family, inaddr, addrbuf,
                             sizeof (addrbuf));
    if (addr)
      printf ("Listening on %s:%d\n", addr, got_port);
    else
      { fprintf (stderr, "evutil_inet_ntop failed\n");
        return 1;
      }
  }

  struct event_base *ev_base;
  struct event *ev;
  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  
  event_init();

  ev_base = event_base_new();
  ev = event_new(ev_base, 
                 -1,
                 EV_TIMEOUT | EV_PERSIST,
                 event_handler,
                 NULL);
  event_add(ev, &tv);
  event_base_dispatch (base);

  event_free(ev);
  event_base_free(ev_base);

  /* not reached; runs forever */

  return 0;
}

int main (int argc, char **argv)
{ 

  if (argc > 1) {
    char *end_ptr;
    long lp = strtol(argv[1], &end_ptr, 0);
    if (*end_ptr) {
      fprintf(stderr, "Invalid integer\n");
      return -1;
    }
    if (lp <= 0) {
      fprintf(stderr, "Port must be positive\n");
      return -1;
    }
    if (lp >= USHRT_MAX) {
      fprintf(stderr, "Port must fit 16-bit range\n");
      return -1;
    }

    serverPort = (unsigned short)lp;
  }

  /* now run http server (never returns) */
  return serve_some_http ();
}


void sendErrorResponse(struct evhttp_request *request, char *errorText) {
   // Reponse
   char responseHeader[HEADER_BUFFER_SIZE];
   size_t responseLen;

   struct evbuffer *responseBuffer;
   responseLen = strlen(errorText);
   responseBuffer = evbuffer_new();
   // content length to string
   sprintf(responseHeader, "%d", (int)responseLen);
   evhttp_add_header(request->output_headers, "Content-Type", "text/plain");
   evhttp_add_header(request->output_headers, "Content-Length", responseHeader);

   evbuffer_add(responseBuffer, errorText, responseLen);
   evhttp_send_reply(request, 400, "Bad JSON", responseBuffer); 
   evbuffer_free(responseBuffer);
}


void sendJSONResponse(struct evhttp_request *request, json_t *requestJSON, struct event_base *base) {
   // Reponse
   json_t *responseJSON;
   json_t *message;
   const char *uri = evhttp_request_get_uri (request);

   char responseHeader[HEADER_BUFFER_SIZE];

   char *responseData;
   int responseLen;
   struct evbuffer *responseBuffer;

   // Create JSON response data
   responseJSON = json_object();


   if ( strcmp(uri, "/news") == 0 ) {
       message = json_string("This is /news");
   } else if ( strcmp(uri, "/info") == 0 ) {
       message = json_string("This is /info");
   } else {
       message = json_string("Hello World. /news and /info available");
   }

   json_object_set_new(responseJSON, "message", message);

   // dump JSON to buffer and store response length as string
   responseData = json_dumps(responseJSON, JSON_INDENT(3));
   responseLen = strlen(responseData);
   snprintf(responseHeader, HEADER_BUFFER_SIZE, "%d", (int)responseLen);
   json_decref(responseJSON);

   // create a response buffer to send reply
   responseBuffer = evbuffer_new();

   // add appropriate headers
   evhttp_add_header(request->output_headers, "Content-Type", "application/json");
   evhttp_add_header(request->output_headers, "Content-Length", responseHeader);

   evbuffer_add(responseBuffer, responseData, responseLen);

   // send the reply
   evhttp_send_reply(request, 200, "OK", responseBuffer); 

   evbuffer_free(responseBuffer);
}
