#include "externals.h"
#include "internals.h"
#include "idf.h"
#include "list.h"
#include <SDL/SDL_rwops.h>

typedef struct _idfd_t {
	unsigned long offset;
	unsigned long size;
	idf_t			idf;
} idfd_t;

struct _idff_t {
	idfd_t		*dir;
	unsigned long pos;
	FILE		*fd;
};

struct _idf_t {
	unsigned long size;
	FILE	*fd;
	char 	*path;
	cache_t	dir;
	int		idfonly;
};

void idf_done(idf_t idf)
{
	if (!idf)
		return;
	if (idf->path)
		free(idf->path);
	if (idf->dir)
		cache_free(idf->dir);
	if (idf->fd)
		fclose(idf->fd);
	free(idf);
}

void idf_shrink(idf_t idf)
{
	if (!idf)
		return;
	if (idf->dir)
		cache_shrink(idf->dir);
}

static int read_word(FILE *fd, unsigned long *w)
{
	unsigned char word[4];
	if (fread(word, 1, 4, fd) != 4)
		return -1;
	*w = (unsigned long)word[0] | ((unsigned long)word[1] << 8) | 
		((unsigned long)word[2] << 16) |
		((unsigned long)word[3] << 24);
	return 0;
}

static int write_word(FILE *fd, unsigned long w)
{
	char word[4];

	word[0] = w & 0xff;
	word[1] = (w & 0xff00) >> 8;
	word[2] = (w & 0xff0000) >> 16;
	word[3] = (w & 0xff000000) >> 24;

	if (fwrite(word, 1, 4, fd) != 4)
		return -1;
	return 0;
}

int	idf_magic(const char *fname)
{
	char sign[4];
	FILE *fd = fopen(dirpath(fname), "rb");
	if (!fd)
		return 0;
	if (fread(sign, 1, 4, fd) != 4) {
		fclose(fd);
		return 0;
	}
	fclose(fd);
	if (!memcmp(sign, "IDF\n", 4))
		return 1;
	return 0;
}

idf_t idf_init(const char *fname)
{
	char sign[4];
	unsigned long dir_size;
	char *fp = dirpath(fname);
	idf_t idf = malloc(sizeof(struct _idf_t));
	if (!idf)
		return NULL;
	idf->path = strdup(fname);
	if (!idf->path)
		goto err;
	idf->idfonly = 0;
	idf->fd = fopen(fp, "rb");
	idf->dir = cache_init(-1, free);
	if (!idf->fd || !idf->dir)
		goto err;
	if (fseek(idf->fd, 0, SEEK_END))
		goto err;
	idf->size = ftell(idf->fd);
	if (idf->size < 0)
		goto err;
	if (fseek(idf->fd, 0, SEEK_SET))
		goto err;
	if (fread(sign, 1, 4, idf->fd) != 4)
		goto err;
	if (memcmp(sign, "IDF\n", 4))
		goto err;
	if (read_word(idf->fd, &dir_size))
		goto err;
	if (dir_size > idf->size)
		goto err;
	while (dir_size > 0) {
		unsigned long off;
		unsigned long size;
		unsigned char sz;
		char name[256];
		idfd_t *e;
		if (fread(&sz, 1, 1, idf->fd) != 1)
			goto err;
		if (fread(name, 1, sz, idf->fd) != sz)
			goto err;
		name[sz] = 0;
		if (read_word(idf->fd, &off))
			goto err;
		if (read_word(idf->fd, &size))
			goto err;
		e = malloc(sizeof(idfd_t));
		if (!e)
			goto err;
		e->offset = off;
		e->size = size;
		e->idf = idf;
		if (cache_add(idf->dir, name, e)) {
			free(e);
			goto err;
		}
		cache_forget(idf->dir, e); /* use like hash */
//		fprintf(stderr,"Parsed: '%s' @ %ld, %ld\n", name, off, size);
		dir_size -= (1 + sz + 4 + 4);
	}
	return idf;
err:
	idf_done(idf);
	return NULL;
}

typedef struct {
	struct list_head list;
	char *path;
	long size;
} idf_item_t;

static int fcopy(FILE *to, const char *fname)
{
	int rc = -1;
	char buff[4096];
	FILE *fd;
	fd = fopen(fname, "rb");
	if (!fd)
		return -1;
	while (!feof(fd)) {
		int s = fread(buff, 1, sizeof(buff), fd);
		if (!s) {
			if (feof(fd))
				break;
			goto err;
		}
		if (fwrite(buff, 1, s, to) != s)
			goto err;
	}
	rc = 0;
err:
	fclose(fd);
	return rc;
}

