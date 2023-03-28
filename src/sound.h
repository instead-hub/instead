#ifndef __SOUND_H__
#define __SOUND_H__

typedef void*  wav_t;
typedef struct _mus_t *mus_t;

#define DEFAULT_CHUNKSIZE 1024

/* extern mus_t	snd_load_mus(const char *path); */
extern void	snd_free_mus(mus_t mus);
extern int	snd_open(int hz);
extern int	snd_init(int hz);
extern int	snd_hz(void);
extern int	snd_play(wav_t chunk, int channel, int loop,int fading);
extern void 	snd_halt_chan(int han, int ms);

extern void	snd_free_wav(wav_t chunk);
extern wav_t	snd_load_wav(const char *fname);

#define SND_FMT_STEREO	1
#define SND_FMT_44	2
#define SND_FMT_22	4
#define SND_FMT_11	8

extern wav_t	snd_load_mem(int fmt, const short *buf, size_t len);
extern int	snd_play_mus(char *music, int ms, int loop);
extern void	snd_mus_callback(void (*fn)(void *udata, unsigned char *stream, int len), void *arg);
extern int	snd_playing_mus();
extern void     snd_stop_mus(int ms);
extern int 	snd_volume_mus(int vol);
extern void	snd_done(void);
extern void	snd_close(void);
extern int 	snd_vol_from_pcn(int v);
extern int 	snd_vol_to_pcn(int v);
extern int 	snd_playing(int channel);
extern void	snd_pause(int on);
extern int	snd_panning(int channel, int left, int right);
extern int	snd_enabled(void);

#endif
