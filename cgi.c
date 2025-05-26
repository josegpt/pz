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

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"

static char	*sevtab[] = {
	"DBG",
	"INF",
	"WRN",
	"ERR",
	"FTL"
};

static int	redirects[] = {301, 302, 303, 307, 308, 0};

static struct method {
	char	*from;
	int	 to;
} methodtab[] = {
	{"CONNECT",	Connect},
	{"DELETE",	Delete},
	{"GET",		Get},
	{"HEAD",	Head},
	{"OPTIONS",	Options},
	{"PATCH",	Patch},
	{"POST",	Post},
	{"PUT",		Put},
	{"TRACE",	Trace}
};

char *
lowercase(char *s)
{
	char *p;

	for (p=s; *p; ++p)
		*p = tolower(*p);
	return (s);
}

char *
uppercase(char *s)
{
	char *p;

	for (p=s; *p; ++p)
		*p = toupper(*p);
	return (s);
}

char *
replace(char *s, char o, char n)
{
	char *p;

	for (p=s; *p; ++p)
		if (*p==o)
			*p = n;
	return (s);
}

char *
triml(char *s)
{
	for (; isspace(*s); ++s);
	return (s);
}

char *
trimr(char *s)
{
	char *p;

	for (p=s+strlen(s); isspace(*++p););
	*++p ='\0';
	return (s);
}

char *
trims(char *s)
{
	s = triml(s);
	s = trimr(s);
	return (s);
}

static void
logv(int sev, char *fmt, va_list ap)
{
	char *prefix, *prog, buf[256], *s;

	s      = sevtab[sev];
	prefix = "[%s] %s: ";
	prog   = (char *)getprogname();
	sprintf(buf, prefix, s, prog);
	strcat(buf, fmt);
	vfprintf(stderr, buf, ap);
}

void
info(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(Info, fmt, ap);
	va_end(ap);
}

void
debug(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(Debug, fmt, ap);
	va_end(ap);
}

void
warn(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(Warn, fmt, ap);
	va_end(ap);
}

void
err(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(Error, fmt, ap);
	va_end(ap);
}

void
fatal(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(Fatal, fmt, ap);
	va_end(ap);
	exit(1);
}

int
writev(struct buffer *b, char *fmt, va_list ap)
{
	int res;

	if (b->at == NULL)
		b->at = b->s;

	res    = vsprintf(b->at, fmt, ap);
	b->at += res;
	return (res);
}

int
writef(struct buffer *b, char *fmt, ...)
{
	va_list ap;
	int res;

	va_start(ap, fmt);
	res = writev(b, fmt, ap);
	va_end(ap);
	return (res);
}

int
writen(struct buffer *b, char *fmt, ...)
{
	va_list ap;
	char buf[256], end[3];
	int res;

	end[0] = '\r';
	end[1] = '\n';
	strcpy(buf, fmt);
	strcat(buf, end);

	va_start(ap, fmt);
	res = writev(b, buf, ap);
	va_end(ap);
	return (res);
}

static unsigned long
hash(char *key)
{
	unsigned long res;
	int c;

	res = 5381;
	while ((c = *key++))
		res = ((res<<5) + res) + c;
	return (res % NHASH);
}

static struct value *
lookup(struct map *m, char *key)
{
	struct value *res;
	unsigned long i, hashed;
	char **keys, *s;

	hashed = hash(key);
	i      = hashed;
	keys   = m->keys;
	while ((s = keys[i]) && strcmp(s, key)) {
		i = (i + 1) % NHASH;
		assert(i != hashed);
	}

	keys[i] = key;
	res     = &m->vals[i];
	if (res->at == NULL)
		res->at = res->ss;

	return (res);
}

void
add(struct map *m, char *key, char *val)
{
	struct value *v;
	int diff;

	v    = lookup(m, key);
	diff = v->at - v->ss;
	assert(diff < VALSZ);
	*v->at++ = val;
}

