/*
 * Copyright 2009-2026 Peter Kosyh <pkosyh at yandex.ru>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "externals.h"
#include "internals.h"

#include <SDL3/SDL.h>

extern SDL_Window *SDL_VideoWindow;

static SDL_AtomicInt dialog_done;
static char dialog_result[PATH_MAX];
static char dialog_dir[PATH_MAX];
static int dialog_dir_set = 0;

static const SDL_DialogFileFilter dialog_filters[] = {
	{ "*", "*" },
	{ "main?.lua; *.zip; *.idf", "*.zip;main.lua;main3.lua;*.idf" },
};

static void dialog_response(void *userdata, const char * const *filelist, int filter)
{
	(void)userdata;
	(void)filter;
	if (filelist && filelist[0] && filelist[0][0]) {
		snprintf(dialog_result, sizeof(dialog_result), "%s", filelist[0]);
		unix_path(dialog_result);
	}
	SDL_MemoryBarrierRelease();
	SDL_SetAtomicInt(&dialog_done, 1);
}

char *open_file_dialog(void)
{
	static char file[PATH_MAX];
	file[0] = 0;
	dialog_result[0] = 0;
	SDL_SetAtomicInt(&dialog_done, 0);
	SDL_ShowOpenFileDialog(dialog_response, NULL, SDL_VideoWindow,
		dialog_filters, (int)(sizeof(dialog_filters) / sizeof(dialog_filters[0])),
		dialog_dir_set ? dialog_dir : NULL, false);
	while (!SDL_GetAtomicInt(&dialog_done)) {
		SDL_PumpEvents();
		SDL_Delay(10);
	}
	SDL_MemoryBarrierAcquire();
	if (!dialog_result[0])
		return NULL;
	snprintf(file, sizeof(file), "%s", dialog_result);
	snprintf(dialog_dir, sizeof(dialog_dir), "%s", dialog_result);
	dirname(dialog_dir);
	dialog_dir_set = 1;
	return file;
}
