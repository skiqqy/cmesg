/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include <stdlib.h>
#include <stdio.h>
#include "admin.c"

int
main(int argc, char *argv[])
{
	FILE *f = fopen(argv[1], "r");
	char buff[256];

	while (readln(f, buff)) {
		printf("%s\n", buff);
	}

	return EXIT_SUCCESS;
}
