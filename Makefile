
CC=g++
CFLAGS= -O3 -g -std=c++20 -fomit-frame-pointer -Iapultra/src/libdivsufsort/include -Iapultra/src
LDFLAGS=
MKDIR=$(shell which mkdir)

APP := test

MSRC += test.cpp

all: $(APP)

$(APP): obj/apultra.o test.cpp compression.cpp rom.cpp json.cpp freespace.cpp
	$(CC) $(wildcard obj/*.o) $(MSRC) $(CFLAGS) $(LDFLAGS) -o $(APP)

obj/apultra.o: apultra
	$(MKDIR) -p obj
	$(CC) -c $(wildcard apultra/src/*.c) $(CFLAGS)
	$(CC) -c $(wildcard apultra/src/libdivsufsort/lib/*.c) $(CFLAGS)
	strip --strip-symbol main apultra.o
	mv *.o obj

clean:
	@rm -rf $(APP) $(APP).exe obj
