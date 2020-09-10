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
