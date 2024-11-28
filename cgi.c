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
ctos(int code)
{
	char *s;

	switch (code) {
	case 100: s = "Continue"; break;
	case 101: s = "Switching Protocols"; break;
	case 102: s = "Processing"; break;
	case 200: s = "OK"; break;
	case 201: s = "Created"; break;
	case 202: s = "Accepted"; break;
	case 203: s = "Non-Authoritative Information"; break;
	case 204: s = "No Content"; break;
	case 205: s = "Reset Content"; break;
	case 206: s = "Partial Content"; break;
	case 207: s = "Multi-Status"; break;
	case 208: s = "Already Reported"; break;
	case 226: s = "IM Used"; break;
	case 300: s = "Multiple Choices"; break;
	case 301: s = "Moved Permanently"; break;
	case 302: s = "Found"; break;
	case 303: s = "See Other"; break;
	case 304: s = "Not Modified"; break;
	case 305: s = "Use Proxy"; break;
	case 307: s = "Temporary Redirect"; break;
	case 308: s = "Permanent Redirect"; break;
	case 400: s = "Bad Request"; break;
	case 401: s = "Unauthorized"; break;
	case 402: s = "Payment Required"; break;
	case 403: s = "Forbidden"; break;
	case 404: s = "Not Found"; break;
	case 405: s = "Method Not Allowed"; break;
	case 406: s = "Not Acceptable"; break;
	case 407: s = "Proxy Authentication Required"; break;
	case 408: s = "Request Timeout"; break;
	case 409: s = "Conflict"; break;
	case 410: s = "Gone"; break;
	case 411: s = "Length Required"; break;
	case 412: s = "Precondition Failed"; break;
	case 413: s = "Payload Too Large"; break;
	case 414: s = "Uri Too Long"; break;
	case 415: s = "Unsupported Media Type"; break;
	case 416: s = "Range Not Satisfiable"; break;
	case 417: s = "Expectation Failed"; break;
	case 418: s = "I'm a teapot"; break;
	case 422: s = "Unprocessable Entity"; break;
	case 423: s = "Locked"; break;
	case 424: s = "Failed Dependency"; break;
	case 426: s = "Upgrade Required"; break;
	case 428: s = "Precondition Required"; break;
	case 429: s = "Too Many Requests"; break;
	case 431: s = "Request Header Fields Too Large"; break;
	case 500: s = "Internal Server Error"; break;
	case 501: s = "Not Implemented"; break;
	case 502: s = "Bad Gateway"; break;
	case 503: s = "Service Unavailable"; break;
	case 504: s = "Gateway Timeout"; break;
	case 505: s = "HTTP Version Not Supported"; break;
	case 506: s = "Variant Also Negotiates"; break;
	case 507: s = "Insufficient Storage"; break;
	case 508: s = "Loop Detected"; break;
	case 510: s = "Not Extended"; break;
	case 511: s = "Network Authentication Required"; break;
	}
	return (s);
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
