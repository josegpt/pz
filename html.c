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
#include <string.h>

#include "pz.h"

void
html(struct buffer *b, char *fmt, ...)
{
	va_list ap;
	char buf[256], end[3];

	end[0] = '\r';
	end[1] = '\n';
	end[2] = '\0';
	strlcpy(buf, fmt, sizeof(buf));
	strlcat(buf, end, sizeof(buf));

	va_start(ap, fmt);
	bufwritev(b, buf, ap);
	va_end(ap);
}

static void
nl(struct buffer *b)
{
	bufwrite(b, "\r\n");
}

static void
meta(struct buffer *b, char *name, char *content, ...)
{
	va_list ap;

	bufwrite(b, "<meta name=\"%s\" content=\"", name);
	va_start(ap, content);
	bufwritev(b, content, ap);
	va_end(ap);
	bufwrite(b, "\">");
	nl(b);
}

static void
prop(struct buffer *b, char *name, char *content, ...)
{
	va_list ap;

	bufwrite(b, "<meta property=\"og:%s\" content=\"", name);
	va_start(ap, content);
	bufwritev(b, content, ap);
	va_end(ap);
	bufwrite(b, "\">");
	nl(b);
}

static void
favicon(struct buffer *b, char *type, char *href)
{
	html(b, "<link rel=\"%s\" href=\"%s\">", type, href);
}

static void
link(struct buffer *b, char *as, char *href)
{
	html(b, "<link rel=\"preload\" as=\"%s\" href=\"%s\">", as, href);
}

void
htmlheader(struct buffer *b, char *title)
{
	html(b, "<!doctype html>");
	html(b, "<html lang=en>");
	html(b, "<meta charset=utf-8>");
	html(b, "");
	html(b, "<title>pz &sol; %s</title>", title);
	meta(b, "viewport","width=device-width, initial-scale=1");
	meta(b, "robots", "nofollow");
	meta(b, "author", "josegpt");
	meta(b, "description", "pz &sol; %s", title);
	meta(b, "keywords", "web, services, c, broker");
	meta(b, "theme-color", "#000000");
	prop(b, "type", "website");
	prop(b, "title", "pz &sol; %s", title);
	prop(b, "url", "https://design.pz.do");
	prop(b, "description", "pz &sol; %s", title);
	prop(b, "image", "/static/img/banner.png");
	favicon(b, "icon", "/static/img/favicon.ico");
	link(b, "style", "/static/css/style.css");
	html(b, "");
	html(b, "<body>");

	html(b, "<header>");
	html(b, "<a href=\"/\">");
	html(b, "<h1>");
	html(b, "<span>p</span>");
	html(b, "<span>e</span>");
	html(b, "<span>r</span>");
	html(b, "<span>e</span>");
	html(b, "<span>z</span>");
	html(b, "</h1>");
	html(b, "</a>");
	html(b, "<article>");
	html(b, "<span>%s</span>", title);
	html(b, "</article>");
	html(b, "</header>");
}
