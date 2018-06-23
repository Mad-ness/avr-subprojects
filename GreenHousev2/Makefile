INCLUDE=-I/usr/local/lib -I. -I./lib/ghair -I./src/arm -I./src/arm/include -I
# OS=LINUX
# SOC=A20
RF24_INCL=-I/var/tmp/librf24-sunxi/src
RF24_LIBS=-L/usr/local/lib -lrf24-sunxi
JSON_INCL=-I./src/arm/nlohmann-json/include
LIBS=-L/usr/local/lib -L/usr/lib -levent -ljansson
CFLAGS=
# NDEBUG is for the JSON library
CXXFLAGS=-DDEBUG -D__LINUX__ -DCUBIEBOARD2 -DGPIO_SUN7I -DNDEBUG
CC=g++ -std=c++11 $(INCLUDE) $(CFLAGS) $(CXXFLAGS)
BUILD_DIR=./arm/build
TARGET=idea24
TESTS_DIR=src/arm/tests

all: $(TARGET)


pmtest: $(BUILD_DIR)/packetmanager.o $(BUILD_DIR)/pmtest.o
	$(CC) $(BUILD_DIR)/pmtest.o $(BUILD_DIR)/packetmanager.o -o $@ $(RF24_INCL)

$(BUILD_DIR)/pmtest.o: $(TESTS_DIR)/pmtest.cpp $(BUILD_DIR)/packetmanager.o
	$(CC) -c $(TESTS_DIR)/pmtest.cpp -o $@ $(RF24_INCL)

$(TARGET):  $(BUILD_DIR)/ghair.o \
		    $(BUILD_DIR)/data.o \
            $(BUILD_DIR)/packetmanager.o \
		    $(BUILD_DIR)/b64encode.o \
		    $(BUILD_DIR)/b64decode.o \
		    $(BUILD_DIR)/main.o
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(RF24_LIBS)


$(BUILD_DIR)/ghair.o: lib/ghair/ghair.cpp lib/ghair/ghairdefs.h lib/ghair/ghair.h
	$(CC) -c lib/ghair/ghair.cpp -o $@ $(RF24_INCL)

$(BUILD_DIR)/data.o: src/arm/data.cpp src/arm/data.h
	$(CC) -c src/arm/data.cpp -o $@

$(BUILD_DIR)/b64encode.o: src/arm/b64.h src/arm/encode.c
	$(CC) -c src/arm/encode.c -o $@

$(BUILD_DIR)/b64decode.o: src/arm/b64.h src/arm/decode.c
	$(CC) -c src/arm/decode.c -o $@

$(BUILD_DIR)/packetmanager.o: src/arm/packetmanager.cpp src/arm/packetmanager.h lib/ghair/ghairdefs.h ./src/arm/nlohmann-json/single_include/nlohmann/json.hpp
	$(CC) -c src/arm/packetmanager.cpp -o $@ $(RF24_INCL) $(JSON_INCL)


$(BUILD_DIR)/main.o: src/arm/main.cpp lib/ghair/ghairdefs.h src/arm/data.h ./src/arm/nlohmann-json/single_include/nlohmann/json.hpp
	$(CC) -c src/arm/main.cpp -o $@ $(RF24_INCL) $(JSON_INCL)

prepare:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

