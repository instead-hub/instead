#ifndef __GAME_H__
#define __GAME_H__
extern int game_load_theme(const char *path);
extern int game_init(const char *game);
extern void game_done(void);
extern int game_loop(void);
extern int games_lookup(void);
#endif

