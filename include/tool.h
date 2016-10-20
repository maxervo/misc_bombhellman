#ifndef TOOLS_H
#define TOOLS_H

#include <map.h>
#include <player.h>
#include <env_monster.h>
#include <constant.h>

//Tool : Game Files Management (Cache, Saving & Loading Tool)
void tool_save_world(char *source_directory, char *target_directory);
void cache_roaming_init(char* source);
void tool_save_game(struct player* player, struct map* map, char* profile);
void tool_load_savegame(char* profile, struct player* player, struct map* map);
void tool_save_player(char* source_directory, char *target_directory);
void tool_save_env_monster(char *source_directory, char *target_directory);

#endif
