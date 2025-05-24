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
#include <string.h>

#include "cgi.h"
#include "html.h"

void
htmlhead(struct buffer *b, char *title, char *desc, struct htmllink *l)
{
	writen(b, "<!doctype html>");
	writen(b, "<html lang=en>");
	writen(b, "<meta charset=utf-8>");
	writen(b, "");
	writen(b, "<title>%s</title>", title);
	writen(b, "<meta name=viewport content=\"width=device-width, initial-scale=1\">");
	writen(b, "<meta name=author content=josegpt>");
	writen(b, "<meta name=description content=\"%s\">", desc);
	writen(b, "<meta name=keywords content=\"html, css, c, web, services\">");
	writen(b, "<meta name=theme-color content=#d0021b>");
	writen(b, "<meta property=og:type content=website>");
	writen(b, "<meta property=og:title content=\"%s\">", title);
	writen(b, "<meta property=og:url content=https://pz.do>");
	writen(b, "<meta property=og:description content=\"%s\">", desc);
	writen(b, "<meta property=og:image content=/static/img/banner.png>");
	writen(b, "<link rel=icon href=/static/img/favicon.ico>");
	writen(b, "<link rel=preload as=style href=/static/css/style.css>");
	writen(b, "<link rel=stylesheet href=/static/css/style.css>");
	writen(b, "");
	writen(b, "<body>");
	writen(b, "<header>");
	writen(b, "<a href=/ id=logo>");
	writen(b, "<img src=/static/img/logo.webp alt=pz>");
	writen(b, "</a>");
	writen(b, "</header>");
	writen(b, "<nav data-justify=center class=cluster>");

	for (; l->href; ++l)
		writen(b, "<a href=%s>%s</a>", l->href, l->text);

	writen(b, "</nav>");
	writen(b, "<hr>");
}

void
htmlerr(struct buffer *b, int status, struct htmllink *ls)
{
	char *title, *msg;

	switch (status) {
	case 404:
		msg = "you lost.";
		break;
	case 405:
		msg = "we don't do that here.";
		break;
	default:
		msg = "someting weeeeird happened.";
	}

	title  = sttstr(status);
	htmlhead(b, title, title, ls);
	writen(b, "<main class=stack>");
	writen(b, "<header>");
	writen(b, "<h1>%d</h1>", status);
	writen(b, "<p>%s", msg);
	writen(b, "</header>");
	writen(b, "</main>");
}
