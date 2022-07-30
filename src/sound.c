/*
 * Copyright 2009-2022 Peter Kosyh <p.kosyh at gmail.com>
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

#include "externals.h"
#include "internals.h"

#ifdef SAILFISHOS
#include <audioresource.h>
#include <glib.h>
#endif

#include <SDL.h>
#include <SDL_mixer.h>

int audio_rate = 22050;

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

int snd_enabled(void)
{
	return sound_on;
}

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
		if (snd_play_mus(next_mus, next_fadein, next_loop) < 0)
			game_res_err_msg(next_mus, debug_sw);
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

static int _snd_open(int hz)
{
	int chunk;
	if (!hz)
		hz = audio_rate;
	else
		audio_rate = hz;
	chunk = (chunksize_sw>0)?chunksize_sw:DEFAULT_CHUNKSIZE;
	audio_buffers = (audio_rate / 11025) * chunk;
	if (audio_buffers <= 0) /* wrong parameter? */
		audio_buffers = DEFAULT_CHUNKSIZE;
#ifdef __EMSCRIPTEN__
	if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096, NULL, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)) {
#else
	if (Mix_OpenAudio(hz, audio_format, audio_channels, audio_buffers)) {
#endif
		fprintf(stderr, "Unable to open audio!\n");
		return -1;
	}
	sound_on = 1;
	Mix_ChannelFinished(game_channel_finished);
	return 0;
}

#ifdef SAILFISHOS
static audioresource_t *audio_resource = NULL;

static void on_audio_resource_acquired(audioresource_t *ar, bool acquired, void *phz)
{
	if (acquired && !sound_on)
		_snd_open(*(int *)phz);
}

int snd_open(int hz)
{
	if (nosound_sw)
		return -1;
	if (sound_on)
		snd_close(); /* reopen */
	if (!audio_resource) {
		audio_resource = audioresource_init(AUDIO_RESOURCE_GAME,
	            on_audio_resource_acquired, &hz);
		if (!audio_resource)
			return -1;
		audioresource_acquire(audio_resource);
	}
	while (!sound_on) {
		fprintf(stderr, "Waiting for audio resource to be acquired...\n");
		g_main_context_iteration(NULL, true);
	}
	return 0;
}
#else
int snd_open(int hz)
{
	if (nosound_sw)
		return -1;
	if (sound_on)
		snd_close(); /* reopen */
	return _snd_open(hz);
}
#endif

int snd_init(int hz)
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Unable to init audio!\n");
		return -1;
	}
	return snd_open(hz);
}

int snd_volume_mus(int vol)
{
	if (!sound_on)
		return 0;
	Mix_Volume(-1, vol);
	return Mix_VolumeMusic(vol);
}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SND_DEFAULT_FORMAT  AUDIO_S16LSB
#else
#define SND_DEFAULT_FORMAT  AUDIO_S16MSB
#endif

#define MIXER_VERSION_ATLEAST(a,b,c)  (SDL_VERSIONNUM(SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION,SDL_MIXER_PATCHLEVEL) >= SDL_VERSIONNUM(a, b, c))

wav_t	snd_load_mem(int fmt, const short *data, size_t len)
{
	int freq = 22050, ffreq;
	SDL_AudioCVT wavecvt;
	Mix_Chunk *chunk;
	size_t size = len * sizeof(short);

	freq = snd_hz();

	if (fmt & SND_FMT_11)
		ffreq = 11025;
	else if (fmt & SND_FMT_22)
		ffreq = 22050;
	else
		ffreq = 44100;

	if (audio_format != SND_DEFAULT_FORMAT ||
	    audio_channels != ((fmt & SND_FMT_STEREO) ? 2:1) ||
	    ffreq != freq) {
		if (SDL_BuildAudioCVT(&wavecvt,
			      SND_DEFAULT_FORMAT, (fmt & SND_FMT_STEREO) ? 2:1, ffreq,
			      audio_format, audio_channels, freq) < 0)
			return NULL;

		wavecvt.len = size;
		wavecvt.buf = (Uint8 *)SDL_calloc(1, wavecvt.len * wavecvt.len_mult);

		if (!wavecvt.buf)
			return NULL;

		SDL_memcpy(wavecvt.buf, data, size);

		if (SDL_ConvertAudio(&wavecvt) < 0) {
			SDL_free(wavecvt.buf);
			return NULL;
		}
		chunk = Mix_QuickLoad_RAW(wavecvt.buf, wavecvt.len_cvt);
	} else {
		Uint8 *b = (Uint8 *)SDL_calloc(1, size);
		if (!b)
			return NULL;
		SDL_memcpy(b, data, size);
		chunk = Mix_QuickLoad_RAW(b, size);
	}
	if (!chunk)
		return NULL;
	chunk->allocated = 1;
	return (wav_t)chunk;
}

