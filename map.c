/*
 * Copyright (c) 2024 Jose G Perez Taveras <josegpt27@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

static unsigned long
hash(char *key)
{
	unsigned long h;

	h = 5381;
	while (*key++)
		h = ((h<<5) + h) + *key;
	return (h%NHASH);
}

static struct map *
lookup(struct map **m, char *key)
{
	struct map *c;
	unsigned long h;

	h = hash(key);
	for (c = m[h]; c && strcmp(key, c->key); c = c->next);
	return (c);
}

static void
append(struct map *m, char *val)
{
	assert((m->len + 1) < VLEN);
	m->val[m->len++] = val;
}

static struct map *
link(struct map **m, char *key)
{
	struct map *c;
	unsigned long h;
	int n;

	h = hash(key);
	n = sizeof(*c);
	c = malloc(n);
	memset(c, 0, n);
	c->key = key;
	c->next = m[h];
	m[h] = c;
	return (c);
}

void
mapadd(struct map **m, char *key, char *val)
{
	struct map *c;

	if ((c = lookup(m, key)) == NULL)
		c = link(m, key);
	append(c, val);
}

void
mapset(struct map **m, char *key, char *val)
{
	struct map *c;

	if ((c = lookup(m, key)))
		c->len = 0;
	else
		c = link(m, key);
	append(c, val);
}

char *
mapget(struct map **m, char *key)
{
	struct map *c;

	c = lookup(m, key);
	return (c ? *c->val : NULL);
}

char **
mapgetv(struct map **m, char *key)
{
	struct map *c;

	c = lookup(m, key);
	return (c ? c->val : NULL);
}

int
maphas(struct map **m, char *key)
{
	struct map *c;

	c = lookup(m, key);
	return (c ? 1 : 0);
}

void
mapdel(struct map **m, char *key)
{
	struct map *c, *p;
	unsigned long h;

	h = hash(key);
	for (c = m[h], p = NULL; c; p = c, c = c->next)
		if (strcmp(key, c->key) == 0) {
			if (p)
				p->next = c->next;
			else
				m[h] = c->next;
		}
}
