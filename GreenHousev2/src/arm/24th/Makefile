SRCS=main.cpp
LDFLAGS=-levent
CXXFLAGS=-I/usr/local/include -L/usr/local/lib
PROG=http_server

all:
	c++ ${SRCS} -o ${PROG} ${LDFLAGS} ${CXXFLAGS}

clean:
	rm -f ${PROG}

