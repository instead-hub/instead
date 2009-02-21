#include "sound.h"
#include <SDL.h>
#include <SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>

Mix_Music *music = NULL;

int audio_rate = 22050;
Uint16 audio_format = MIX_DEFAULT_FORMAT; 
int audio_channels = 2;
int audio_buffers = 8192;

static Mix_Music *mus;
static char *next_mus = NULL;
static int   next_fadein = 0;
static SDL_TimerID timer_id = NULL;

static Uint32 callback(Uint32 interval, void *aux)
{
	if (!snd_playing_mus()) {
		if (mus)
			snd_free_mus(mus);
		mus = NULL;
		if (next_mus) {
			snd_play_mus(next_mus, next_fadein);
			free(next_mus);
			next_mus = NULL;
		}
		timer_id = NULL;
		return 0;
	}
	return interval;	
}

int snd_hz(void)
{
	int freq = 0;
	Mix_QuerySpec(&freq, NULL, NULL);
	return freq;
}

int snd_init(int hz) 
{
	if (!hz)
		hz = audio_rate;
	else
		audio_rate = hz;
	if (Mix_OpenAudio(hz, audio_format, audio_channels, audio_buffers)) {
		fprintf(stderr, "Unable to open audio!\n");
		return -1;
	}
	return 0;
}

int snd_volume_mus(int vol)
{
	Mix_Volume(-1, vol);
	return Mix_VolumeMusic(vol);
}

Mix_Music *snd_load_mus(const char *fname)
{
	Mix_Music *mus;
	mus = Mix_LoadMUS(fname);
	return mus;
}


int snd_play_mus(char *fname, int ms)
{
	if (snd_playing_mus()) {
		if (next_mus) {
			free(next_mus);
		}
		next_mus = strdup(fname);
		next_fadein = ms;
		if (!timer_id)
			timer_id = SDL_AddTimer(200, callback, NULL);
		return 1;
	}
	if (mus)
		snd_free_mus(mus);

	mus = snd_load_mus(fname);
	if (!mus)
		return 0;
	
	if (ms)
		Mix_FadeInMusic((Mix_Music*)mus, -1, ms);
	else
		Mix_PlayMusic((Mix_Music*)mus, -1);
	snd_volume_mus(snd_volume_mus(-1)); // hack?
	return 0;
}

void snd_stop_mus(int ms)
{
	if (ms)
		Mix_FadeOutMusic(ms);
	else
		Mix_HaltMusic();
}

int snd_playing_mus(void)
{
	if (Mix_PlayingMusic() | Mix_FadingMusic())
		return 1;
	return 0;
}

void snd_free_mus(mus_t mus)
{
	Mix_HaltMusic();
	Mix_FreeMusic((Mix_Music*) mus);
}

void snd_done(void)
{
	Mix_HaltMusic();	
	if (timer_id)
		SDL_RemoveTimer(timer_id);
	timer_id = NULL;
	if (mus)
		Mix_FreeMusic((Mix_Music*) mus);
	mus = NULL;
	if (next_mus)
		free(next_mus);
	next_mus = NULL;
	Mix_CloseAudio();
}

