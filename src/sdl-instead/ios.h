#ifndef _IOS_H_INCLUDED
#define _IOS_H_INCLUDED
#define VERSION "2.2.2"
#define STEAD_PATH "stead"
#define THEMES_PATH "themes"
#define GAMES_PATH "games"
#define ICON_PATH "icon"
#define LANG_PATH "lang"

extern int setup_inbox(void);

void set_portrait(int isPortrait);
void correct_iOS_font_size_if_need (void);

#endif