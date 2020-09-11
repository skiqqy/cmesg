/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "admin.c"

struct slaveData {
	int client;
	int clientID;
	pthread_t *thread;
};

struct user_data {
	char username[32];
	int age;
	int used;
	int socket;
} User;

typedef struct thread_pool ThreadPool;
struct thread_pool {
	int count;
	pthread_t *thread;
	ThreadPool *next;
};

#define MESSAGE_OF_THE_DAY "Welcome!\n" // must have trailing newline
#define LOCKS 3

struct user_data *clients = NULL;
struct thread_pool *pool = NULL;
int users = 0;
int locks[LOCKS]; // Thread safety. 0 -> unlocked
int max_users = 2;

void *slave(void *args);
void getlock(int *lock);
void unlock(int *lock);
void broadcast(char *s, int CID);
void disconnect(int clientID);
int get_client(int *i); // Gets an available slave
pthread_t *get_thread();
void rel_thread(pthread_t *thread);
int init_user(struct user_data *ud, int socket);
int uniq_user(char *c);
int open_socket(int *sfd, struct sockaddr_in *address, int opt, int max, int port);
