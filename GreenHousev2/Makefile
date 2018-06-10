INCLUDE=-I/usr/local/lib -I. -I./lib/ghair -I./src/arm
LIBS=-L/usr/local/lib -lrf24
CFLAGS=
CXXFLAGS=
CC=g++ $(INCLUDE) $(LIBS) $(CFLAGS) $(CXXFLAGS)
BUILD_DIR=arm/build
TARGET=idea24th


#%.o: %.cpp
#	$(CC) -c -o $@ $< $(CFLAGS)

ghair.o: prepare
	$(CC) -c lib/ghair/ghair.cpp -o $(BUILD_DIR)/ghair.o

data.o: prepare
	$(CC) -c src/arm/data.cpp -o $(BUILD_DIR)/$@

b64encode.o:
	$(CC) -c src/arm/b64/encode.c -o $(BUILD_DIR)/$@
b64decode.o:
	$(CC) -c src/arm/b64/decode.c -o $(BUILD_DIR)/$@


$(TARGET): ghair.o data.o b64encode.o b64decode.o

prepare:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*

