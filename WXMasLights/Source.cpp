#include <iostream>
#include <mpg123.h>
#include <SDL2/SDL.h>

int main() {
    if (mpg123_init() != MPG123_OK) {
        std::cerr << "mpg123 init failed!" << std::endl;
        return -1;
    }

    mpg123_handle* mh = mpg123_new(nullptr, nullptr);
    if (mh == nullptr) {
        std::cerr << "Failed to create mpg123 handle!" << std::endl;
        mpg123_exit();
        return -1;
    }

    // Open MP3 file
    if (mpg123_open(mh, "music.mp3") != MPG123_OK) {
        std::cerr << "Error opening file!" << std::endl;
        mpg123_delete(mh);
        mpg123_exit();
        return -1;
    }

    // Get audio format info
    long rate;
    int channels, encoding;
    mpg123_getformat(mh, &rate, &channels, &encoding);

    // Initialize SDL audio
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec wanted_spec, spec;
    wanted_spec.freq = rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = channels;
    wanted_spec.samples = 4096;
    wanted_spec.callback = nullptr;

    if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
        std::cerr << "SDL_OpenAudio failed!" << std::endl;
        mpg123_delete(mh);
        mpg123_exit();
        return -1;
    }

    SDL_PauseAudio(0); // Start audio playback

    unsigned char* audio_buffer;
    size_t buffer_size;
    int err;
    while ((err = mpg123_read(mh, audio_buffer, buffer_size, &buffer_size)) == MPG123_OK) {
        SDL_QueueAudio(1, audio_buffer, buffer_size);
    }

    SDL_CloseAudio();
    mpg123_delete(mh);
    mpg123_exit();

    return 0;
}
