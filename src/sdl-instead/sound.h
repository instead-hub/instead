#ifndef __SOUND_H__
#define __SOUND_H__

#define SND_CLICK	0
#define SND_BONUS	1
#define SND_HISCORE	2
#define SND_GAMEOVER	3
#define SND_BOOM 	4
#define SND_FADEOUT	5
#define SND_PAINT	6

typedef void*  wav_t;
typedef void*  mus_t;

//extern mus_t	snd_load_mus(const char *path);
extern void	snd_free_mus(mus_t mus);
extern int	snd_init(int hz);
extern int	snd_hz(void);
extern int	snd_play(wav_t chunk, int channel, int loop);
extern void 	snd_halt_chan(int han, int ms);

extern void	snd_free_wav(wav_t chunk);
extern wav_t	snd_load_wav(const char *fname);
extern int      snd_play_mus(char *music, int ms, int loop);
extern int	snd_playing_mus();
extern void     snd_stop_mus(int ms);
extern int 	snd_volume_mus(int vol);
extern	void	snd_done(void);
extern int 	snd_vol_from_pcn(int v);
extern int 	snd_vol_to_pcn(int v);
extern int 	snd_playing(int channel);

#endif
