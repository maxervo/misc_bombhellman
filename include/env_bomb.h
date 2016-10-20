//NEW

#ifndef ENV_BOMB_H_
#define ENV_BOMB_H_


#include <map.h>
#include <player.h>
#include <env_monster.h>

#include <constant.h>

#define BOMB_MAX_LIFESPAN 4000
#define BOMB_FIRE_MAX_LIFESPAN 2000

struct bomb;

struct bomb* env_bomb_new();                                                     //for design e.i useful cuz in map.c need a env_bomb and struct is opaque there (pb of sizeof), be careful with opaque structure, use functions to retrieve objects...etc
void env_bomb_init(struct bomb* env_bomb);
void slot_bomb_reset(struct bomb* bomb);
void env_bomb_free(struct bomb* env_bomb);
int env_bomb_add(int x, int y, int range, struct map* map);
void env_bomb_refresh(struct map* map, struct player* player);
void env_fire_spread(int x_bomb, int y_bomb, int range_bomb, struct map* map, struct player *player);
int env_fire_create(int x, int y, struct map* map, struct player *player);
void env_fire_clean(int x_bomb, int y_bomb, int range_bomb, struct map* map, struct player *player);
int env_fire_extinguish(int x, int y, struct map* map, struct player *player);
int player_get_life(struct player* player);
void env_case_transform(struct map* map, int x, int y);
void env_bomb_affect_time(struct map* map, int modifier);

#endif
