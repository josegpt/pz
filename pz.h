#define NHASH	4096

struct map {
	char*	k;
	void*	v;
	struct	map *n;
};

void	mapset(struct map**, char*, void*);
void*	mapget(struct map**, char*);
int	maphas(struct map**, char*);
void	mapdel(struct map**, char*);
