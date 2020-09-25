# cmesg Makefile
COMP=gcc
FLAGS=-Wall -pedantic -lpthread

all: server client

init:
	mkdir -p bin/
	-@[ ! -f config ] && cp config.example config || echo "Existing config found. Skipping."

clean:
	rm -rf bin

server: init
	$(COMP) $(FLAGS) src/server.c -o bin/cmesg

client: init
	$(COMP) `pkg-config --cflags gtk+-3.0` -o bin/client src/client.c `pkg-config --libs gtk+-3.0`

test: init
	$(COMP) $(FLAGS) src/testadmin.c -o bin/testadmin
	./bin/testadmin tests/configs/test0.in
	./bin/testadmin tests/configs/test1.in
