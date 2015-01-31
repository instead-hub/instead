/*
 * Copyright 2009-2014 Peter Kosyh <p.kosyh at gmail.com>
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

#ifndef __CONFIG_H_INCLUDED
#define __CONFIG_H_INCLUDED

#define FONT_SZ(v) (SCALABLE_FONT?((v) * (1.0f + ((0.1f * opt_fsize)))):(v))

#define KBD_SMART	0
#define KBD_NORMAL	1
#define KBD_INVERSE	2
#define KBD_MAX		3

#define JUST_THEME	0
#define JUST_NO		1
#define JUST_YES	2
#define JUST_MAX	3

extern char *appdata_sw;
extern int chunksize_sw;

extern int opt_fading;
extern int opt_fsize;
extern int opt_fs;
extern int opt_owntheme;
extern int opt_hl;
extern int opt_hz;
extern int opt_vol;
extern int opt_motion;
extern int opt_click;
extern int opt_music;
extern int opt_autosave;
extern int opt_filter;
extern int opt_kbd;
extern int opt_justify;
extern int opt_vsync;
extern int opt_debug;

extern int opt_mode[2];
extern char *opt_game;
extern char *opt_theme;
extern char *opt_lang;

extern int cfg_load(void);
extern int cfg_save(void);

extern int parse_mode(const char *v, void *data);

#endif
