#include <alsa/asoundlib.h>
#include <math.h>

class WavePlayer {

public:

    int SAMPLE_RATE = 48000;

    short *sine_wave(short *buffer, size_t sample_count, int freq) {
        for (int i = 0; i < sample_count; i++) {
            buffer[i] = 10000 * sinf(2 * M_PI * freq * ((float)i / SAMPLE_RATE));
        }
        return buffer;
    }


    WavePlayer() {

        snd_pcm_t* pcm;
        snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
        snd_pcm_hw_params_t *hw_params;
        snd_pcm_hw_params_alloca(&hw_params);

        snd_pcm_hw_params_any(pcm, hw_params);
        snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(pcm, hw_params, 1);
        snd_pcm_hw_params_set_rate(pcm, hw_params, 48000, 0);
        snd_pcm_hw_params_set_periods(pcm, hw_params, 10, 0);
        snd_pcm_hw_params_set_period_time(pcm, hw_params, 100000, 0); // 0.1 seconds

        snd_pcm_hw_params(pcm, hw_params);

        short buffer[48000];
        sine_wave(buffer, 48000, 440);

        snd_pcm_writei(pcm, buffer, 48000);

        snd_pcm_drain(pcm);
        snd_pcm_close(pcm);


    }



};