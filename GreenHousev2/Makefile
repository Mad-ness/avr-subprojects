INCLUDE=-I/usr/local/lib -I. -I./lib/ghair -I./src/arm -I./src/arm/include -I./src/arm/urlparser/src -I./src/arm/simple-url-parser
# OS=LINUX
# SOC=A20
RF24_INCL=-I/var/tmp/librf24-sunxi/src
RF24_LIBS=-L/usr/local/lib -lrf24-sunxi
JSON_INCL=-I./src/arm/nlohmann-json/include
LIBS=-L/usr/local/lib -L/usr/lib -levent -ljansson
CFLAGS=
# NDEBUG is for the JSON library
CXXFLAGS=-DDEBUG -D__LINUX__ -DCUBIEBOARD2 -DGPIO_SUN7I 
#-DNDEBUG
# -flto
CC=g++ -Os -std=c++11 $(INCLUDE) $(CFLAGS) $(CXXFLAGS)
BUILD_DIR=./arm/build
TARGET=idea24
TESTS_DIR=src/arm/tests

EXAMPLES=router_check pmtest rexp routemanager_test

target: $(TARGET)
all: $(TARGET) $(EXAMPLES)
examples: $(EXAMPLES)

#
# Run modules
#

pmtest: $(BUILD_DIR)/packetmanager.o $(BUILD_DIR)/pmtest.o
	$(CC) $(BUILD_DIR)/pmtest.o $(BUILD_DIR)/packetmanager.o -o $@ $(RF24_INCL)

router_check: $(BUILD_DIR)/router_check.o $(BUILD_DIR)/surlparser.o $(BUILD_DIR)/routemanager.o
	$(CC) $(BUILD_DIR)/routemanager.o $(BUILD_DIR)/router_check.o $(BUILD_DIR)/surlparser.o -o $@

rexp: $(BUILD_DIR)/routemanager.o $(BUILD_DIR)/rexp.o
	$(CC) $(BUILD_DIR)/routemanager.o $(BUILD_DIR)/rexp.o -o $@

routemanager_test: $(BUILD_DIR)/surlparser.o $(BUILD_DIR)/routemanager.o $(BUILD_DIR)/routemanager_test.o
	$(CC) $(BUILD_DIR)/routemanager_test.o $(BUILD_DIR)/routemanager.o $(BUILD_DIR)/surlparser.o -o $@


#
# Object modules
#

$(BUILD_DIR)/routemanager.o: src/arm/routemanager.cpp src/arm/routemanager.h
	$(CC) -c src/arm/routemanager.cpp -o $@

$(BUILD_DIR)/routemanager_test.o: src/arm/tests/routemanager_test.cpp src/arm/routemanager.h
	$(CC) -c src/arm/tests/routemanager_test.cpp -o $@

$(BUILD_DIR)/rexp.o: src/arm/routemanager.h src/arm/tests/rexp.cpp
	$(CC) -c src/arm/tests/rexp.cpp -o $@

$(BUILD_DIR)/surlparser.o: src/arm/simple-url-parser/surlparser.cpp src/arm/simple-url-parser/surlparser.h
	$(CC) -c src/arm/simple-url-parser/surlparser.cpp -o $@

$(BUILD_DIR)/router_check.o: src/arm/routemanager.h  src/arm/tests/router_check.cpp
	$(CC) -c src/arm/tests/router_check.cpp -o $@

$(BUILD_DIR)/pmtest.o: $(TESTS_DIR)/pmtest.cpp $(BUILD_DIR)/packetmanager.o
	$(CC) -c $(TESTS_DIR)/pmtest.cpp -o $@ $(RF24_INCL)

$(TARGET):  $(BUILD_DIR)/ghair.o \
		    $(BUILD_DIR)/data.o \
            $(BUILD_DIR)/packetmanager.o \
		    $(BUILD_DIR)/b64encode.o \
		    $(BUILD_DIR)/b64decode.o \
            $(BUILD_DIR)/routemanager.o \
            $(BUILD_DIR)/surlparser.o \
            $(BUILD_DIR)/proxy-api.o \
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

$(BUILD_DIR)/edurlparser.o: src/arm/urlparser/src/EdUrlParser.cpp src/arm/urlparser/src/EdUrlParser.h
	$(CC) -c src/arm/urlparser/src/EdUrlParser.cpp -o $@


$(BUILD_DIR)/main.o: src/arm/main.cpp lib/ghair/ghairdefs.h src/arm/data.h ./src/arm/nlohmann-json/single_include/nlohmann/json.hpp
	$(CC) -c src/arm/main.cpp -o $@ $(RF24_INCL) $(JSON_INCL)

$(BUILD_DIR)/proxy-api.o: src/arm/proxy-api.cpp src/arm/proxy-api.h
	$(CC) -c src/arm/proxy-api.cpp -o $@

$(BUILD_DIR)/device-api.o: src/arm/device-api.cpp src/arm/device-api.h lib/ghair/ghairdefs.h lib/ghair/ghair.h
	$(CC) -c src/arm/device-api.cpp -o $@ $(RF24_INCL)

prepare:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET) $(EXAMPLES)

