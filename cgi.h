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

enum {
	CgiGet,
	CgiHead,
	CgiPost,
	CgiPut,
	CgiPatch,
	CgiDelete,
	CgiConnect,
	CgiOptions,
	CgiTrace,
	CgiMax
};

struct request {
	int	 method;
	char	*path;
	char	*type;
	char	*accept;
	char	*ip;
	char	*qs;
	struct	 buffer body;
	struct	 map *h[NHASH];
};

struct response {
	int	 status;
	char	*type;
	struct	 buffer body;
	struct	 map *h[NHASH];
};

char	*ctos(int);
char	*cgishift(struct request *);
char	*cgiis(struct request *, char *);
char	*cgiaccepts(struct request *, char *);
void	 cgiredirect(struct request *, struct response *, char *);
int	 cgiparse(struct request *, char **);
void	 cgiserve(struct response *);
