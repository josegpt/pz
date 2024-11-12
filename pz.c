#include <stdlib.h>
#include <string.h>

#include "pz.h"

static unsigned long
hash(char *k)
{
	unsigned long h;

	h = 5381;
	while (*k++)
		h = ((h<<5) + h) + *k;
	return (h%NHASH);
}

static struct map*
search(struct map **m, char *k)
{
	struct map *c;
	unsigned long h;

	h = hash(k);
	for (c = m[h]; c && strcmp(k, c->k); c = c->n);
	return (c);
}

void
mapset(struct map **m, char *k, void *v)
{
	struct map *c;
	unsigned long h;

	if ((c = search(m, k)))
		c->v = v;
	else {
		h = hash(k);
		c = malloc(sizeof(*c));
		c->k = k;
		c->v = v;
		c->n = m[h];
		m[h] = c;
	}
}

void*
mapget(struct map **m, char *k)
{
	struct map *c;

	c = search(m, k);
	return (c ? c->v : NULL);
}

int
maphas(struct map **m, char *k)
{
	struct map *c;

	c = search(m, k);
	return (c ? 1 : 0);
}

void
mapdel(struct map **m, char *k)
{
	struct map *c, *p;
	unsigned long h;

	h = hash(k);
	for (c = m[h], p = NULL; c; p = c, c = c->n)
		if (strcmp(k, c->k) == 0) {
			if (p)
				p->n = c->n;
			else
				m[h] = c->n;
		}
}
