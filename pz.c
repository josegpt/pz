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
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pz.h"

static int	redirects[] = {301, 302, 303, 307, 308, 0};

static struct method {
	char*	from;
	int	to;
} mthdtab[MaxMethod] = {
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

char*
strlc(char *s)
{
	char *p;

	for (p=s; *p; ++p)
		*p = tolower(*p);
	return (s);
}

char*
struc(char *s)
{
	char *p;

	for (p=s; *p; ++p)
		*p = toupper(*p);
	return (s);
}

char*
strrplc(char *s, char o, char n)
{
	char *p;

	for (p=s; *p; ++p)
		if (*p==o)
			*p = n;
	return (s);
}

char*
strtriml(char *s)
{
	for (; isspace(*s); ++s);
	return (s);
}

char*
strtrimr(char *s)
{
	char *p;

	for (p = s + strlen(s); isspace(*--p););
	*++p = '\0';
	return (s);
}

char*
strtrim(char *s)
{
	s = strtriml(s);
	s = strtrimr(s);
	return (s);
}

static unsigned long
maphash(char *k)
{
	unsigned long h;

	h = 5381;
	while (*k++)
		h = ((h<<5) + h) + *k;
	return (h%NHASH);
}

static struct map*
mapsearch(struct map **m, char *k)
{
	struct map *c;
	unsigned long h;

	h = maphash(k);
	for (c = m[h]; c && strcmp(k, c->k); c = c->n);
	return (c);
}

static void
mapappend(struct map *m, char *v)
{
	assert(m->l < VLEN);
	m->v[m->l++] = v;
}

static struct map*
maplink(struct map **m, char *k)
{
	struct map *c;
	unsigned long h;

	h = maphash(k);
	c = malloc(sizeof(*c));
	memset(c, 0, sizeof(*c));
	c->k = k;
	c->n = m[h];
	m[h] = c;
	return (c);
}

void
mapadd(struct map **m, char *k, char *v)
{
	struct map *c;

	c = mapsearch(m, k);
	if (c == NULL)
		c = maplink(m, k);
	mapappend(c, v);
}

void
mapset(struct map **m, char *k, char *v)
{
	struct map *c;

	if ((c = mapsearch(m, k)))
		c->l = 0;
	else
		c = maplink(m, k);
	mapappend(c, v);
}

char*
mapget(struct map **m, char *k)
{
	struct map *c;

	c = mapsearch(m, k);
	return (c ? *c->v : NULL);
}

char**
mapgetv(struct map **m, char *k)
{
	struct map *c;

	c = mapsearch(m, k);
	return (c ? c->v : NULL);
}

int
maphas(struct map **m, char *k)
{
	struct map *c;

	c = mapsearch(m, k);
	return (c ? 1 : 0);
}

void
mapdel(struct map **m, char *k)
{
	struct map *c, *p;
	unsigned long h;

	h = maphash(k);
	for (c = m[h], p = NULL; c; p = c, c = c->n)
		if (strcmp(k, c->k) == 0) {
			if (p)
				p->n = c->n;
			else
				m[h] = c->n;
		}
}

int
bufwritev(struct buffer *b, char *fmt, va_list ap)
{
	va_list tp;
	int n;

	if (b->c == 0) {
		b->n = 0;
		b->c = 10<<20;
		b->s = malloc(b->c);
		memset(b->s, 0, b->c);
	}

	va_copy(tp, ap);
	n = vsnprintf(NULL, 0, fmt, tp);
	va_end(tp);

	assert((b->n + n) < b->c);
	b->n += vsnprintf(b->s+b->n, n+1, fmt, ap);
	return (b->n);
}

int
bufwrite(struct buffer *b, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	bufwritev(b, fmt, ap);
	va_end(ap);
	return (b->n);
}

char*
cgishift(struct request *r)
{
	char *p;

	p = strsep(&r->path, "/");
	return (p);
}

char*
cgiis(struct request *r, char *s)
{
	char *p;

	p = NULL;
	if (r->type)
		p = strstr(r->type, s);
	return (p);
}

char*
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
	int *ss;

	for (ss = redirects; *ss && *ss != res->status; ++ss);
	if (*ss == 0)
		res->status = 303;
	mapset(res->h, "location", path);

	if (cgiaccepts(req, "html")) {
		res->type = "text/html;charset=utf-8";
		bufwrite(&res->body, "redirecting to <a href=\"%s\">link</a>", path);
	} else {
		res->type = "text/plain;charset=utf-8";
		bufwrite(&res->body, "redirecting to %s", path);
	}
}

static int
printrn(char *fmt, ...)
{
	va_list ap;
	char buf[256], end[3];
	int n;

	end[0] = '\r';
	end[1] = '\n';
	end[2] = '\0';
	strlcpy(buf, fmt, sizeof(buf));
	strlcat(buf, end, sizeof(buf));

	va_start(ap, fmt);
	n = vprintf(buf, ap);
	va_end(ap);

	return (n);
}

static int
stom(char *s)
{
	struct method *m;
	int c, h, l, md;

	l = 0;
	h = MaxMethod - 1;
	while (l <= h) {
		md = (l + h) / 2;
		m = mthdtab + md;
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

int
cgiparse(struct request *r, char **envv)
{
	struct map *m[NHASH], **h, *c;
	char *k, *v, *ck, **vv;

	memset(m, 0, NHASH*sizeof(*m));
	while ((v = *envv++))
		if ((k = strsep(&v, "="))) {
			k = strlc(k);
			k = strrplc(k, '_', '-');
			mapadd(m, k, v);
		}

	r->method = stom(mapget(m, "request-method"));
	r->path = mapget(m, "path-info") + 1;
	r->type = mapget(m, "content-type");
	r->accept = mapget(m, "http-accept");
	r->ip = mapget(m, "http-x-forwarded-for");
	r->qs = mapget(m, "query-string");

	ck = mapget(m, "http-cookie");
	while ((v = strsep(&ck, ";")))
		if ((k = strsep(&v, "="))) {
			k = strlc(k);
			k = strtrim(k);
			mapadd(r->h, k, v);
		}

	for (h = m; h < &m[NHASH]; ++h)
		for (c = *h; c; c = c->n)
			if ((k = strsep(&c->k, "-")) && strcmp(k, "http") == 0)
				for (vv = c->v; *vv; ++vv)
					mapadd(r->h, c->k, *vv);
	return (r->method);
}

static char*
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

void
cgiserve(struct response *r)
{
	struct map **h, *c;
	char **vv;

	if (r->status == 0) {
		if (r->body.n == 0)
			r->status = 204;
		else
			r->status = 200;
	}

	if (r->type == NULL)
		r->type = "text/html;charset=utf-8";

	printrn("status: %d %s", r->status, ctos(r->status));
	for (h = r->h; h < &r->h[NHASH]; ++h)
		for (c = *h; c; c = c->n)
			for (vv = c->v; *vv; ++vv)
				printrn("%s: %s", c->k, *vv);
	printrn("content-type: %s", r->type);
	printrn("content-length: %d", r->body.n);
	printrn("");

	if (r->body.n > 0)
		printf("%.*s", r->body.n, r->body.s);
}
