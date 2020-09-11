/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <stdlib.h>
#include <stdio.h>

#include "server.h"

int
main(int argc, char *argv[])
{
	int server_fd, client, clientID, i;
	int opt = 1;
	int port = 8199;
	struct sockaddr_in address, admin_address;
	struct slaveData sd;
	struct admin *ad = malloc(sizeof(struct admin));
	int addrlen = sizeof(address);
	char buff[256];

	while ((i = getopt(argc, argv, "p:M:c:h")) != -1) {
		switch (i) {
			case 'p':
				port = atoi(optarg);
				printf("Port: %d\n", port);
				break;
			case 'M':
				max_users = atoi(optarg);
				printf("max_users: %d\n", max_users);
				break;
			case 'c':
				config_file = fopen(optarg, "r");
				break;
			case 'h':
				printf("cmesg v1.3 (https://github.com/skippy404/cmesg)\n\n");
				printf("Usage: cmesg [options].\n");
				printf("-h\tShows this message.\n");
				printf("-p\tSpecify a port to use.\n");
				printf("-M\tSpecify the max user count.\n");
				printf("-c\tSpecify the path to an admin config file.\n");
				return EXIT_SUCCESS;
				break;
		}
	}

	clients = malloc(sizeof(struct user_data)*max_users);
	ad->port = 0;

	// Init
	pool = malloc(sizeof(struct thread_pool));
	pool->count = 0;
	pool->thread = 0;
	pool->next = 0;

	signal(SIGPIPE, SIG_IGN);

	for (i = 0; i < LOCKS; i++) {
		locks[i] = 0; // init the locks
	}

	for (i = 0; i < max_users; i++) {
		clients[i].used = 0; // init the locks
	}

	if (!open_socket(&server_fd, &address, opt, max_users, port)) {
		return EXIT_FAILURE;
	}
	printf("Server socket opened, port = %d\nserver_fd = %d\n", port, server_fd);

	// Spawn admin thread if needed
	if (config_file && init_admin(ad)) {
		if (!ad->port || ad->port == port || !open_socket(&ad->fd, &admin_address, opt, max_users, ad->port)) {
			printf("ERROR: Admin cannot open socket\n");
		} else {
			// TODO: init and spawn the thread.
			ad->address = &admin_address;
			pthread_create(malloc(sizeof(pthread_t)), NULL, admin_slave, ad);
		}
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
open_socket(int *sfd, struct sockaddr_in *address, int opt, int max, int port)
{
	if (!(*sfd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("socket failed!");
	}

	if (setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("setsockopt"); 
		return 0;
	}

	address->sin_family = AF_INET;
	address->sin_addr.s_addr = INADDR_ANY;
	address->sin_port = htons(port);

	if (bind(*sfd, (struct sockaddr *) address, sizeof(*address)) < 0) {
		perror("bind fail!");
		return 0;
	}

	if (listen(*sfd, max_users) < 0) {
		perror("listen fail!");
		return 0;
	}

	// Success
	return 1;
}

int
init_user(struct user_data *ud, int socket)
{
	// TODO Populate ud
	char buff[256] = "";
	int val;
	ud->socket = socket;

	send(socket, "Enter Username: ", 16, 0);
	val = read(socket, buff, 32);
	if (!val) {
		return val; // an error occured.
	}
	
	strtok(buff, "\n");
	if (!uniq_user(buff)) {
		send(socket, "ERROR: Username taken.\n", 23, 0);
		return 0;
	}
	strcpy(ud->username, buff);

	send(socket, "Enter Age: ", 11, 0);
	val = read(socket, buff, 256);
	if (!val) {
		return val; // An error occured
	}
	strtok(buff, "\n");
	ud->age = atoi(buff);

	ud->used = 1;
	return 1;
}

int
uniq_user(char *c)
{
	int i;
	for (i = 0; i < max_users; i++) {
		if (clients[i].used && !strcmp(clients[i].username, c)) {
			return 0;
		}
	}
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
			sprintf(c, "\n%s\nType: ", s); // This is for nc support, the GUI client will remove this
			printf("Broadcasting \"%s\" -> %d\n", s, clients[i].socket);
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
