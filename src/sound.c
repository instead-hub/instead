/*
 * Copyright 2009-2026 Peter Kosyh <pkosyh at yandex.ru>
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

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>


int audio_rate = 44100;

#define SND_DEFAULT_FORMAT SDL_AUDIO_S16
#define MIX_CHANNELS SND_CHANNELS
#define MUS_CHANNEL MIX_CHANNELS
#define MUS_CHAN channels[MUS_CHANNEL]

Uint16 audio_format = SND_DEFAULT_FORMAT;

int audio_channels = 2;
int audio_buffers = 8192;

static mus_t mus;
static char *next_mus = NULL;
static int   next_fadein = 0;
static int   next_loop = -1;
static SDL_TimerID timer_id = 0;
static MIX_Mixer *mixer = NULL;

struct {
	int id;
	MIX_Track *track;
	SDL_PropertiesID props;
} channels[MIX_CHANNELS+1] = { }; /* last is music channel */

static int sound_on = 0;

struct _mus_t {
	MIX_Audio *mus;
	SDL_IOStream *rw;
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
	timer_id = 0;
}

static Uint32 callback(void *aux, SDL_TimerID timerID, Uint32 interval)
{
	push_user_event(mus_callback,  aux);
	return interval;
}

int snd_hz(void)
{
	SDL_AudioSpec aspec;
	if (sound_on && MIX_GetMixerFormat(mixer, &aspec))
		return aspec.freq;
	return 0;
}

int nosound_sw = 0;
void snd_pause(int on)
{
	if (!sound_on)
		return;
	if (on)
		MIX_PauseAllTracks(mixer);
	else
		MIX_ResumeAllTracks(mixer);
	return;
}

extern void game_music_finished(void);

static void track_cb(void *userdata, MIX_Track *track)
{
	int id = *(int*)userdata;
	if (id == MUS_CHANNEL)
		return game_music_finished();
	game_channel_finished(id);
}

static int _snd_open(int hz)
{
	struct SDL_AudioSpec aspec;
	int chunk, i;
	if (!hz)
		hz = audio_rate;
	else
		audio_rate = hz;
	chunk = (chunksize_sw>0)?chunksize_sw:DEFAULT_CHUNKSIZE;
	audio_buffers = (audio_rate / 11025) * chunk;
	if (audio_buffers <= 0) /* wrong parameter? */
		audio_buffers = DEFAULT_CHUNKSIZE;
	aspec.channels = audio_channels;
	aspec.freq = hz;
	aspec.format = SND_DEFAULT_FORMAT;
	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &aspec);
	if (!mixer) {
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
		return -1;
	}
	for (i = 0; i <= MIX_CHANNELS; i++) {
		channels[i].id = i;
		channels[i].track = MIX_CreateTrack(mixer);
		if (!channels[i].track) {
			fprintf(stderr, "Unable to create audio track: %s\n", SDL_GetError());
			return -1;
		}
		channels[i].props = SDL_CreateProperties();
		if (i != MUS_CHANNEL)
			MIX_SetTrackStoppedCallback(channels[i].track, track_cb, &channels[i].id);
	}
	sound_on = 1;
	return 0;
}

int snd_open(int hz)
{
	if (nosound_sw)
		return -1;
	if (sound_on)
		snd_close(); /* reopen */
	if (_snd_open(hz)) {
		snd_close();
		return -1;
	}
	return 0;
}

int snd_init(int hz)
{
	if (!SDL_InitSubSystem(SDL_INIT_AUDIO) || !MIX_Init()) {
		fprintf(stderr, "Unable to init audio: %s\n", SDL_GetError());
		return -1;
	}
	return snd_open(hz);
}

int snd_volume_mus(int vol)
{
	if (!sound_on)
		return 0;
	if (vol < 0)
		return (int)(MIX_GetMixerGain(mixer)*128);
	MIX_SetMixerGain(mixer, ((float)vol)/128.0f);
	return vol;
}

wav_t	snd_load_mem(int fmt, const short *data, size_t len)
{
	SDL_AudioSpec sspec, dspec;
	int freq = 22050, ffreq;
	MIX_Audio *chunk;
	size_t size = len * sizeof(short);

	freq = snd_hz();

	if (fmt & SND_FMT_11)
		ffreq = 11025;
	else if (fmt & SND_FMT_22)
		ffreq = 22050;
	else
		ffreq = 44100;

	dspec.freq = freq;
	dspec.channels = audio_channels;
	dspec.format = audio_format;

	if (audio_format != SDL_AUDIO_S16 ||
	    audio_channels != ((fmt & SND_FMT_STEREO) ? 2:1) || ffreq != freq) {
		Uint8 *dst; int dst_len;
		sspec.freq = ffreq;
		sspec.channels = (fmt & SND_FMT_STEREO) ? 2:1;
		sspec.format = SDL_AUDIO_S16;
		if (!SDL_ConvertAudioSamples(&sspec, (Uint8 *)data, size, &dspec, &dst, &dst_len))
			return NULL;
		chunk = MIX_LoadRawAudioNoCopy(mixer, dst, dst_len, &dspec, true);
	} else {
		Uint8 *b = (Uint8 *)SDL_calloc(1, size);
		if (!b)
			return NULL;
		SDL_memcpy(b, data, size);
		chunk = MIX_LoadRawAudioNoCopy(mixer, b, size, &dspec, true);
	}
	if (!chunk)
		return NULL;
	return (wav_t)chunk;
}

wav_t	snd_load_wav(const char *fname)
{
	SDL_IOStream *rw;
	wav_t r;
	if (!sound_on)
		return NULL;
	if (!fname || !*fname)
		return NULL;
	rw = RWFromIdf(instead_idf(), fname);
	if (!rw || !(r = (wav_t)MIX_LoadAudio_IO(mixer, rw, true, true))) {
		return NULL;
	}
	return r;
}

