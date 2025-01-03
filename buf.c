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
#include <stdarg.h>
#include <stdio.h>

#include "buf.h"

int
bufwritev(struct buffer *b, char *fmt, va_list ap)
{
	va_list tp;
	int n;

	va_copy(tp, ap);
	n = vsnprintf(NULL, 0, fmt, tp);
	va_end(tp);

	assert((b->len + n) < BUFSZ);
	b->len += vsnprintf(b->s+b->len, n+1, fmt, ap);
	return (b->len);
}

int
bufwrite(struct buffer *b, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	bufwritev(b, fmt, ap);
	va_end(ap);
	return (b->len);
}
