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
	char key[64], val[64];
	struct admin ad;

	// Init
	sprintf(ad.passw, "Not-Set");
	sprintf(ad.user, "Not-Set");
	sprintf(ad.misc, "Not-Set");
	ad.port = 0;

	printf("Testing readln\n");
	while (readln(f, key)) {
		printf("%s\n", key);
	}

	f = fopen(argv[1], "r");
	printf("\nTesting parse\n");
	while (parse(f, key, val)) {
		printf("key=%s, val=%s\n", key, val);
	}

	printf("\nTesting init_admin\n");
	config_file = fopen(argv[1], "r");
	if (init_admin(&ad)) {
		print_admin(&ad);
	}

	return EXIT_SUCCESS;
}
