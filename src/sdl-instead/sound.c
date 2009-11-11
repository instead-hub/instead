#include "externals.h"
#include "internals.h"

#include <SDL.h>
#include <SDL_mixer.h>

Mix_Music *music = NULL;

int audio_rate = 22050;
Uint16 audio_format = MIX_DEFAULT_FORMAT; 
int audio_channels = 2;
int audio_buffers = 8192;

static Mix_Music *mus;
static char *next_mus = NULL;
static int   next_fadein = 0;
static int   next_loop = -1;
static SDL_TimerID timer_id = NULL;

static int sound_on = 0;

static void mus_callback(void *aux)
{
	if (!timer_id)
		return;
	if (snd_playing_mus())
		return;
	if (mus)
		snd_free_mus(mus);
	mus = NULL;
	if (next_mus) {
		snd_play_mus(next_mus, next_fadein, next_loop);
		free(next_mus);
		next_mus = NULL;
	}
	SDL_RemoveTimer(timer_id);
	timer_id = NULL;
}

static Uint32 callback(Uint32 interval, void *aux)
{
	push_user_event(mus_callback,  aux);
	return interval;
}

int snd_hz(void)
{
	int freq = 0;
	if (sound_on)
		Mix_QuerySpec(&freq, NULL, NULL);
	return freq;
}

int alsa_sw = 0;
int nosound_sw = 0;

int snd_init(int hz) 
{
	if (nosound_sw)
		return -1;
	if (!hz)
		hz = audio_rate;
	else
		audio_rate = hz;

	audio_buffers = (audio_rate / 11025) * 2048;
	if (!audio_buffers) /* wrong parameter? */
		audio_buffers = 8192;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Unable to init audio!\n");
		return -1;
	}
	
	if (alsa_sw) {
		SDL_AudioInit("alsa");
	}
	
	if (Mix_OpenAudio(hz, audio_format, audio_channels, audio_buffers)) {
		fprintf(stderr, "Unable to open audio!\n");
		return -1;
	}
	sound_on = 1;
	return 0;
}

int snd_volume_mus(int vol)
{
	if (!sound_on)
		return 0;
	Mix_Volume(-1, vol);
	return Mix_VolumeMusic(vol);
}

wav_t	snd_load_wav(const char *fname)
{
	if (!sound_on)
		return NULL;
	if (!fname)
		return NULL;
	return (wav_t)Mix_LoadWAV(fname);
}

void	snd_free_wav(wav_t w)
{	
	if (!w)
		return;	
	Mix_HaltChannel(-1);
	Mix_FreeChunk((Mix_Chunk*)w);
}

void snd_halt_chan(int han)
{
	Mix_HaltChannel(han);
}

Mix_Music *snd_load_mus(const char *fname)
{
	Mix_Music *mus;
	if (!sound_on)
		return NULL;
	mus = Mix_LoadMUS(fname);
	return mus;
}

extern void game_music_finished(void);

int snd_play_mus(char *fname, int ms, int loop)
{
	if (!sound_on)
		return 0;
	if (snd_playing_mus()) {
		if (next_mus) {
			free(next_mus);
		}
		next_mus = strdup(fname);
		next_fadein = ms;
		next_loop = loop;
		if (!timer_id)
			timer_id = SDL_AddTimer(200, callback, NULL);
		return 1;
	}
	if (mus)
		snd_free_mus(mus);

	mus = snd_load_mus(fname);
	if (!mus) {
		fprintf(stderr,"Can't load '%s'.\n", fname);
		return 0;
	}
	if (loop >= 0)
		Mix_HookMusicFinished(game_music_finished);
	else
		Mix_HookMusicFinished(NULL);
	if (ms)
		Mix_FadeInMusic((Mix_Music*)mus, loop, ms);
	else
		Mix_PlayMusic((Mix_Music*)mus, loop);
	snd_volume_mus(snd_volume_mus(-1)); // SDL hack?
	return 0;
}

void snd_stop_mus(int ms)
{
	if (!sound_on)
		return;
	Mix_HookMusicFinished(NULL);
	if (ms)
		Mix_FadeOutMusic(ms);
	else
		Mix_HaltMusic();
}

int snd_playing_mus(void)
{
	if (!sound_on)
		return 0;
	if (Mix_PlayingMusic() | Mix_FadingMusic())
		return 1;
	return 0;
}

void snd_free_mus(mus_t mus)
{
	if (!sound_on)
		return;
	Mix_HaltMusic();
	Mix_FreeMusic((Mix_Music*) mus);
}

void snd_play(void *chunk, int channel, int loop)
{
	if (!sound_on)
		return;
	if (!chunk)
		return;		
	if (channel >= MIX_CHANNELS)
		channel %= MIX_CHANNELS;
	if (channel < 0)
		channel = -1;	
	Mix_PlayChannel(channel, (Mix_Chunk*)chunk, loop);
}

void snd_done(void)
{
	if (!sound_on)
		return;
	if (timer_id)
		
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
	timer_id = NULL;
	if (mus)
		Mix_FreeMusic((Mix_Music*) mus);
	mus = NULL;
	if (next_mus)
		free(next_mus);
	next_mus = NULL;
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int snd_vol_from_pcn(int v)
{
	return (v * 127) / 100;
}

int snd_vol_to_pcn(int v)
{
	return (v * 100) / 127;
}
