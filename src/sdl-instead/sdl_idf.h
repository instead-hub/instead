#ifndef __SDL_IDF_H_INCLUDED
#define __SDL_IDF_H_INCLUDED
#include <SDL_rwops.h>
#include "idf.h"

extern SDL_RWops *RWFromIdf(idf_t idf, const char *fname);
#endif
