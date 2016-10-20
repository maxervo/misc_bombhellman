#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL_mixer.h>    //To think later: design of includes in .h or .c...etc

void sound_load();
void sound_unload();
void mixer_init();
void mixer_free();

void sound_play_music(Mix_Music* new_music);
void sound_play_effect(Mix_Chunk* new_effect);
Mix_Music* sound_get_music_ambient();
Mix_Chunk* sound_get_effect_bang();
Mix_Music* sound_get_music_danger();
Mix_Chunk* sound_get_effect_locked();
Mix_Chunk* sound_get_effect_unlocked();
Mix_Chunk* sound_get_effect_bomb_explosion();
Mix_Chunk* sound_get_evil_laugh();
Mix_Chunk* sound_get_congrats();

#endif
