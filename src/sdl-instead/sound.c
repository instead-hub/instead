#include "externals.h"
#include "internals.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include "sdl_idf.h"

#ifdef S60
int audio_rate = 11025;
#else
int audio_rate = 22050;
#endif

Uint16 audio_format = MIX_DEFAULT_FORMAT; 
int audio_channels = 2;
int audio_buffers = 8192;

static mus_t mus;
static char *next_mus = NULL;
static int   next_fadein = 0;
static int   next_loop = -1;
static SDL_TimerID timer_id = NULL_TIMER;

static int sound_on = 0;

struct _mus_t {
	Mix_Music *mus;
	SDL_RWops *rw;
};

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
	timer_id = NULL_TIMER;
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
void snd_pause(int on)
{
	if (!sound_on) 
		return;
	if (on) {
		Mix_Pause(-1);
		Mix_PauseMusic();
	} else {
		Mix_Resume(-1);
		Mix_ResumeMusic();
	}
	return;
}

int snd_init(int hz) 
{
	int chunk;
	if (nosound_sw)
		return -1;
	if (!hz)
		hz = audio_rate;
	else
		audio_rate = hz;

	chunk = (chunksize_sw>0)?chunksize_sw:DEFAULT_CHUNKSIZE;
	audio_buffers = (audio_rate / 11025) * chunk;
	if (audio_buffers <=0) /* wrong parameter? */
		audio_buffers = DEFAULT_CHUNKSIZE;
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
	Mix_ChannelFinished(game_channel_finished);
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
	SDL_RWops *rw;
	wav_t r;
	if (!sound_on)
		return NULL;
	if (!fname || !*fname)
		return NULL;
	rw = RWFromIdf(game_idf, fname);
	if (!rw || !(r = (wav_t)Mix_LoadWAV_RW(rw, 1))) {
		fprintf(stderr,"Can't load '%s'.\n", fname);
		return NULL;
	}
	return r;
}

void	snd_free_wav(wav_t w)
{	
	if (!w)
		return;	
	Mix_HaltChannel(-1);
	Mix_FreeChunk((Mix_Chunk*)w);
}

void snd_halt_chan(int han, int ms)
{
	if (ms)
		Mix_FadeOutChannel(han, ms);
	else
		Mix_HaltChannel(han);
}

mus_t snd_load_mus(const char *fname)
{
	mus_t	mus = NULL;
	if (!sound_on)
		return NULL;
	mus = malloc(sizeof(struct _mus_t));
	if (!mus)
		return NULL;
	mus->rw = RWFromIdf(game_idf, fname);
	if (!mus->rw) 
		goto err;
	mus->mus = Mix_LoadMUS_RW(mus->rw);
	if (!mus->mus)
		goto err1;
	return mus;
err1:
	SDL_RWclose(mus->rw);
err:
	free(mus);
	return NULL;
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
		Mix_FadeInMusic(mus->mus, loop, ms);
	else
		Mix_PlayMusic(mus->mus, loop);
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

int snd_playing(int channel)
{
	if (!sound_on)
		return 0;
	if (channel >= MIX_CHANNELS)
		channel %= MIX_CHANNELS;
	if (channel < 0)
		channel = -1;	
	return Mix_Playing(channel);
}

int snd_panning(int channel, int left, int right)
{
	if (channel >= MIX_CHANNELS)
		channel %= MIX_CHANNELS;
	if (channel < 0)
		channel = -1;	
	return Mix_SetPanning(channel, left, right);
}

#define MIXER_VERSION_ATLEAST(a,b,c)  (SDL_VERSIONNUM(SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION,SDL_MIXER_PATCHLEVEL) >= SDL_VERSIONNUM(a, b, c))

void snd_free_mus(mus_t mus)
{
	int to_close = 0;
	if (!sound_on)
		return;
	if (!mus)
		return;
	Mix_HaltMusic();
	if (mus->mus) {
#ifdef _SDL_MOD_BUG
		if ((Mix_GetMusicType(mus->mus) == MUS_MOD) && !MIXER_VERSION_ATLEAST(1, 2, 12))
			SDL_RWclose(mus->rw);
#endif
		if (MIXER_VERSION_ATLEAST(1, 2, 12) && Mix_GetMusicType(mus->mus) != MUS_MP3) {
			to_close = 1;
		}
		Mix_FreeMusic((Mix_Music*) mus->mus);
		if (to_close)
			SDL_RWclose(mus->rw);
	}
	free(mus);
}

int snd_play(void *chunk, int channel, int loop)
{
	if (!sound_on)
		return -1;
	if (!chunk)
		return -1;		
	if (channel >= MIX_CHANNELS)
		channel %= MIX_CHANNELS;
	if (channel < 0)
		channel = -1;
	if (channel != -1)
		snd_halt_chan(channel, 0);
	snd_volume_mus(snd_volume_mus(-1)); // SDL hack?
	return Mix_PlayChannel(channel, (Mix_Chunk*)chunk, loop);
}

void snd_done(void)
{
	if (!sound_on)
		return;
	Mix_ChannelFinished(game_channel_finished);
	if (timer_id) {
		SDL_RemoveTimer(timer_id);
		timer_id = NULL_TIMER;
	}
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
	if (mus)
		snd_free_mus(mus);
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
