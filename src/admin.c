/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "server.h"
#include "admin.h"

FILE *config_file = NULL;

/* Simple read function, reads a single line from a file.
 *
 * @param FILE *f: The stream to read from
 * @param char *buff: The buffer to store the line in, ensure that there is enough space.
 * 
 * @return int: 1 if there is still more lines to read from, 0 if there are no more new lines.
 */
int
readln(FILE *f, char *buff)
{
	int ch, i = 0;
	while ((ch = fgetc(f)) != EOF) {
		if (ch == '\n') {
			buff[i] = 0;
			return 1;
		}
		buff[i++] = ch;
	}
	buff[i] = 0;
	return 0;
}

/* Simple parse function, gets data from a line from config file.
 * 
 * @param FILE *f: The stream to read from.
 * @param char *key: A buffer to store the key in.
 * @param char *val: A buffer to store the value in.
 *
 * @return int: 1 if there is more to parse, 0 if there is no more to parse.
 */
int
parse(FILE *f, char *key, char *val)
{
	char buff[256];
	int ret;
	ret = readln(f, buff);
	sscanf(buff, "%s", key);
	sscanf(&buff[strlen(key)], "%s", val);
	return ret;
}

/* Simple toString function for an admin struct
 *
 * @param struct admin *ad: The struct to print
 */
void
print_admin(struct admin *ad)
{
	printf("User  -> %s\n", ad->user);
	printf("passw -> %s\n", ad->passw);
	printf("port  -> %d\n", ad->port);
	printf("Misc  -> %s\n", ad->misc);
}

/* Init an admin struct from config_file.
 * 
 * @param struct admin *ad: The strut to init
 * 
 * @return int: 1 -> Success init, 0 -> Something went wrong, ad->misc will contain details.
 */
int
init_admin(struct admin *ad)
{
	char buff[256], key[64], val[64];
	if (!config_file) {
		// We assume no config file given -> doesnt enable an admin.
		sprintf(ad->misc, "ERROR: config_file not set.");
		return 0;
	}

	// Parse the config file
	while (parse(config_file, key, val)) {
		if (!strcmp("user", key)) {
			strcpy(ad->user, val);
		} else if (!strcmp("passw", key)) {
			strcpy(ad->passw, val);
		} else if (!strcmp("port", key)) {
			ad->port = atoi(val);
		} else {
			sprintf(buff, "ERROR: Illegal config -> %s:%s", key, val);
			strcpy(ad->misc, buff);
			print_admin(ad);
			return 0;
		}
		
	}
	return 1; // Success.
}

void *
admin_slave(void *in)
{
	struct admin *ad = ((struct admin *) in);
	int code;
	char buff[256];
	socklen_t size = sizeof(ad->address);

	printf("Admin slave started\nuser = %s\nSocket FD = %d\n", ad->user, ad->fd);

	while (1) {
		admin_socket = accept(ad->fd, (struct sockaddr *) ad->address, (socklen_t *) &size);

		send(admin_socket, "Login: ", 7, 0);
		code = read(admin_socket, buff, 256);
		
		if (!code) {
			continue;
		}

		strtok(buff, "\n");
		if (strcmp(ad->user, buff)) {
			printf("ADMIN ERROR: Invalid username.\n");
			send(admin_socket, "ERROR: Incorrect username.", 26, 0);
			close(admin_socket);
			continue;
		}

		send(admin_socket, "Password: ", 10, 0);
		code = read(admin_socket, buff, 256);

		if (!code) {
			continue;
		}

		strtok(buff, "\n");
		if (strcmp(ad->passw, buff)) {
			printf("ADMIN ERROR: Invalid password.\n");
			send(admin_socket, "ERROR: Incorrect password.", 26, 0);
			close(admin_socket);
			continue;
		}
		
		send(admin_socket, "Enter Command: ", 15, 0);
		while (read(admin_socket, buff, 256)) {
			strtok(buff, "\n");
			// TODO: Parse command.
			command(buff);
			send(admin_socket, "Enter Command: ", 15, 0);
		}
	}
	close(admin_socket);
	return 0;
}

void
command(char *c)
{
	// TODO Run a command.
	char buff[256];
	char *command = 0, *flag = 0;
	int i;
	const char help[512] = "\nWelcome to the help menu!\n\n"
							"Available commands:\n"
							"'help' -> Displays this message.\n"
							"'ls'   -> Lists connected users, as well as thier client ID\n"
							"\n";

	command = strtok(c, " ");
	flag = strtok(NULL, " ");

	if (!command) {
		printf("ADMIN ERROR: command is NULL.\n");
		return; // Not legal token.
	}

	// For commands that use a flag.
	if (flag) {
		printf("FLAG = %s\n", flag);
	}

	if (!strcmp(c, "ls")) {
		// List all the users.
		printf("ADMIN: 'ls' command.\n");
		send(admin_socket, "\n", 1, 0);
		for (i = 0; i < max_users; i++) {
			if (clients[i].used) {
				sprintf(buff, "%s\n", clients[i].username);
				send(admin_socket, buff, strlen(buff), 0);
			}
		}
		send(admin_socket, "\n", 1, 0);
	} else if (!strcmp(c, "help")) {
		printf("ADMIN: '\\h' command.\n");
		send(admin_socket, help, strlen(help), 0);
	} else if (!strcmp(c, "mute")) {
		if (!flag) {
			sprintf(buff, "ADMIN ERROR 'mute' requires an argument, e.g. -> mute <clientID>\n");
			printf("%s", buff);
			send(admin_socket, buff, strlen(buff), 0);
		}
	} else {
		sprintf(buff, "ADMIN ERROR: Invalid command!\n");
		send(admin_socket, buff, strlen(buff), 0);
	}
}
