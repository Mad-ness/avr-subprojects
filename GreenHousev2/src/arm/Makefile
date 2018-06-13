SRCS=main.cpp b64/encode.c b64/decode.c 
LDFLAGS=-levent -ljansson -lstdc++
CXXFLAGS=-std=c++11 -DDEBUG -I/usr/local/include -L/usr/local/lib -I./include -I./b64 -I. -I./janssonxx -I../../lib/ghair
PROG=http_server


all: data.o
	g++ ${SRCS} data.o -o ${PROG} ${LDFLAGS} ${CXXFLAGS}

data.o: 
	g++ -c data.cpp -o data.o ${LDFLAGS} ${CXXFLAGS}

clean:
	rm -f ${PROG} data.o



