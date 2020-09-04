# cmesg Makefile
COMP=gcc
FLAGS=-Wall -pedantic -lpthread

all: init server

init:
	mkdir -p bin/

clean:
	rm -rf bin

server:
	$(COMP) $(FLAGS) src/server.c -o bin/cmesg
