/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>

struct admin {
	int port;
	int fd;
	struct sockaddr_in *address;
	char user[256];
	char passw[256];
	char misc[256];
};

// Admin error codes.
#define NONE   0
#define BOUNDS 1
#define FLAG   2
#define RANGE  3

// Admin commands.
#define HELP   "help"
#define LS     "ls"
#define MUTE   "mute"
#define UNMUTE "unmute"
#define KICK   "kick"

int admin_socket;
void command(char *c);
void admin_error(char *command, int code);
int check_flag(char *command, char *flag);
