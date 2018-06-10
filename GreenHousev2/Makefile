INCLUDE=-I/usr/local/lib -I. -I./lib/ghair -I./src/arm -I./src/arm/include
LIBS=-L/usr/local/lib -lrf24 -L/usr/lib -levent -ljansson
CFLAGS=
CXXFLAGS=
CC=g++ -std=c++11 $(INCLUDE) $(CFLAGS) $(CXXFLAGS)
BUILD_DIR=./arm/build
TARGET=idea24

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/ghair.o $(BUILD_DIR)/data.o $(BUILD_DIR)/b64encode.o $(BUILD_DIR)/b64decode.o $(BUILD_DIR)/main.o
	$(CC)         $^ -o $@ $(CFLAGS) $(LIBS)

$(BUILD_DIR)/ghair.o: lib/ghair/ghair.cpp lib/ghair/ghairdefs.h
	$(CC) -c lib/ghair/ghair.cpp -o $@

$(BUILD_DIR)/data.o: src/arm/data.cpp src/arm/data.h
	$(CC) -c src/arm/data.cpp -o $@

$(BUILD_DIR)/b64encode.o: src/arm/b64.h src/arm/encode.c
	$(CC) -c src/arm/encode.c -o $@
$(BUILD_DIR)/b64decode.o: src/arm/b64.h src/arm/decode.c
	$(CC) -c src/arm/decode.c -o $@

$(BUILD_DIR)/main.o: src/arm/main.cpp lib/ghair/ghairdefs.h src/arm/data.h
	$(CC) -c src/arm/main.cpp -o $@

prepare:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*.o

