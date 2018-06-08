INCLUDE=-I/usr/local/include -I/usr/local/include/RF24 -I./lib/ghair -I./src/arm
LIBS=-L/usr/local/lib -lrf24
CFLAGS= -fPIC
CCFLAGS=
CC=g++ $(LIBS) $(INCLUDE) $(CFLAGS) $(CCFLAGS)
BUILD_DIR=arm/build
LIBVERS=1.0.0



main: ghair.o main.o
	$(CC) $(BUILD_DIR)/main.o $(BUILD_DIR)/ghair.o -o rf24_server

ghair.o: prepare
	$(CC) -c lib/ghair/ghair.cpp -o $(BUILD_DIR)/ghair.o

main.o: prepare
	$(CC) -c src/arm/main.cpp -o $(BUILD_DIR)/main.o

prepare:
	mkdir -p $(BUILD_DIR)

build_lib: ghair.o
	$(CC) -shared -Wl,-soname,libghair.so -o libghair.so.$(LIBVERS) $(BUILD_DIR)/ghair.o

clean:
	rm -rf $(BUILD_DIR) libghair.so.$(LIBVERS)

