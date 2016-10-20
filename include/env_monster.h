#ifndef ENV_MONSTER_H_
#define ENV_MONSTER_H_

#include <map.h>
#include <player.h>

#include <constant.h>

struct monster;

struct monster* env_monster_new();
void env_monster_init(struct monster* env_monster);
void slot_monster_reset(struct monster* monster);
void env_monster_free(struct monster* env_monster);
void env_monster_add(int x, int y, enum direction current_direction, int life, int range, struct map* map);
void env_monster_refresh(struct map* map, struct player* player);
void monster_move_rand(struct monster* monster, struct map* map,  struct player* player);
int monster_move_aux(struct monster* monster, struct map* map, int x, int y,  struct player* player);
void monster_affect_life(struct monster* monster, int modifier);
struct monster* monster_locate(int x, int y, struct map* map);
void monster_handle_fire(int x, int y, struct map* map);
int monster_mouvement_inhibition(struct monster* monster);
void monster_handle_extinguish(int x, int y, struct map* map);
void env_monster_save_current(struct map* map);
void env_monster_load(char *env_monster_path, struct map* map);

#endif
