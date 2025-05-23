/*
 * Copyright (c) 2025 Jose G Perez Taveras <josegpt27@gmail.com>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cgi.h"
#include "html.h"

static struct	request req;
static struct	response res;

static void
plainf(struct response *res, char *fmt, ...)
{
	va_list ap;

	res->type = "text/plain;charset=utf-8";
	va_start(ap, fmt);
	writev(&res->body, fmt, ap);
	va_end(ap);
}

static void
addrhtml(struct response *res, char *ip)
{
	struct buffer *b;

	b = &res->body;
	htmlhead(b, "addr", "public ip addr.");
	writen(b, "<main>");
	writen(b, "<h1>%s</h1>", ip);
	writen(b, "</main>");
}

static void
notallowed(struct request *req, struct response *res)
{
	char *path;
	int status;

	status      = 405;
	path        = shift(req);
	res->status = status;
	info("notallowed: path %s", path);
	if (accepts(req, "html"))
		htmlerr(&res->body, status);
	else
		plainf(res, "%d %s", status, sttstr(status));
}

static void
notfound(struct request *req, struct response *res)
{
	char *path;

	path = shift(req);
	info("notfound: path %s", path);
	switch (req->method) {
	case Get:
		res->status = 404;
		if (accepts(req, "html"))
			htmlerr(&res->body, res->status);
		else
			plainf(res, "usage: addr.pz.do[/]");
		break;
	default:
		notallowed(req, res);
	}
}

static void
addr(struct request *req, struct response *res)
{
	char *ip;

	ip = req->ip;
	info("ip %s", ip);
	switch (req->method) {
	case Get:
		if (accepts(req, "html"))
			addrhtml(res, ip);
		else
			plainf(res, "%s", ip);
		break;
	default:
		notallowed(req, res);
	}
}

int
main(int argc, char **argv, char **envv)
{
	char *path;

	(void)argc;
	(void)argv;
	if (pledge("stdio", NULL) == -1)
		fatal("pledge");

	parse(envv, &req);
	path = shift(&req);
	if (*path == '\0')
		addr(&req, &res);
	else
		notfound(&req, &res);

	render(&res);
	return (EXIT_SUCCESS);
}
