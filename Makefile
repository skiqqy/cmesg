# cmesg Makefile
COMP=gcc
FLAGS=-Wall -pedantic -lpthread

all: server

init:
	mkdir -p bin/

clean:
	rm -rf bin

server: init
	$(COMP) $(FLAGS) src/server.c -o bin/cmesg

test: init
	$(COMP) $(FLAGS) src/testadmin.c -o bin/testadmin
	./bin/testadmin tests/configs/test0.in
