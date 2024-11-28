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

#include "log.h"

enum {
	Debug,
	Info,
	Warn,
	Err,
	Fatal
};

static void
logf(int sev, char *fmt, va_list ap)
{

	char *p;

	switch (sev) {
	case Debug:
		p = "DEBUG";
		break;
	case Warn:
		p = "WARN";
		break;
	case Err:
		p = "ERR";
		break;
	case Fatal:
		p = "FATAL";
		break;
	case Info:
	default:
		p = "INFO";
	}
	fprintf(stderr, "[%s] %s: ", p, getprogname());
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
}

void
loginfo(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logf(Info, fmt, ap);
	va_end(ap);
}

void
logdebug(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logf(Debug, fmt, ap);
	va_end(ap);
}

void
logwarn(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logf(Warn, fmt, ap);
	va_end(ap);
}

void
logerr(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logf(Err, fmt, ap);
	va_end(ap);
}

void
logfatal(int code, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logf(Fatal, fmt, ap);
	va_end(ap);
	exit(code);
}
