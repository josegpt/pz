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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.h"
#include "map.h"
#include "cgi.h"
#include "str.h"

static int	redirects[] = {301, 302, 303, 307, 308, 0};

static struct method {
	char*	from;
	int	to;
} methodtab[CgiMax] = {
	{"CONNECT",	CgiConnect},
	{"DELETE",	CgiDelete},
	{"GET",		CgiGet},
	{"HEAD",	CgiHead},
	{"OPTIONS",	CgiOptions},
	{"PATCH",	CgiPatch},
	{"POST",	CgiPost},
	{"PUT",		CgiPut},
	{"TRACE",	CgiTrace}
};

static char *
ctos(int code)
{
	char *s;

	switch (code) {
	case 100: s = "continue"; break;
	case 101: s = "switching protocols"; break;
	case 102: s = "processing"; break;
	case 200: s = "ok"; break;
	case 201: s = "created"; break;
	case 202: s = "accepted"; break;
	case 203: s = "non-authoritative information"; break;
	case 204: s = "no content"; break;
	case 205: s = "reset content"; break;
	case 206: s = "partial content"; break;
	case 207: s = "multi-status"; break;
	case 208: s = "already reported"; break;
	case 226: s = "im used"; break;
	case 300: s = "multiple choices"; break;
	case 301: s = "moved permanently"; break;
	case 302: s = "found"; break;
	case 303: s = "see other"; break;
	case 304: s = "not modified"; break;
	case 305: s = "use proxy"; break;
	case 307: s = "temporary redirect"; break;
	case 308: s = "permanent redirect"; break;
	case 400: s = "bad request"; break;
	case 401: s = "unauthorized"; break;
	case 402: s = "payment required"; break;
	case 403: s = "forbidden"; break;
	case 404: s = "not found"; break;
	case 405: s = "method not allowed"; break;
	case 406: s = "not acceptable"; break;
	case 407: s = "proxy authentication required"; break;
	case 408: s = "request timeout"; break;
	case 409: s = "conflict"; break;
	case 410: s = "gone"; break;
	case 411: s = "length required"; break;
	case 412: s = "precondition failed"; break;
	case 413: s = "payload too large"; break;
	case 414: s = "uri too long"; break;
	case 415: s = "unsupported media type"; break;
	case 416: s = "range not satisfiable"; break;
	case 417: s = "expectation failed"; break;
	case 418: s = "I'm a teapot"; break;
	case 422: s = "unprocessable entity"; break;
	case 423: s = "locked"; break;
	case 424: s = "failed dependency"; break;
	case 426: s = "upgrade required"; break;
	case 428: s = "precondition required"; break;
	case 429: s = "too many requests"; break;
	case 431: s = "request header fields too large"; break;
	case 500: s = "internal server error"; break;
	case 501: s = "not implemented"; break;
	case 502: s = "bad gateway"; break;
	case 503: s = "service unavailable"; break;
	case 504: s = "gateway timeout"; break;
	case 505: s = "http version not supported"; break;
	case 506: s = "variant also negotiates"; break;
	case 507: s = "insufficient storage"; break;
	case 508: s = "loop detected"; break;
	case 510: s = "not extended"; break;
	case 511: s = "network authentication required"; break;
	}
	return (s);
}

static int
stom(char *s)
{
	struct method *m;
	int c, h, l, md;

	l = 0;
	h = CgiMax - 1;
	while (l <= h) {
		md = (l+h) / 2;
		m = methodtab + md;
		c = strcmp(s, m->from);
		if (c > 0)
			l = md + 1;
		else if (c < 0)
			 h = md - 1;
		else
			return (m->to);
	}
	return (-1);
}

char *
cgishift(struct request *r)
{
	char *p;

	p = strsep(&r->path, "/");
	return (p);
}

char *
cgiis(struct request *r, char *s)
{
	char *p;

	p = NULL;
	if (r->type)
		p = strstr(r->type, s);
	return (p);
}

char *
cgiaccepts(struct request *r, char *s)
{
	char *p;

	p = NULL;
	if (r->accept)
		p = strstr(r->accept, s);
	return (p);
}

void
cgiredirect(struct request *req, struct response *res, char *path)
{
	char *fmt;
	int *s;

	for (s = redirects; *s && *s != res->status; ++s);
	if (*s == 0)
		res->status = 303;
	mapset(res->h, "location", path);
	if (cgiaccepts(req, "html")) {
		res->type = "text/html;charset=utf-8";
		fmt = "redirecting to <a href=\"%s\">link</a>";
	} else {
		res->type = "text/plain;charset=utf-8";
		fmt = "redirecting to %s";
	}
	bufwrite(&res->body, fmt, path);
}

int
cgiparse(struct request *r, char **envv)
{
	struct map *m[NHASH], **h, *c;
	char *cookie, *key, *val, **v;

	memset(m, 0, NHASH*sizeof(*m));
	while ((val = *envv++))
		if ((key = strsep(&val, "="))) {
			key = strlc(key);
			key = strrplc(key, '_', '-');
			mapadd(m, key, val);
		}

	r->method = stom(mapget(m, "request-method"));
	r->path = mapget(m, "path-info") + 1;
	r->type = mapget(m, "content-type");
	r->accept = mapget(m, "http-accept");
	r->ip = mapget(m, "http-x-forwarded-for");
	r->qs = mapget(m, "query-string");

	cookie = mapget(m, "http-cookie");
	while ((val = strsep(&cookie, ";")))
		if ((key = strsep(&val, "="))) {
			key = strlc(key);
			key = strtrim(key);
			mapadd(r->h, key, val);
		}

	for (h = m; h < &m[NHASH]; ++h)
		for (c = *h; c; c = c->next)
			if ((key = strsep(&c->key, "-"))
			&& strcmp(key, "http") == 0)
				for (v = c->val; *v; ++v)
					mapadd(r->h, c->key, *v);
	return (r->method);
}

void
cgiserve(struct response *r)
{
	struct map **m, *c;
	char **v;

	if (r->status == 0) {
		if (r->body.len == 0)
			r->status = 204;
		else
			r->status = 200;
	}

	if (r->type == NULL)
		r->type = "text/html;charset=utf-8";
	printf("status: %d %s\r\n", r->status, ctos(r->status));
	for (m = r->h; m < &r->h[NHASH]; ++m)
		for (c = *m; c; c = c->next)
			for (v = c->val; *v; ++v)
				printf("%s: %s\r\n", c->key, *v);
	printf("content-type: %s\r\n", r->type);
	printf("content-length: %d\r\n", r->body.len);
	printf("\r\n");
	if (r->body.len > 0)
		printf("%.*s", r->body.len, r->body.s);
}