void
set(struct map *m, char *key, char *val)
{
	struct value *v;

	v        = lookup(m, key);
	v->at    = v->ss;
	*v->at++ = val;
}

char *
get(struct map *m, char *key)
{
	struct value *v;
	char *res;
	int diff;

	v    = lookup(m, key);
	diff = v->at - v->ss;
	res  = diff > 0 ? *v->ss : "";
	return (res);
}

int
has(struct map *m, char *key)
{
	char *s;
	int res;

	s   = get(m, key);
	res = *s != '\0';
	return (res);
}

/* todo: move to a table */
char *
sttstr(int status)
{
	char *s;

	switch (status) {
	case 100: s = "continue"; break;
	case 101: s = "switching Protocols"; break;
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
	case 418: s = "i'm a teapot"; break;
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
	default:  s = "internal server error"; break;
	}
	return (s);
}

char *
shift(struct request *req)
{
	char *p;

	if (*req->path == '/')
		++req->path;

	p = strsep(&req->path, "/");

	if (req->path == NULL)
		req->path = "";

	return (p);
}

char *
istype(struct request *req, char *s)
{
	char *p;

	p = strstr(req->type, s);
	return (p);
}

char *
accepts(struct request *req, char *s)
{
	char *p;

	p = strstr(req->accept, s);
	return (p);
}

void
redirect(struct response *res, struct request *req, char *path)
{
	char *fmt;
	int stt, *stts;

	stt = res->status;
	for (stts = redirects; *stts && *stts != stt; ++stts);
	if (*stts == 0)
		res->status = 303;

	set(&res->header, "location", path);
	if (accepts(req, "html")) {
		res->type = "text/html;charset=utf-8";
		fmt = "redirecting to <a href=\"%s\">link</a>";
	} else {
		res->type = "text/plain;charset=utf-8";
		fmt = "redirecting to %s";
	}

	writef(&res->body, fmt, path);
}

void
parse(struct request *req, char **envv)
{
	struct method *low, *high, *mid;
	struct map *h;
	char *key, *val, *s;
	int cmp;

	h = &req->header;
	while ((val = *envv++))
		if ((key = strsep(&val, "="))) {
			key = lowercase(key);
			key = replace(key, '_', '-');
			add(h, key, val);
		}

	low  = methodtab;
	cmp  = -1;
	high = methodtab + Trace;
	s    = get(h, "request-method");
	while (low <= high && cmp) {
		mid  = low + (high - low) / 2;
		cmp  = strcmp(s, mid->from);
		if (cmp > 0)
			low = mid + 1;
		else if (cmp < 0)
			high = mid - 1;
	}

	req->method = cmp == 0 ? mid->to : Get;
	req->path   = get(h, "path-info");
	req->type   = get(h, "content-type");
	req->accept = get(h, "http-accept");
	req->ip     = get(h, "http-x-forwarded-for");
	req->query  = get(h, "query-string");
}

void
render(struct response *res)
{
	struct buffer *b;
	struct value *vs, *v;
	char **keys, *key, **vals;
	int i, status, len;

	b = &res->body;
	if (b->at == NULL)
		len = 0;
	else
		len = b->at - b->s;

	if (res->status == 0) {
		if (len == 0)
			res->status = 204;
		else
			res->status = 200;
	}

	if (res->type == NULL)
		res->type = "text/html;charset=utf-8";

	status = res->status;
	printf("status: %d %s\r\n", status, sttstr(status));

	keys = res->header.keys;
	vs   = res->header.vals;
	for (i = 0; i < NHASH; ++i) {
		key = *(keys + i);
		v   = vs + i;
		if (key) for (vals = v->ss; vals < v->at; ++vals)
			printf("%s: %s\n", key, *vals);
	}

	printf("content-type: %s\r\n", res->type);
	printf("content-length: %d\r\n", len);
	printf("\r\n");
	printf("%s", b->s);
}
