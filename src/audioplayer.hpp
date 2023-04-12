#include <alsa/asoundlib.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>


#include "waveheader.hpp"

class AudioPlayer {

private:

    WaveHeader header = {};
    uint8_t* data = nullptr;
    std::filesystem::path audio_path;
    std::string int_to_string(const uint32_t i) const;
    void delete_data();

    std::thread play_thread;

    bool playing = false;

public:

    ~AudioPlayer();

    // Reads a file, if it is a well-formed wave file parse its header
    // Returns: `true` if the file is a well-formed wave file,
    //          `false otherwise
    // If `false` was returned, all other functions have undefined behavior
    bool read(const std::filesystem::path& filename);

    WaveHeader get_header();
    // Copies the audio data into `buf`
    // User must ensure `buf` is sufficiently long
    void get_data(uint8_t* buf);

    // Returns: formatted wave file header 
    std::string header_string() const;
    // Print the formatted wave file header
    void print_header() const;

    void save_header(std::filesystem::path header_path = std::filesystem::path()) const;

    int SAMPLE_RATE = 48000;

    AudioPlayer() {

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
        std::cout << "here\n";

        snd_pcm_writei(pcm, buffer, 48000);
        std::cout << "here\n";
        snd_pcm_drain(pcm);
        std::cout << "here\n";
        snd_pcm_close(pcm);
        std::cout << "here\n";


    }

    bool play(double timestamp = 0.0, double speed = 1.0, bool blocking = true) {
        if (!blocking) {
            play(timestamp, speed, true);
        }
        assert(timestamp >= 0);

    }

    void pause() {

    }



};