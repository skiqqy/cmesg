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

/* Standerd Libraries*/
#include <stdlib.h>
#include <stdio.h>

void soc_read(char *buff);
void on_send_clicked(GtkButton *b);
