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
#include <string.h>

#include "buf.h"
#include "map.h"
#include "cgi.h"
#include "html.h"

int
html(struct response *r, char *fmt, ...)
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
	n = bufwritev(&r->body, buf, ap);
	va_end(ap);
	return (n);
}

void
htmlhead(struct response *r, char *title, char *desc)
{
	html(r, "<!doctype html>");
	html(r, "<html lang=en>");
	html(r, "<meta charset=utf-8>");
	html(r, "");
	html(r, "<title>%s</title>", title);
	html(r, "<meta name=viewport content=\"width=device-width, initial-scale=1\">");
	html(r, "<meta name=author content=josegpt>");
	html(r, "<meta name=description content=\"%s\">", desc);
	html(r, "<meta name=keywords content=\"HTML, CSS, C, Web, Services\">");
	html(r, "<meta name=theme-color content=#d0021b>");
	html(r, "<meta property=og:type content=website>");
	html(r, "<meta property=og:title content=\"%s\">", title);
	html(r, "<meta property=og:url content=https://pz.do>");
	html(r, "<meta property=og:description content=\"%s\">", desc);
	html(r, "<meta property=og:image content=/static/img/banner.png>");
	html(r, "<link rel=icon href=/static/img/favicon.ico>");
	html(r, "<link rel=preload as=style href=/static/css/style.css>");
	html(r, "<link rel=stylesheet href=/static/css/style.css>");
	html(r, "");
	html(r, "<body>");
	html(r, "<header class=stack>");
	html(r, "<nav data-align=center data-justify=space-between class=cluster>");
	html(r, "<a href=/ data-style=tertiary class=logo>");
	html(r, "<img src=/static/img/logo.webp alt=pz>");
	html(r, "</a>");
	html(r, "<article>");
	html(r, "<span>%s</span>", title);
	html(r, "</article>");
	html(r, "</nav>");
	html(r, "</header>");
}

void
htmlerr(struct response *r)
{
	char *title;

	title = ctos(r->status);
	htmlhead(r, title, title);
	html(r, "<main>");
	html(r, "<hgroup>");
	html(r, "<h1>%d</h1>", r->status);
	html(r, "<p>%s", title);
	html(r, "</hgroup>");
	html(r, "<section>");
	html(r, "<a href=/>Go Home</a>");
	html(r, "</section>");
	html(r, "</main>");
}
