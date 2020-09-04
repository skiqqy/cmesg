#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"

#define MESSAGE_OF_THE_DAY "Welcome!\n" // must have trailing newline
#define LOCKS 3

struct user_data *clients;
struct thread_pool *pool;
int users = 0;
int locks[LOCKS]; // Thread safety. 0 -> unlocked
int max_users = 2;

int
main(int argc, char *argv[])
{
	int server_fd, client, clientID, i;
	int opt = 1;
	int port = 8199;
	struct sockaddr_in address;
	struct slaveData sd;
	int addrlen = sizeof(address);
	char buff[256];

	while ((i = getopt(argc, argv, "p:M:h")) != -1) {
		switch (i) {
			case 'p':
				port = atoi(optarg);
				printf("Port: %d\n", port);
				break;
			case 'M':
				max_users = atoi(optarg);
				printf("max_users: %d\n", max_users);
				break;
			case 'h':
				printf("cmesg 1.2 (https://github.com/skippy404/cmesg)\n\n");
				printf("Usage: cmesg [options].\n");
				printf("-h\tShows this message.\n");
				printf("-p\tSpecify a port to use.\n");
				printf("-M\tSpecify the max user count.\n");
				return EXIT_SUCCESS;
				break;
		}
	}

	clients = malloc(sizeof(struct user_data)*max_users);

	// Init
	pool = malloc(sizeof(struct thread_pool));
	pool->count = 0;
	pool->thread = 0;
	pool->next = 0;

	for (i = 0; i < LOCKS; i++) {
		locks[i] = 0; // init the locks
	}

	for (i = 0; i < max_users; i++) {
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

	if (listen(server_fd, max_users) < 0) {
		perror("listen fail!");
		return EXIT_FAILURE;
	}

	while (1) {
		if (users < max_users) {
			*buff = '\0';
			client = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);

			if (!get_client(&clientID)) {
				sprintf(buff, "ERROR: No available client, please try again later.");
				send(client, buff, strlen(buff), 0);
				close(client);
				continue;
			}

			if (!init_user(&clients[clientID], client)) {
				printf("ERROR: Init of client failed.\n");
				close(clientID);
				continue;
			}

			sd.client = client;
			sd.clientID = clientID;

			getlock(&locks[0]);
			users++;
			unlock(&locks[0]);
			sd.thread = get_thread();
			pthread_create(sd.thread, NULL, slave, &sd);
			printf("Current connected users: %d/%d\n", users, max_users);
		}
	}
	return EXIT_SUCCESS;
}

int
init_user(struct user_data *ud, int socket)
{
	// TODO Populate ud
	char buff[256] = "";
	int val;
	ud->socket = socket;

	send(socket, "Enter Username: ", 16, 0);
	val = read(socket, buff, 32); // WARNING Socket closing will break server TODO: Make solid.
	if (!val) {
		return val; // an error occured.
	}
	strtok(buff, "\n");
	strcpy(ud->username, buff);

	send(socket, "Enter Age: ", 11, 0);
	val = read(socket, buff, 256); // WARNING Socket closing will break server TODO: Make solid.
	if (!val) {
		return val; // An error occured
	}
	strtok(buff, "\n");
	ud->age = atoi(buff);

	ud->used = 1;
	return 1;
}

pthread_t *
get_thread()
{
	pthread_t *thread;
	struct thread_pool *t = pool;

	if (pool->count) {
		printf("Thread space available from pool.\n");
		pool = pool->next;
		thread = t->thread;
		free(t);
	} else {
		printf("Pool empty, creating new thread.\n");
		thread = malloc(sizeof(pthread_t));
	}

	return thread;
}

void
rel_thread(pthread_t *thread)
{
	struct thread_pool *t = malloc(sizeof(struct thread_pool));
	t->count = pool->count;
	t->count++;
	t->thread = thread;
	t->next = pool;
	pool = t;
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
get_client(int *cl)
{
	int i;
	for (i = 0; i < max_users; i++) {
		if (!clients[i].used) {
			*cl = i;
			return 1;
		}
	}
	return 0;
}

void
broadcast(char *s, int CID)
{
	int i;
	char c[512];
	getlock(&locks[1]);
	for (i = 0; i < max_users; i++) {
		if (clients[i].used && (i !=CID)) {
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
	pthread_t *thread = sd.thread;
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
		broadcast(bigbuff, clientID); // We broadcast this.
		send(client, "Type: ", 6, 0);
	}

	disconnect(clientID);
	rel_thread(thread);
	return 0;
}
