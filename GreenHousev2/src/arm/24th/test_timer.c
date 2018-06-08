#include <stdlib.h>
#include <event2/event.h>

static int cnt = 0;
void cb_func(evutil_socket_t fd, short what, void *arg)
{
        const char *data = (const char *)arg;
        fprintf(stdout, "Got an event on socket %d:%s%s%s%s [%s]\n",
            (int) fd,
            (what&EV_TIMEOUT) ? " timeout" : "",
            (what&EV_READ)    ? " read" : "",
            (what&EV_WRITE)   ? " write" : "",
            (what&EV_SIGNAL)  ? " signal" : "",
            data);
        fflush(stdout);
        if ( cnt++ > 3 ) 
            exit(1);
}

void main_loop(evutil_socket_t fd1, evutil_socket_t fd2)
{
        struct event *ev1, *ev2;
        struct timeval five_seconds = {5,0};
        struct event_base *base = event_base_new();

        /* The caller has already set up fd1, fd2 somehow, and make them
           nonblocking. */

        ev1 = event_new(base, fd1, EV_TIMEOUT|EV_PERSIST, cb_func,
           (char*)"Reading event");
        ev2 = event_new(base, fd2, EV_TIMEOUT|EV_PERSIST, cb_func,
           (char*)"Writing event");

        event_add(ev1, &five_seconds);
        event_add(ev2, NULL);
        event_base_dispatch(base);
}

int main(int argc, char **argv) {
    main_loop(-1, -1);
    return 0;
}

