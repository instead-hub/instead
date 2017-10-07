#ifndef _WINRT_EXTERNAL_H
#define _WINRT_EXTERNAL_H

#define PATH_MAX 255
#define putenv(a) ;

/* must be implemented as extern "C" in winrt cpp code */
extern void getAppTempDir(char *lpPathBuffer);

#endif
