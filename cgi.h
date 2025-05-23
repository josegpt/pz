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

#define BUFSZ	65535
#define NHASH	256
#define VALSZ	16

enum {
	Debug,
	Info,
	Warn,
	Error,
	Fatal
};

enum {
	Connect,
	Delete,
	Get,
	Head,
	Options,
	Patch,
	Post,
	Put,
	Trace
};

struct	value {
	char	*ss[VALSZ], **at;
};

struct	map {
	char	*keys[NHASH];
	struct	 value vals[NHASH];
};

struct	buffer {
	char	s[BUFSZ], *at;
};

struct	request {
	char	*path, *type, *accept, *ip, *query;
	int	 method;
	struct	 buffer body;
	struct	 map header;
};

struct	response {
	char	*type;
	int	 status;
	struct	 buffer body;
	struct	 map header;
};

void	 debug(char *, ...);
void	 info(char *, ...);
void	 warn(char *, ...);
void	 error(char *, ...);
void	 fatal(char *, ...);
void	 add(struct map *, char *, char *);
void	 set(struct map *, char *, char *);
char	*get(struct map *, char *);
int	 has(struct map *, char *);
int	 writev(struct buffer *, char *, va_list);
int	 writef(struct buffer *, char *, ...);
int	 writen(struct buffer *, char *, ...);
char	*sttstr(int);
char	*shift(struct request *);
char	*istype(struct request *, char *);
char	*accepts(struct request *, char *);
void	 redirect(struct request *, struct response *, char *);
void	 parse(char **, struct request *);
void	 render(struct response *);
