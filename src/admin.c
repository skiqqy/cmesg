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
			return 1;
		}
		buff[i++] = ch;
	}
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

void
print_admin(struct admin *ad)
{
	printf("User  -> %s\n", ad->user);
	printf("passw -> %s\n", ad->passw);
	printf("Misc -> %s\n", ad->misc);
}

int
init_admin(struct admin *ad)
{
	char key[64], val[64];
	if (!config_file) {
		// We assume no config file given -> doesnt enable an admin.
		return 1;
	}

	// Parse the config file
	while (parse(config_file, key, val)) {
		if (!strcmp("user", key)) {
			strcpy(ad->user, val);
		} else if (!strcmp("passw", key)) {
			strcpy(ad->passw, val);
		} else if (!strcmp("port", key)) {
			ad->port = atoi(val);
		} else if (!strcmp("Line", key)) {
			// Just for debugging.
			strcpy(ad->misc, val);
			print_admin(ad);
		} else {
			printf("ERROR: Anvalid config option");
		}
		
	}
	return 1; // Success.
}
