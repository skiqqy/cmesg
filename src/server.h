// @author Stephen Cochrane
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

void *slave(void *args);
void getlock(int *lock);
void unlock(int *lock);
int getclient(); // Gets an available slave
void broadcast(char *s);
void disconnect(int clientID);
pthread_t *getthread();
void relthread(pthread_t *thread);

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
