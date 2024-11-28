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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buf.h"
#include "map.h"
#include "cgi.h"
#include "html.h"
#include "log.h"

static struct	request req;
static struct	response res;
static char	*type = "text/plain;charset=utf-8";

static void
ntallowed(struct request *req, struct response *res)
{
	res->status = 405;
	if (cgiaccepts(req, "html"))
		htmlerr(res);
	else {
		res->type = type;
		bufwrite(&res->body, "%d %s", res->status, ctos(res->status));
	}
}

static void
addr(struct request *req, struct response *res)
{
	switch (req->method) {
	case CgiGet:
		if (cgiaccepts(req, "html")) {
			htmlhead(res, "My Public IP Address", "Find out your public IP address.");
			html(res, "<main>");
			html(res, "<hgroup>");
			html(res, "<h1>%s</h1>", req->ip);
			html(res, "<p>My public IP address.");
			html(res, "</hgroup>");
			html(res, "</main>");
		} else {
			res->type = type;
			bufwrite(&res->body, "%s", req->ip);
		}
		break;
	default:
		ntallowed(req, res);
	}
}

static void
usage(struct request *req, struct response *res)
{
	switch (req->method) {
	case CgiGet:
		res->status = 404;
		if (cgiaccepts(req, "html"))
			htmlerr(res);
		else {
			res->type = type;
			bufwrite(&res->body, "usage: addr.pz.do[/]");
		}
		break;
	default:
		ntallowed(req, res);
	}
}

int
main(int argc, char **argv, char **envv)
{
	char *path;

	(void)argc;
	(void)argv;
	if (pledge("stdio", NULL) == -1)
		logfatal(EXIT_FAILURE, "pledge");
	if (cgiparse(&req, envv) == -1)
		logfatal(EXIT_FAILURE, "cgiparse");
	path = cgishift(&req);
	if (*path == '\0')
		addr(&req, &res);
	else
		usage(&req, &res);
	cgiserve(&res);
	return (EXIT_SUCCESS);
}
