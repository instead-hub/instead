#ifndef __CONFIG_H_INCLUDED
#define __CONFIG_H_INCLUDED

#define FONT_SZ(v) ((v) * (1.0f + ((0.1f * opt_fsize))))

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
extern char *opt_game;
extern char *opt_theme;
extern char *opt_lang;

extern int cfg_load(void);
extern int cfg_save(void);

#endif