wav_t	snd_load_wav(const char *fname)
{
	SDL_RWops *rw;
	wav_t r;
	if (!sound_on)
		return NULL;
	if (!fname || !*fname)
		return NULL;
	rw = RWFromIdf(instead_idf(), fname);
	if (!rw || !(r = (wav_t)Mix_LoadWAV_RW(rw, 1))) {
		return NULL;
	}
	return r;
}

void	snd_free_wav(wav_t w)
{
	if (!w)
		return;
/*	Mix_HaltChannel(-1); */
	Mix_FreeChunk((Mix_Chunk*)w);
}

static int mix_fn = 0;

void snd_halt_chan(int han, int ms)
{
	if (han >= MIX_CHANNELS)
		han %= MIX_CHANNELS;
	if (han == -1 && mix_fn) /* forever wait */
		return;
	if (ms)
		Mix_FadeOutChannel(han, ms);
	else {
		Mix_HaltChannel(han);
	}
}

mus_t snd_load_mus(const char *fname)
{
	mus_t	mus = NULL;
	if (!sound_on)
		return NULL;
	mus = malloc(sizeof(struct _mus_t));
	if (!mus)
		return NULL;
	mus->rw = RWFromIdf(instead_idf(), fname);
	if (!mus->rw)
		goto err;
#if MIXER_VERSION_ATLEAST(2,0,0)
	mus->mus = Mix_LoadMUS_RW(mus->rw, SDL_FALSE);
#else
	mus->mus = Mix_LoadMUS_RW(mus->rw);
#endif
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
	if (!mus)
		return -1;
	if (loop >= 0)
		Mix_HookMusicFinished(game_music_finished);
	else
		Mix_HookMusicFinished(NULL);
	if (ms)
		Mix_FadeInMusic(mus->mus, loop, ms);
	else
		Mix_PlayMusic(mus->mus, loop);
	snd_volume_mus(snd_volume_mus(-1)); /* SDL hack? */
	return 0;
}

void snd_stop_mus(int ms)
{
	if (!sound_on)
		return;
	if (mix_fn)
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
	snd_volume_mus(snd_volume_mus(-1)); /* SDL hack? */
	return Mix_PlayChannel(channel, (Mix_Chunk*)chunk, loop);
}

void snd_mus_callback(void (*fn)(void *udata, unsigned char *stream, int len), void *arg)
{
	mix_fn = !!fn;
	Mix_HookMusic(fn, arg);
}

void snd_close(void)
{
	if (!sound_on)
		return;
	Mix_HookMusic(NULL, NULL);
	Mix_ChannelFinished(NULL);
	if (timer_id) {
		SDL_RemoveTimer(timer_id);
		timer_id = NULL_TIMER;
	}
	Mix_HaltChannel(-1);
	Mix_HookMusicFinished(NULL);
	Mix_HaltMusic();
	if (mus)
		snd_free_mus(mus);
	mus = NULL;
	if (next_mus)
		free(next_mus);
	next_mus = NULL;
#ifndef __EMSCRIPTEN__
	Mix_CloseAudio();
#endif
	sound_on = 0;
#ifdef SAILFISHOS
	audioresource_release(audio_resource);
	audioresource_free(audio_resource);
	audio_resource = NULL;
#endif
}

void snd_done(void)
{
	if (sound_on)
		snd_close();
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
