#ifndef __IDF_H_INCLUDED
#define __IDF_H_INCLUDED

struct _idf_t;
typedef struct _idf_t *idf_t;

extern idf_t	idf_init(const char *path);
extern void	idf_done(idf_t idf);

extern SDL_RWops *RWFromIdf(idf_t idf, const char *fname);
extern int idf_create(const char *file, const char *path);

#endif