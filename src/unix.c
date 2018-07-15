/*
 * Copyright 2009-2017 Peter Kosyh <p.kosyh at gmail.com>
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

#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "externals.h"
#include "internals.h"

#ifdef _USE_GTK
#include "input.h"
#include <gtk/gtk.h>
#endif

static char save_path[PATH_MAX];
static char cfg_path[PATH_MAX];
static char local_games_path[PATH_MAX];
static char local_themes_path[PATH_MAX];
static char local_stead_path[PATH_MAX];

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
	static time_t t = 0;
	static char tmp[PATH_MAX]="/tmp/instead-games";
	if (!t) {
		t = time(NULL);
		sprintf(tmp, "/tmp/instead-games-%lu", (unsigned long)t);
	}
	if (mkdir(tmp, S_IRWXU) && errno != EEXIST)
		return NULL;
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
GdkPixbuf *create_pixbuf(const gchar * filename)
{
	gchar path[PATH_MAX];
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	path[0] = 0;
	if (filename[0] != '/') {
		strcpy(path, game_cwd);
		strcat(path, "/");
	}
	strcat(path, filename);
	pixbuf = gdk_pixbuf_new_from_file(path, &error);
	if(!pixbuf) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	return pixbuf;
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

	static int old_dir_set = 0;
	static char file[PATH_MAX];
	static char old_dir[PATH_MAX];

	GtkWidget *file_dialog;

	GtkFileFilter *file_filter_all;
	GtkFileFilter *file_filter_zip;
/*	GtkFileFilter *file_filter_lua; */

	file_filter_all = gtk_file_filter_new();
	gtk_file_filter_add_pattern(file_filter_all, "*");
	gtk_file_filter_set_name(file_filter_all, "*");


	file_filter_zip = gtk_file_filter_new();
	gtk_file_filter_add_pattern(file_filter_zip, "*.zip");
	gtk_file_filter_add_pattern(file_filter_zip, "main.lua");
	gtk_file_filter_add_pattern(file_filter_zip, "main3.lua");
	gtk_file_filter_add_pattern(file_filter_zip, "*.idf");
	gtk_file_filter_set_name(file_filter_zip, "main?.lua; *.zip; *.idf");

/*
	file_filter_lua = gtk_file_filter_new();
	gtk_file_filter_add_pattern(file_filter_lua, "main.lua");
	gtk_file_filter_set_name(file_filter_lua, "main.lua");
*/

	file[0] = 0;
	file_dialog = gtk_file_chooser_dialog_new (BROWSE_MENU, 
			NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN,   GTK_RESPONSE_ACCEPT, NULL);

	if (old_dir_set)
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_dialog),
			old_dir);

	gtk_window_set_icon(GTK_WINDOW(file_dialog), create_pixbuf(ICON_PATH"/sdl_instead.png"));

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog),
		file_filter_all);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog),
		file_filter_zip);

/*	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog),
		file_filter_lua);*/

	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(file_dialog), file_filter_zip);

	response_handler = g_signal_connect (file_dialog, "response",
		G_CALLBACK (run_response_handler), NULL);

	gtk_window_set_modal (GTK_WINDOW (file_dialog), TRUE);
	gtk_widget_show(file_dialog);

	gtk_response = -1; /* dirty, but we need both SDL and gtk */

	while (gtk_response == -1) {
		struct inp_event ev;
		memset(&ev, 0, sizeof(struct inp_event));
		while (gtk_events_pending()) {
			gtk_main_iteration();
			while ((input(&ev, 0)) == AGAIN);
		} 
		while ((input(&ev, 0)) == AGAIN);
		usleep(HZ*100);
	}
	if (gtk_response == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_dialog));
		if (filename) {
			strcpy(file, filename);
			g_free (filename);
		}
		filename = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(file_dialog));
		if (filename) {
			strcpy(old_dir, filename);
			g_free (filename);
			old_dir_set = 1;
		}
	}

	if (gtk_response != GTK_RESPONSE_DELETE_EVENT) {
		g_signal_handler_disconnect (GTK_WIDGET(file_dialog), response_handler);
		gtk_widget_destroy(GTK_WIDGET(file_dialog));
	}
	while(gtk_events_pending())
		gtk_main_iteration();
	return (file[0])?file:NULL;
#endif
}

char *appdir(void)
{
	static char dir[PATH_MAX] = "";
	struct passwd *pw;
#ifdef _LOCAL_APPDATA
	if (!appdata_sw) {
		strcpy(dir, game_cwd);
		strcat(dir, "/appdata");
	}
#endif
	if (appdata_sw)
		strcpy(dir, appdata_sw);
	if (dir[0] && !access(dir, W_OK))
		return dir;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(dir, sizeof(dir) - 1 , "%s/.instead", pw->pw_dir);
	return dir;
}

char *game_local_games_path(int cr)
{
	char *app = appdir();
	if (!app)
		return NULL;
	strcpy(local_games_path, app);
	if (cr) {
		if (mkdir(local_games_path, S_IRWXU) && errno != EEXIST)
			return NULL;
	}
	strcat(local_games_path,"/games");
	if (cr) { 
		if (mkdir(local_games_path, S_IRWXU) && errno != EEXIST)
			return NULL;
	}
	return local_games_path;
}

char *game_local_themes_path(void)
{
	char *app = appdir();
	if (!app)
		return NULL;
	snprintf(local_themes_path, sizeof(local_themes_path) - 1 , "%s/themes", app);
	return local_themes_path;
}

char *instead_local_stead_path(void)
{
	char *app = appdir();
	if (!app)
		return NULL;
	snprintf(local_stead_path, sizeof(local_stead_path) - 1 , "%s/stead", app);
	return local_stead_path;
}

char *game_cfg_path(void)
{
	char *app = appdir();
	struct passwd *pw;
	if (app) {
		snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/", app);
		if (mkdir(cfg_path, S_IRWXU) == 0 || errno == EEXIST) {
			snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/insteadrc", app);
			return cfg_path;
		}
	}

	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;

	snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/.insteadrc", pw->pw_dir); /* at home */
	return cfg_path;
}

char *game_save_path(int cr, int nr)
{
	char *app = appdir();
	if (!curgame_dir)
		return NULL;
	if (!access("saves", R_OK)) {
		if (nr)
			snprintf(save_path, sizeof(save_path) - 1, "saves/save%d", nr);
		else
			snprintf(save_path, sizeof(save_path) - 1, "saves/autosave");
		return save_path;
	}	
	if (!app) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/", app);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/saves", app);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/saves/%s/", app, curgame_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	if (nr)
		snprintf(save_path, sizeof(save_path) - 1, "%s/saves/%s/save%d", app, curgame_dir, nr);
	else
		snprintf(save_path, sizeof(save_path) - 1, "%s/saves/%s/autosave", app, curgame_dir);
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
