/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <unistd.h> 
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <netdb.h>

/* Standerd Libraries*/
#include <stdlib.h>
#include <stdio.h>

struct widge_mapping {
	char *ID; // The ID
	GtkWidget **widget; // The Widget
};

int sock; /* The socket FD */

void send_mesg(char *mesg);
int recv_mesg(char *buff, int *type);
void on_send_clicked(GtkButton *b);
int init_sock(int port, char *host, int *sock, struct sockaddr_in *address);
