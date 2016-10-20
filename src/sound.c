#include <sound.h>
#include <stdio.h>
#include <assert.h>

#define MUSIC_AMBIENT_1 "sound/cataclysm_ambient_theme.wav"
#define MUSIC_DANGER "sound/danger.wav"

#define EFFECT_BANG "sound/bang.wav"
#define EFFECT_LOCKED "sound/door_close.wav"
#define EFFECT_UNLOCKED "sound/door_open.wav"
#define EFFECT_BOMB_EXPLOSION "sound/bomb_explosion.wav"

#define EVIL_LAUGH "sound/evil_laugh.wav"
#define CONGRATS "sound/congrats.wav"

Mix_Music *current_music = NULL;
Mix_Music *music_ambient = NULL;
Mix_Music *music_danger = NULL;
Mix_Chunk *effect_bang = NULL;
Mix_Chunk *effect_locked = NULL;
Mix_Chunk *effect_unlocked = NULL;              //Possible to add even more sounds: danger_heavy_breathing, player_footstep, monster shout, fire_explosion, effect_burnt_touched_by_fire, fire_loop_burning, fire_extinguised, teleport_sound, pushing_case_sound, monster_footstep
Mix_Chunk *effect_bomb_explosion = NULL;
Mix_Chunk *evil_laugh = NULL;
Mix_Chunk *congrats = NULL;

void sound_load()
{
  music_ambient = Mix_LoadMUS(MUSIC_AMBIENT_1);
  music_danger = Mix_LoadMUS(MUSIC_DANGER);
  effect_bang = Mix_LoadWAV(EFFECT_BANG);
  effect_locked = Mix_LoadWAV(EFFECT_LOCKED);
  effect_unlocked = Mix_LoadWAV(EFFECT_UNLOCKED);
  effect_bomb_explosion = Mix_LoadWAV(EFFECT_BOMB_EXPLOSION);
  evil_laugh = Mix_LoadWAV(EVIL_LAUGH);
  congrats = Mix_LoadWAV(CONGRATS);
}

void sound_unload()
{
  Mix_FreeMusic(music_ambient);
  Mix_FreeMusic(music_danger);
  Mix_FreeChunk(effect_bang);
  Mix_FreeChunk(effect_locked);
  Mix_FreeChunk(effect_unlocked);
  Mix_FreeChunk(effect_bomb_explosion);
  Mix_FreeChunk(evil_laugh);
  Mix_FreeChunk(congrats);
}

void mixer_init()
{
  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
  sound_load();
}

void mixer_free()
{
    sound_unload();
    Mix_CloseAudio();
}

void sound_play_music(Mix_Music* new_music)     //Play music only if different music
{
  if(current_music != new_music) {
    Mix_PlayMusic(new_music, -1);
    current_music = new_music;
  }
}

void sound_play_effect(Mix_Chunk* new_effect)
{
  Mix_PlayChannel(-1, new_effect, 0);
}

Mix_Music* sound_get_music_ambient()
{
  assert(music_ambient);
  return music_ambient;
}

Mix_Music* sound_get_music_danger()
{
  assert(music_danger);
  return music_danger;
}

Mix_Chunk* sound_get_effect_bang()
{
  assert(effect_bang);
  return effect_bang;
}

Mix_Chunk* sound_get_effect_locked()
{
  assert(effect_locked);
  return effect_locked;
}

Mix_Chunk* sound_get_effect_unlocked()
{
  assert(effect_unlocked);
  return effect_unlocked;
}

Mix_Chunk* sound_get_effect_bomb_explosion()
{
  assert(effect_bomb_explosion);
  return effect_bomb_explosion;
}

Mix_Chunk* sound_get_evil_laugh()
{
  assert(evil_laugh);
  return evil_laugh;
}

Mix_Chunk* sound_get_congrats()
{
  assert(congrats);
  return congrats;
}
