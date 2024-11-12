#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pz.h"

int
main(void)
{
	struct map *m[NHASH];

	memset(m, 0, NHASH*sizeof(*m));
	mapset(m, "fname", "jose");
	printf("has: %d\n", maphas(m, "fname"));
	printf("fname: %s\n", (char*)mapget(m, "fname"));
	return (EXIT_SUCCESS);
}
