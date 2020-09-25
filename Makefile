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
	$(COMP) $(FLAGS) src/client.c -o bin/client

test: init
	$(COMP) $(FLAGS) src/testadmin.c -o bin/testadmin
	./bin/testadmin tests/configs/test0.in
	./bin/testadmin tests/configs/test1.in
