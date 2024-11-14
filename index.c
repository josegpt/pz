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

#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pz.h"
#include "html.h"

static struct	response res;

static void
pgcs(struct response *res)
{
	char *s;

	s = "Coming soon";
	res->type = "text/html;charset=utf-8";
	htmlheader(&res->body, s);
	html(&res->body, "<main>");
	html(&res->body, "<h2>%s</h2>", s);
	html(&res->body, "</main>");
}

int
main(void)
{
	if (pledge("stdio", NULL) == -1)
		err(EXIT_FAILURE, "pledge");

	pgcs(&res);
	cgiserve(&res);
	return (EXIT_SUCCESS);
}
