// @author Stephen Cochrane
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

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

void *slave(void *args);
void getlock(int *lock);
void unlock(int *lock);
void broadcast(char *s, int CID);
void disconnect(int clientID);
int get_client(int *i); // Gets an available slave
pthread_t *get_thread();
void rel_thread(pthread_t *thread);
int init_user(struct user_data *ud, int socket);