static int idf_tree(const char *path, struct list_head *list, const char *fname)
{
	DIR *d;
	struct dirent *de;
	if (!path)
		return 0;
	d = opendir(dirpath(path));
	if (!d) {
		if (!access(dirpath(path), R_OK) && fname) {
			FILE *fd; idf_item_t *i; 
			fd = fopen(dirpath(path), "rb");
			i = malloc(sizeof(idf_item_t));
			if (!i)
				return -1;
			INIT_LIST_HEAD(&i->list);
			if (!(i->path = strdup(fname)))
				goto err;
			if (fseek(fd, 0, SEEK_END) < 0)
				goto err;
			if ((i->size = ftell(fd)) < 0)
				goto err;
			fclose(fd);
			fprintf(stderr, "Added file: '%s' size: %ld\n", path, i->size);
			list_add(&i->list, list);
			return 0;
		err:
			if (i->path)
				free(i->path);
			free(i);
			return -1;
		}
		return 0;
	}
	while ((de = readdir(d))) {
		char *p;
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		p = getfilepath(path, de->d_name);
		if (p) {
			char *pp = getfilepath(fname, de->d_name);
			if (pp) {
				idf_tree(p, list, pp);
				free(pp);
			}
			free(p);
		}
	}
	closedir(d);
	return 0;
}

int idf_create(const char *file, const char *path)
{
	int rc = -1, i;
	FILE *fd;
	char *p;
	unsigned long off = 0;
	long dict_size = 0;
	struct list_head *pos;

	LIST_HEAD(items);
	p = strdup(path);
	if (!p)
		return -1;

	unix_path(p);

	i = strlen(p) - 1;

	while (i >= 0 && p[i] == '/') {
		p[i --] = 0;
	}

	idf_tree(p, &items, NULL);

	free(p);

	list_for_each(pos, &items) {
		idf_item_t *it = (idf_item_t *)pos;
		dict_size += (1 + strlen(it->path) + 4 + 4);
	}

	fd = fopen(dirpath(file), "wb");
	fwrite("IDF\n", 1, 4, fd);
	write_word(fd, dict_size);
	off = 4 + 4 + dict_size;

	list_for_each(pos, &items) {
		unsigned char s;
		idf_item_t *it = (idf_item_t *)pos;
		s = strlen(it->path);
		if (fwrite(&s, 1, 1, fd) != 1)
			goto err;
		p = strdup(it->path);
		if (!p)
			goto err;
		tolow(p); /* in idf always lowcase */
		if (fwrite(p, 1, s, fd) != s) {
			free(p);
			goto err;
		}
		free(p);
		if (write_word(fd, off) < 0)
			goto err;
		if (write_word(fd, it->size) < 0)
			goto err;
		off += it->size;
	}

	list_for_each(pos, &items) {
		idf_item_t *it = (idf_item_t *)pos;
		char *p;
		p = getfilepath(path, it->path);
		if (p) {
			int rc = fcopy(fd, p);
			free(p);
			if (rc) {
				fprintf(stderr, "Error while copy file '%s'...\n", it->path);
				goto err;
			}
		}
	}
	rc = 0;
err:
	if (rc)
		fprintf(stderr, "Error creating idf file...\n");

	while (!list_empty(&items)) {
		idf_item_t *it = (idf_item_t *)items.next;
		free(it->path);
		list_del(&it->list);
		free(it);
	}
	fclose(fd);
	return rc;
}


int idf_seek(idff_t fil, int offset, int whence)
{
	idfd_t *dir = fil->dir;
	switch (whence) {
	case SEEK_SET:
		if (offset < 0 || offset > dir->size) {
			return -1;
		}
		fil->pos = offset;
		break;
	case SEEK_END:
		if (dir->size + offset > dir->size || dir->size + offset < 0) {
			return -1;
		}
		fil->pos = dir->size + offset;
		break;
	case SEEK_CUR:
		if (fil->pos + offset > dir->size || fil->pos + offset < 0) {
			return -1;
		}
		fil->pos += offset;
		break;
	}
	if (!fseek(fil->fd, fil->pos + dir->offset, SEEK_SET))
		return fil->pos;
	return -1;
}

static int idfrw_seek(struct SDL_RWops *context, int offset, int whence)
{
	idff_t fil = (idff_t)context->hidden.unknown.data1;
	return idf_seek(fil, offset, whence);
}	

