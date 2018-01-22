/*
 * Copyright 2009-2018 Peter Kosyh <p.kosyh at gmail.com>
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

#ifndef __GAME_H__
#define __GAME_H__
#include <SDL_mixer.h>
#include "utils.h"

#define SND_CHANNELS MIX_CHANNELS

#ifndef GAMES_PATH
#define GAMES_PATH "./games"
#endif

#define HZ 		100

extern int	game_wait_use;
extern int	game_running;
extern int	game_theme_changed;

extern int	nosound_sw;
extern int	debug_sw;
extern int	fullscreen_sw;
extern int	hires_sw;
extern int	window_sw;
extern int	nopause_sw;
extern int	nocursor_sw;
extern int	game_own_theme; /* current game has own theme */
extern char	*games_sw;
extern char	game_cwd[PATH_MAX]; /* current game cwd */
extern char	*curgame_dir;
extern int	vsync_sw;
extern int	resizable_sw;
extern int	scale_sw;
extern int	standalone_sw;
extern char	*render_sw;

extern char	*appdir(void);

extern void	game_clear(int x, int y, int w, int h);
extern void	game_clear_all(void);

extern char	*game_local_games_path(int cr);
extern char	*game_local_themes_path(void);
extern char	*game_tmp_path(void);
extern int	game_theme_select(const char *name);

extern int	game_init(const char *game);
extern void	game_flip(void);
extern int	game_loop(void);
extern void	game_done(int err);
extern char	*game_reset_name(void);

extern int	game_load_theme(const char *path);
extern int	game_apply_theme(void);
extern int	game_use_theme(void);
extern void	game_release_theme(int force);
extern int	game_reset(void);
extern int	game_cfg_save(void);

extern void 	game_music_player(void);

extern void	game_stop_mus(int ms);

extern int 	game_change_vol(int d, int val);
extern int 	game_change_hz(int hz);

extern int 	games_lookup(const char *path);
extern struct game *game_lookup(const char *name);

extern int	is_game(const char *games, const char *name);
extern int	games_remove(int nr);
extern int	games_replace(const char *path, const char *dir);
extern int 	games_rename(void);

extern void 	game_res_err_msg(const char *s, int alert);
extern int 	game_error(void);

extern int	game_restart(void);
extern int	game_select(const char *name);

extern void game_gfx_clip(void);

#define GAME_CMD_CLICK 1
#define GAME_CMD_FILE  2
#define GAME_CMD_NOHL  4

extern int	game_cmd(char *cmd, int flags);

extern void	game_menu(int nr); /* select and show menu */
extern int	game_menu_box(int show, const char *txt); /* show menu */
extern int	game_menu_box_wh(const char *txt, int *w, int *h);
extern int	game_menu_box_width(int show, const char *txt, int width);

extern void mouse_reset(int hl);
extern int mouse_restore(void);

extern int	game_load(int nr);
extern int	game_save(int nr);
extern int	game_saves_enabled(void);

extern char 	*game_cfg_path(void);
extern char 	*game_save_path(int rc, int nr);

extern char 	*game_locale(void);

extern int	game_freezed(void);

extern int	game_paused(void);

extern char	*open_file_dialog(void);

extern int	game_from_disk(void);

extern int	game_pict_modify(img_t p);
extern int	game_bg_modify(img_t p);

extern int game_pict_coord(int *x, int *y, int *w, int *h);
extern void menu_toggle(int menu);
extern int menu_visible(void);
extern void game_channel_finished(int channel);

extern void sound_play_click(void);
extern void *sound_get(const char *fname);
extern void sound_put(void *sn);

extern int game_tag_valid(const char *p);

void game_gfx_commit(int sync);

#define CURSOR_CLEAR -1
#define CURSOR_OFF    0
#define CURSOR_ON     1
#define CURSOR_DRAW   2

extern void 	game_cursor(int on); /* must be called with -1 before gfx change and 1 after, 2 - not update */
extern int game_cursor_show;

struct game {
	char *path;
	char *name;
	char *author;
	char *version;
	char *info;
	char *dir;
/*	char *api; */
	int idf;
};

extern struct	game *games;
extern int	games_nr;

extern unsigned long timer_counter;
#endif
