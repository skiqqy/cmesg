/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
	printf("Admin slave started\nuser = %s\nSocket FD = %d\n", ad->user, ad->fd);
	return 0;
}
