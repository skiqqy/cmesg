#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 

int
main(int argc, char *argv[])
{
	int server_fd, new_socket;
	int opt = 1;
	int port = (argc > 1) ? atoi(argv[0]) : 8200;
	printf("%d\n", port);
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buff[256] = {0};
	char *hello = "Hello from serv\n";

	if (!(server_fd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("socket failed!");
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("setsockopt"); 
		return EXIT_FAILURE;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind fail!");
		return EXIT_FAILURE;
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen fail!");
		return EXIT_FAILURE;
	}

	if ((new_socket = accept(server_fd, (struct sockaddr * ) &address, (socklen_t *) &addrlen)) < 0) {
		perror("accept error!");
		return EXIT_FAILURE;
	}

	read(new_socket, buff, 256);
	printf("%s\n", buff);
	send(new_socket, hello, strlen(hello), 0);
	return EXIT_SUCCESS;
}
