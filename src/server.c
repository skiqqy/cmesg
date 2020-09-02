#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

#include "server.h"

#define MAX_USERS 2
#define MESSAGE_OF_THE_DAY "Welcome!\n" // must have trailing newline
#define LOCKS 2

struct user_data clients[MAX_USERS];
int users = 0;
int locks[LOCKS]; // Thread safety. 0 -> unlocked
pthread_t sockets[MAX_USERS];

void *slave(void *args);
int getlock(int *lock);
void unlock(int *lock);
int getslave(); // Gets an available slave

int
main(int argc, char *argv[])
{
	int server_fd, client, clientID, i;
	int opt = 1;
	int port = (argc > 1) ? atoi(argv[1]) : 8199;
	struct sockaddr_in address;
	struct slaveData sd;
	int addrlen = sizeof(address);

	for (i = 0; i < LOCKS; i++) {
		locks[i] = 0; // init the locks
	}

	for (i = 0; i < MAX_USERS; i++) {
		clients[i].used = 0; // init the locks
	}

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

	if (listen(server_fd, MAX_USERS) < 0) {
		perror("listen fail!");
		return EXIT_FAILURE;
	}

	while (1) {
		if (users < MAX_USERS) {
			client = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
			clientID = getslave();

			if (clientID == -1) {
				close(client); // We dont have a thread...
				continue;
			}

			clients[clientID].age = 0;
			sprintf(clients[clientID].username, "TempName%d", clientID);
			clients[clientID].used = 1;
			sd.client = client;
			sd.clientID = clientID;
			while (getlock(&locks[0]));
			users++;
			pthread_create(&sockets[clientID], NULL, slave, &sd);
			unlock(&locks[0]);
			printf("Current connected users: %d/%d\n", users, MAX_USERS);
		}
	}
	return EXIT_SUCCESS;
}

int
getlock(int *lock)
{
	if (!*lock) {
		*lock = 1;
		return 0;
	}
	return 1;
}

void
unlock(int *lock)
{
	*lock = 0;
}

int
getslave()
{
	int i;
	for (i = 0; i < MAX_USERS; i++) {
		if (!clients[i].used) {
			return i;
		}
	}
	return -1;
}

void * 
slave(void *args)
{
	struct slaveData sd = *((struct slaveData *) args);
	int client = sd.client;
	int clientID = sd.clientID;
	char *s = "Type: ";
	char buff[256];

	printf("Slave, fd=%d, clientID=%d\n", client, clientID);
	printf("Username = %s\n", clients[clientID].username);
	send(client, MESSAGE_OF_THE_DAY, strlen(MESSAGE_OF_THE_DAY), 0);
	send(client, s, strlen(s), 0);
	while (read(client, buff, 256)) {
		strtok(buff, "\n"); // remove trailing newline:
		printf("Mesg from client: %s\n", buff);
		strcat(buff, " <- message sent\n");
		send(client, buff, strlen(buff), 0);
		send(client, s, strlen(s), 0);
	}

	close(client);
	while (getlock(&locks[0]));
	users--;
	clients[clientID].used = 0;
	unlock(&locks[0]);
	return 0;
}
