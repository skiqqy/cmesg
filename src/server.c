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
void getlock(int *lock);
void unlock(int *lock);
int getslave(); // Gets an available slave
void broadcast(char *s);
void disconnect(int clientID);

int
main(int argc, char *argv[])
{
	int server_fd, client, clientID, i;
	int opt = 1;
	int port = (argc > 1) ? atoi(argv[1]) : 8199;
	struct sockaddr_in address;
	struct slaveData sd;
	int addrlen = sizeof(address);
	char buff[256];

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

			// Get User Data
			send(client, MESSAGE_OF_THE_DAY, strlen(MESSAGE_OF_THE_DAY), 0);
			send(client, "Enter Username: ", strlen("Enter Username: "), 0);
			read(client, buff, 256); // WARNING Socket closing will break server
			strtok(buff, "\n"); // Remove newline
			sprintf(clients[clientID].username, buff); // WARNING Secuirity risk

			clients[clientID].age = 0; // TODO get age
			clients[clientID].used = 1;
			clients[clientID].socket = client;
			sd.client = client;
			sd.clientID = clientID;

			getlock(&locks[0]);
			users++;
			pthread_create(&sockets[clientID], NULL, slave, &sd);
			unlock(&locks[0]);
			printf("Current connected users: %d/%d\n", users, MAX_USERS);
		}
	}
	return EXIT_SUCCESS;
}

void
getlock(int *lock)
{
	while (*lock);
	*lock = 1;
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

void
broadcast(char *s)
{
	int i;
	char c[512];
	getlock(&locks[1]);
	for (i = 0; i < MAX_USERS; i++) {
		if (clients[i].used) {
			// We can send to this client
			printf("Broadcasting \"%s\" -> %d\n", s, clients[i].socket);
			sprintf(c, "\n%s\nType: ", s); // This is for nc support, the GUI client will remove this
			send(clients[i].socket, c, strlen(c), 0);
		}
	}
	unlock(&locks[1]);
}

void
disconnect(int clientID)
{
	close(clients[clientID].socket);
	// Begin lock for thread safety
	getlock(&locks[0]);
	clients[clientID].used = 0;
	users--;
	unlock(&locks[0]);
}

void * 
slave(void *args)
{
	struct slaveData sd = *((struct slaveData *) args);
	int client = sd.client;
	int clientID = sd.clientID;
	char s[64];
	char buff[256];
	char bigbuff[512];
	sprintf(s, "(%s) Type: ", clients[clientID].username);

	printf("Slave, fd=%d, clientID=%d\n", client, clientID);
	printf("Username = %s\n", clients[clientID].username);
	send(client, s, strlen(s), 0);
	while (read(client, buff, 256)) {
		strtok(buff, "\n"); // remove trailing newline:
		if (!strcmp("/q", buff)) {
			disconnect(clientID);
			return 0;
		}
		printf("Mesg from client: %s\n", buff);
		sprintf(bigbuff, "(%s) ~ %s", clients[clientID].username, buff);
		broadcast(bigbuff); // We broadcast this.
	}

	disconnect(clientID);
	return 0;
}
