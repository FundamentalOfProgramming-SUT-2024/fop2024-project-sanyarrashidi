// #include "music.h"


// int init_audio() {
//     if (SDL_Init(SDL_INIT_AUDIO) < 0) {
//         printf("Failed to initialize SDL: %s\n", SDL_GetError());
//         return -1;
//     }

//     if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
//         printf("Failed to initialize SDL_mixer: %s\n", Mix_GetError());
//         SDL_Quit();
//         return -1;
//     }

//     return 0;
// }

// Mix_Music* load_music(const char* file_path) {
//     Mix_Music* music = Mix_LoadMUS(file_path);
//     if (!music) {
//         printf("Failed to load music: %s\n", Mix_GetError());
//     }
//     return music;
// }

// void cleanup(Mix_Music* music) {
//     if (music) {
//         Mix_FreeMusic(music);
//     }
//     Mix_CloseAudio();
//     SDL_Quit();
// }
