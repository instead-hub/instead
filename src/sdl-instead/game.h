#ifndef __GAME_H__
#define __GAME_H__
#include <SDL_mixer.h>
#include "idf.h"

#define SND_CHANNELS MIX_CHANNELS

#ifndef GAMES_PATH
#define GAMES_PATH "./games"
#endif

#define MAIN_FILE	"main.lua"
#define HZ 		100

extern int	game_running;
extern int	game_theme_changed;

extern int 	nosound_sw;
extern int 	alsa_sw;
extern int	debug_sw;
extern int 	fullscreen_sw;
extern int 	window_sw;
extern int	nopause_sw;
extern int 	game_own_theme; /* current game has own theme */
extern char	*games_sw;
extern char 	*err_msg; /* last error message */
extern char 	game_cwd[PATH_MAX]; /* current game cwd */
extern char 	*curgame_dir;

extern char	*game_local_stead_path(void);

extern char 	*game_local_games_path(int cr);
extern char 	*game_tmp_path(void);
extern int 	game_theme_select(const char *name);

extern int 	game_init(const char *game);

extern int 	game_loop(void);
extern void 	game_done(int err);

extern int 	game_load_theme(const char *path);
extern int 	game_apply_theme(void);
extern int game_use_theme(void);
extern void game_release_theme(void);
extern int game_reset(void);

extern void 	game_music_player(void);
extern void	game_stop_mus(int ms);

extern int 	game_change_vol(int d, int val);
extern int 	game_change_hz(int hz);

extern int 	games_lookup(const char *path);
extern int	games_remove(int nr);
extern int	games_replace(const char *path, const char *dir);
extern int 	games_rename(void);

extern void 	game_err_msg(const char *s);
extern int 	game_error(const char *name);

extern int	game_restart(void);
extern int	game_select(const char *name);
extern int	game_cmd(char *cmd);

extern void	game_menu(int nr); /* select and show menu */
extern void	game_menu_box(int show, const char *txt); /* show menu */
extern int	game_menu_box_wh(const char *txt, int *w, int *h);
extern void	game_menu_box_width(int show, const char *txt, int width);

extern int	game_load(int nr);
extern int	game_save(int nr);
extern int	game_saves_enabled(void);

extern char 	*game_cfg_path(void);
extern char 	*game_save_path(int rc, int nr);

extern char 	*game_locale(void);

extern int	game_paused(void);

extern char	*open_file_dialog(void);

extern int	game_from_disk(void);

extern int	game_pict_modify(img_t p);
extern int game_pict_coord(int *x, int *y, int *w, int *h);
extern void menu_toggle(void);
extern void game_channel_finished(int channel);

extern int sound_load(const char *fname);
extern void sound_unload(const char *fname);
extern void sounds_free(void);
extern const char *sound_channel(int i);

#define CURSOR_CLEAR -1
#define CURSOR_OFF    0
#define CURSOR_ON     1
#define CURSOR_DRAW   2

extern void 	game_cursor(int on); /* must be called with -1 before gfx change and 1 after, 2 - not update */

struct game {
	char *path;
	char *name;
	char *dir;
	int idf;
};

extern struct	game *games;
extern int	games_nr;
extern idf_t	game_idf;
#endif