void	snd_free_wav(wav_t w)
{
	if (!w)
		return;
	MIX_DestroyAudio((MIX_Audio*)w);
}

static int mix_fn = 0;

void snd_halt_chan(int han, int ms)
{
	int i;
	if (han < 0) { /* all channels */
		if (mix_fn) /* forever wait */
			return;
		for (i = 0; i < MIX_CHANNELS; i ++)
			MIX_StopTrack(channels[i].track, MIX_TrackMSToFrames(channels[i].track, ms));
		return;
	}
	han %= MIX_CHANNELS;
	MIX_StopTrack(channels[han].track, MIX_TrackMSToFrames(channels[han].track, ms));
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
	mus->mus = MIX_LoadAudio_IO(mixer, mus->rw, false, false);
	if (!mus->mus)
		goto err1;
	return mus;
err1:
	SDL_CloseIO(mus->rw);
err:
	free(mus);
	return NULL;
}


int snd_play_mus(char *fname, int ms, int loop)
{
	MIX_Track *track = MUS_CHAN.track;
	SDL_PropertiesID props = MUS_CHAN.props;
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
		MIX_SetTrackStoppedCallback(MUS_CHAN.track, track_cb, &MUS_CHAN.id);
	else
		MIX_SetTrackStoppedCallback(MUS_CHAN.track, NULL, NULL);
	MIX_SetTrackAudio(track, mus->mus);
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, ms);
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop);
	MIX_PlayTrack(track, props);
	return 0;
}

void snd_stop_mus(int ms)
{
	if (!sound_on)
		return;
	if (mix_fn)
		return;
	MIX_SetTrackStoppedCallback(MUS_CHAN.track, NULL, NULL);
	MIX_StopTrack(MUS_CHAN.track, MIX_TrackMSToFrames(MUS_CHAN.track, ms));
}

int snd_playing_mus(void)
{
	if (!sound_on)
		return 0;
	return MIX_TrackPlaying(MUS_CHAN.track);
}

int snd_playing(int channel)
{
	if (!sound_on)
		return 0;
	if (channel < 0) {
		int i;
		for (i = 0; i < MIX_CHANNELS; i++) {
			if (MIX_TrackPlaying(channels[i].track))
				return 1;
		}
		return 0;
	}
	channel %= MIX_CHANNELS;
	return MIX_TrackPlaying(channels[channel].track);
}

int snd_panning(int channel, int left, int right)
{
	int i;
	MIX_StereoGains gains;
	gains.left = (float)left / 255.0f;
	gains.right = (float)right / 255.0f;

	if (channel < 0) {
		for (i = 0; i < MIX_CHANNELS; i++)
			MIX_SetTrackStereo(channels[i].track, &gains);
		return 0;
	}
	channel %= MIX_CHANNELS;

	return MIX_SetTrackStereo(channels[channel].track, &gains);
}


void snd_free_mus(mus_t mus)
{
	if (!sound_on)
		return;
	if (!mus)
		return;
	MIX_StopTrack(MUS_CHAN.track, MIX_TrackMSToFrames(MUS_CHAN.track, 0));
	if (mus->mus) {
		MIX_DestroyAudio(mus->mus);
		SDL_CloseIO(mus->rw);
	}
	free(mus);
}

int snd_play(void *chunk, int channel, int loop)
{
	MIX_Track *track;
	SDL_PropertiesID props;
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

	if (channel == -1) {
		for (channel = 0; channel < MIX_CHANNELS; channel ++) {
			if (!snd_playing(channel))
				break;
		}
	}

	if (channel == MUS_CHANNEL)
		return -1;

	track = channels[channel].track;
	props = channels[channel].props;

	MIX_SetTrackAudio(track, (MIX_Audio*)chunk);
//	SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, ms);
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop);
	MIX_PlayTrack(track, props);
	return channel;
}

static struct {
	void *arg;
	void (*fn)(void *udata, float *stream, int len);
} raw_cb_ctx;

static void raw_cb(void *userdata, MIX_Mixer *mixer, const SDL_AudioSpec *spec, float *pcm, int samples)
{
	if (raw_cb_ctx.fn)
		raw_cb_ctx.fn(raw_cb_ctx.arg, pcm, samples);
}

void snd_mus_callback(void (*fn)(void *udata, float *stream, int len), void *arg)
{
	mix_fn = !!fn;
	raw_cb_ctx.arg = arg;
	raw_cb_ctx.fn = fn;
	if (fn)
		MIX_SetPostMixCallback(mixer, raw_cb, arg);
	else
		MIX_SetPostMixCallback(mixer, NULL, NULL);
}

void snd_close(void)
{
	int i;
	if (!sound_on)
		return;

	snd_mus_callback(NULL, NULL);

	if (timer_id) {
		SDL_RemoveTimer(timer_id);
		timer_id = 0;
	}

	for (i = 0; i <= MIX_CHANNELS; i++) {
		MIX_SetTrackStoppedCallback(channels[i].track, NULL, NULL);
		MIX_StopTrack(channels[i].track, 0);
		MIX_DestroyTrack(channels[i].track);
		SDL_DestroyProperties(channels[i].props);
		channels[i].track = NULL;
		channels[i].props = 0;
	}
	if (mus)
		snd_free_mus(mus);
	mus = NULL;
	if (next_mus)
		free(next_mus);
	next_mus = NULL;

	MIX_DestroyMixer(mixer);
	mixer = NULL;
	sound_on = 0;
}

void snd_done(void)
{
	if (sound_on)
		snd_close();
	MIX_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int snd_vol_to_pcn(int v)
{
	return (v * 100) / 128;
}