int idf_read(idff_t fil, void *ptr, int size, int maxnum)
{
	int rc = 0;
	long pos;

	idfd_t *dir = fil->dir;

	if (fseek(fil->fd, dir->offset + fil->pos, SEEK_SET) < 0) {
		return 0;
	}
#if 1
	while (maxnum) {
		pos = ftell(fil->fd);
		fil->pos = pos - dir->offset;

		if (fil->pos + size > dir->size) {
			break;
		}

		if (fread(ptr, size, 1, fil->fd) != 1)
			break;
//		fil->pos += size;
		ptr += size;
		maxnum --;
		rc ++;
	}
#else
	rc = fread(ptr, size, maxnum, fil->fd);
#endif
	pos = ftell(fil->fd);
	fil->pos = pos - dir->offset;
	return rc;
}

static int idfrw_read(struct SDL_RWops *context, void *ptr, int size, int maxnum)
{
	idff_t fil = (idff_t)context->hidden.unknown.data1;
	return idf_read(fil, ptr, size, maxnum);
}

int idf_close(idff_t fil)
{
	if (fil) {
		fclose(fil->fd);
		free(fil);
	}
	return 0; /* nothing todo */
}

static 	int idfrw_close(struct SDL_RWops *context)
{
	if (context) {
		idff_t fil = (idff_t)context->hidden.unknown.data1;
		idf_close(fil);
		SDL_FreeRW(context);
	}
	return 0;
}

#if 0
int idf_extract(idf_t idf, const char *fname)
{
	FILE *out;
	int size;
	idfd_t *dir = NULL;
	char buff[4096];
	if (idf)
		dir = cache_lookup(idf->dir, fname);
	if (!dir)
		return -1;
	fseek(idf->fd, dir->offset, SEEK_SET);
	out = fopen("out.bin", "wb");
	size = dir->size;
	while (size>0) {
		int s ;
		if (size < sizeof(buff))
			s = fread(buff, 1, size, idf->fd);
		else
			s = fread(buff, 1, sizeof(buff), idf->fd);
		fwrite(buff, 1, s, out);
		size -= s;
		fprintf(stderr, "size = %d\n", size);
	}
	fclose(out);
	return 0;
}
#endif
int idf_eof(idff_t idf)
{
	if (!idf)
		return 1;
	if (idf->pos >= idf->dir->size)
		return 1;
	return 0;
}

int idf_error(idff_t idf)
{
	if (!idf || !idf->fd)
		return -1;
	return ferror(idf->fd);
}

int idf_only(idf_t idf, int fl)
{	int i;
	if (!idf)
		return -1;
	if (fl == -1)
		return idf->idfonly;
	i = idf->idfonly;
	idf->idfonly = fl;
	return i;
}

idff_t idf_open(idf_t idf, const char *fname)
{
	idfd_t *dir = NULL;
	idff_t fil = NULL;
	char *p;
	if (!idf || !fname)
		return NULL;
	p = strdup(fname);
	if (!p)
		return NULL;
	tolow(p);
	if (idf)
		dir = cache_lookup(idf->dir, p);
	free(p);
	if (!dir)
		return NULL;

	fil = malloc(sizeof(struct _idff_t));
	if (!fil)
		return NULL;

	fil->dir = dir;
	fil->pos = 0;
	fil->fd = fopen(dirpath(idf->path), "rb");
	if (!fil->fd)
		goto err;
	return fil;
err:
	free(fil);
	return NULL;
}

int idf_access(idf_t idf, const char *fname)
{
	idfd_t *dir = NULL;
	if (idf)
		dir = cache_lookup(idf->dir, fname);
	if (!dir)
		return -1;
	return 0;
}

char *idf_gets(idff_t idf, char *b, int size)
{
	int rc, rc2;
	if (!idf) 
		return NULL;
	if (!size)
		return NULL;
	rc = idf_read(idf, b, 1, size);
	if (rc < 0)
		return NULL;
	if (!rc && idf_eof(idf))
		return NULL;
	b[rc - 1] = 0;
	rc2 = strcspn(b, "\n");
	b[rc2] = 0;
	idf_seek(idf, - (rc - rc2 - 1), SEEK_CUR);
	return b;
}

SDL_RWops *RWFromIdf(idf_t idf, const char *fname)
{
	idff_t fil = NULL;
	SDL_RWops *n;
	fil = idf_open(idf, fname);
	if (!fil) {
		if (!idf || !idf->idfonly)
			return SDL_RWFromFile(dirpath(fname), "rb");
		return NULL;
	}
	n = SDL_AllocRW();
	if (!n)
		goto err;
	n->seek = idfrw_seek;
	n->read = idfrw_read;
	n->close = idfrw_close;
	n->hidden.unknown.data1 = fil;
	return n;
err:
	if (n)
		SDL_FreeRW(n);
	free(fil);
	return NULL;
}
