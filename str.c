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

#include <ctype.h>
#include <string.h>

#include "str.h"

char *
strlc(char *s)
{
	char *p;

	for (p=s; *p; ++p)
		*p = tolower(*p);
	return (s);
}

char *
struc(char *s)
{
	char *p;

	for (p=s; *p; ++p)
		*p = toupper(*p);
	return (s);
}

char *
strrplc(char *s, char o, char n)
{
	char *p;

	for (p=s; *p; ++p)
		if (*p==o)
			*p = n;
	return (s);
}

char *
strtriml(char *s)
{
	for (; isspace(*s); ++s);
	return (s);
}

char *
strtrimr(char *s)
{
	char *p;

	for (p=s+strlen(s); isspace(*++p););
	*++p ='\0';
	return (s);
}

char *
strtrim(char *s)
{
	s = strtriml(s);
	s = strtrimr(s);
	return (s);
}
