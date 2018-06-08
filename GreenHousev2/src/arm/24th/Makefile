SRCS=main.cpp b64/encode.c b64/decode.c
LDFLAGS=-levent -ljansson
CXXFLAGS=-I/usr/local/include -L/usr/local/lib -I./include -I./b64 -I. -I./janssonxx -I../../../lib/ghair
PROG=http_server

all:
	c++ ${SRCS} -o ${PROG} ${LDFLAGS} ${CXXFLAGS}

clean:
	rm -f ${PROG}



