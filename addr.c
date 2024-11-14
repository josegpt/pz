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

static struct	request req;
static struct	response res;

static void
pgaddr(struct request *req, struct response *res)
{
	res->type = "text/plain;charset=utf-8";
	bufwrite(&res->body, "%s", req->ip);
}

static void
pgusage(struct response *res)
{
	res->status = 404;
	res->type = "text/plain;charset=utf-8";
	bufwrite(&res->body, "usage: addr.pz.do[/]");
}

int
main(int argc, char **argv, char **envv)
{
	char *path;

	(void)argc;
	(void)argv;
	if (pledge("stdio", NULL) == -1)
		err(EXIT_FAILURE, "pledge");
	else if (cgiparse(&req, envv) == -1)
		errx(EXIT_FAILURE, "cgiparse");

	path = cgishift(&req);
	if (*path == '\0')
		pgaddr(&req, &res);
	else
		pgusage(&res);
	cgiserve(&res);
	return (EXIT_SUCCESS);
}
