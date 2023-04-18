#include <alsa/asoundlib.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>


#include "waveheader.hpp"

#define part1
#define part2
//#define part3

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

    AudioPlayer() {




    }

#ifdef part2

    bool play(double timestamp = 0.0, double speed = 1.0, bool blocking = true) {
        if (!blocking) {
            play(timestamp, speed, true);
        }
        assert(timestamp >= 0);

        snd_pcm_t* pcm;

        int error;

        if (error = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)) {
            std::cerr << "error: snd_pcm_open - " << snd_strerror(error);
        }

        snd_pcm_hw_params_t *hw_params;
        snd_pcm_hw_params_alloca(&hw_params);
        if (error = snd_pcm_hw_params_any(pcm, hw_params)) {
            std::cerr << "error: snd_pcm_hw_params_any - " << snd_strerror(error);
        }
        if (error = snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) {
            std::cerr << "error: snd_pcm_hw_params_set_access - " << snd_strerror(error);
        }
        if (error = snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE)) {
            std::cerr << "error: snd_pcm_hw_params_set_format - " << snd_strerror(error);
        }
        if (error = snd_pcm_hw_params_set_channels(pcm, hw_params, header.format_chunk.NumChannels)) {
            std::cerr << "error: snd_pcm_hw_params_set_channels - " << snd_strerror(error);
        }
        if (error = snd_pcm_hw_params_set_rate(pcm, hw_params, header.format_chunk.SampleRate, 0)) {
            std::cerr << "error: snd_pcm_hw_params_set_rate - " << snd_strerror(error);
        }
        // if (error = snd_pcm_hw_params_set_periods(pcm, hw_params, 2, 0)) {
        //     std::cerr << "error: snd_pcm_hw_params_set_periods - " << snd_strerror(error);
        // }
        // if (error = snd_pcm_hw_params_set_period_time(pcm, hw_params, header.format_chunk.Size / 20, 0)) {
        //     std::cerr << "error: snd_pcm_hw_params_set_period_time - " << snd_strerror(error);
        // }
        if (error = snd_pcm_hw_params(pcm, hw_params)) {
            std::cerr << "error: snd_pcm_hw_params - " << snd_strerror(error);
        }
        // if (error = snd_pcm_prepare(pcm)) {
        //     std::cerr << "error: snd_pcm_prepare - " << snd_strerror(error);
        // }
        size_t chunk_size = 1024;
        int factor = 1;
        if (header.format_chunk.BitsPerSample == 16) {
            factor *= 2;
        }
        factor *= header.format_chunk.NumChannels;
        for (int i = 0; i < header.data_chunk.Size / factor; i += chunk_size) {
            if ((error = snd_pcm_writei(pcm, data + i * factor, chunk_size)) < 0) {
                std::cerr << "error: snd_pcm_writei - " << snd_strerror(error);
            } else {
                std::cout << "Frames written: " << error << '\n';
            }
        }
        // if (error = snd_pcm_drain(pcm)) {
        //     std::cerr << "error: snd_pcm_drain - " << snd_strerror(error);
        // }
        if (error = snd_pcm_close(pcm)) {
            std::cerr << "error: snd_pcm_close - " << snd_strerror(error);
        }
        return true;
    }

    void pause() {

    }

#endif

};