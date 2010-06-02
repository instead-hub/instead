#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <locale.h>
#include <langinfo.h>
#ifdef _HAVE_ICONV
#include <iconv.h>
#endif
#include "internals.h"
#ifdef _USE_GTK
#include <gtk/gtk.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

static char save_path[PATH_MAX];
static char local_games_path[PATH_MAX];
static char local_themes_path[PATH_MAX];


void	nsleep(int u)
{
	usleep(u);
}

char *game_locale(void)
{
	char *p;
	char *s;
	p = getenv("LANG");
	if (!p || !(s = strdup(p)))
		return NULL;
	if ((p = strchr(s, '_')))
		*p = 0;
	return s;
}

char *game_tmp_path(void)
{
	static char *tmp="/tmp";
	return tmp;
}

#ifdef _USE_GTK
static volatile int gtk_response = -1;
static void
run_response_handler (GtkDialog *dialog,
                      gint response_id,
                      gpointer data)
{
	gtk_response = response_id;
}
#endif

extern char *BROWSE_MENU;

char *open_file_dialog(void)
{
#ifndef _USE_GTK
	/* unix people don't need win solutions */
	return NULL;
#else
	gulong response_handler;
	char *filename = NULL;
	static char file[PATH_MAX];
	GtkWidget *file_dialog;

	GtkFileFilter *file_filter_all;
	GtkFileFilter *file_filter_zip;
	GtkFileFilter *file_filter_lua;

	file_filter_all = gtk_file_filter_new();
	gtk_file_filter_add_pattern(file_filter_all, "*");
	gtk_file_filter_set_name(file_filter_all, "*");


	file_filter_zip = gtk_file_filter_new();
	gtk_file_filter_add_pattern(file_filter_zip, "*.zip");
	gtk_file_filter_set_name(file_filter_zip, "*.zip");

	file_filter_lua = gtk_file_filter_new();
	gtk_file_filter_add_pattern(file_filter_lua, "main.lua");
	gtk_file_filter_set_name(file_filter_lua, "main.lua");

	
	file[0] = 0;
	file_dialog = gtk_file_chooser_dialog_new (BROWSE_MENU, 
			NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN,   GTK_RESPONSE_ACCEPT, NULL);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog),
		file_filter_all);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog),
		file_filter_zip);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog),
		file_filter_lua);

	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(file_dialog), file_filter_zip);

	response_handler = g_signal_connect (file_dialog, "response",
		G_CALLBACK (run_response_handler), NULL);

	gtk_window_set_modal (GTK_WINDOW (file_dialog), TRUE);
	gtk_widget_show(file_dialog);

	gtk_response = -1; /* dirty, but we need both SDL and gtk */

	while (gtk_response == -1) {
		struct inp_event ev;
		gtk_main_iteration();
		memset(&ev, 0, sizeof(struct inp_event));
		while ((input(&ev, 0)) == AGAIN);
	}
/*	if (gtk_dialog_run (GTK_DIALOG (file_dialog)) == GTK_RESPONSE_ACCEPT) { */
	if (gtk_response == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_dialog));
		if (filename) {
			strcpy(file, filename);
			g_free (filename);
		}
	}
	g_signal_handler_disconnect (GTK_WIDGET(file_dialog), response_handler);
	gtk_widget_destroy(GTK_WIDGET(file_dialog));
	while(gtk_events_pending())
		gtk_main_iteration();
	return (file[0])?file:NULL;
#endif
}

char *game_local_games_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;

	snprintf(local_games_path, sizeof(local_games_path) - 1 , "%s/.instead/games/", pw->pw_dir);
	return local_games_path;
}

char *game_local_themes_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(local_themes_path, sizeof(local_themes_path) - 1 , "%s/.instead/themes/", pw->pw_dir);
	return local_themes_path;
	
}

char *game_cfg_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.insteadrc", pw->pw_dir); /* at home */
	if (!access(save_path, R_OK)) 
		return save_path;
/* no at home? Try in dir */
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/", pw->pw_dir);
	if (mkdir(save_path, S_IRWXU) && errno != EEXIST)
		snprintf(save_path, sizeof(save_path) - 1 , "%s/.insteadrc", pw->pw_dir); /* fallback to home */
	else
		snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/insteadrc", pw->pw_dir);
	return save_path;
}
char *game_save_path(int cr, int nr)
{
	struct passwd *pw;
	if (!curgame_dir)
		return NULL;

	if (!access("saves", R_OK)) {
		if (nr)
			snprintf(save_path, sizeof(save_path) - 1, "saves/save%d", nr);
		else
			snprintf(save_path, sizeof(save_path) - 1, "saves/autosave");
		return save_path;
	}
	
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/", pw->pw_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/saves", pw->pw_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/", pw->pw_dir, curgame_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;	
	if (nr)
		snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/save%d", pw->pw_dir, curgame_dir, nr);
	else
		snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/autosave", pw->pw_dir, curgame_dir);
	return save_path;
}

int debug_init(void)
{
	return 0;
}

void debug_done()
{
	
}

char *sdl_path(char *p)
{
	unix_path(p);
	return p;
}
