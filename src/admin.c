/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
