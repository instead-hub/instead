#ifndef __IDF_H_INCLUDED
#define __IDF_H_INCLUDED

struct _idf_t;
struct _idff_t;
typedef struct _idf_t *idf_t;
typedef struct _idff_t *idff_t;

extern idf_t	idf_init(const char *path);
extern void	idf_shrink(idf_t idf);
extern void	idf_done(idf_t idf);

extern int idf_create(const char *file, const char *path);
extern idff_t idf_open(idf_t idf, const char *fname);

extern int idf_seek(idff_t fil, int offset, int whence);
extern int idf_read(idff_t fil, void *ptr, int size, int maxnum);
extern int idf_close(idff_t fil);

extern int idf_eof(idff_t idf);
extern int idf_error(idff_t idf);
extern int idf_access(idf_t idf, const char *fname);
extern char *idf_gets(idff_t idf, char *b, int size);
extern int idf_magic(const char *fname);
extern int idf_only(idf_t idf, int fl);

#endif
