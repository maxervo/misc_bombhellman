#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <tool.h>
#include <misc.h>


//Tool : Game Files Management (Cache, Saving & Loading Tool)

void cache_roaming_init(char* source)
{
	char world_source[50]; sprintf(world_source, "%s/world", source);
	char env_monster_source[50]; sprintf(env_monster_source, "%s/env_monster", source);

	struct stat buf;
	if( stat("save/roaming", &buf) == -1) {
		mkdir("save/roaming", 0700);
		mkdir("save/roaming/world", 0700);
		mkdir("save/roaming/env_monster", 0700);
	}
	else {
		printf("Roaming cache already present, we shall continue.\n");		//no problem to reuse cache cuz file_copy : fopen w option so overwrites ok, write later : recursive delete using opendir/readdir closedir or nftw, then unlink then rmdir
	}
	tool_save_player(source, "save/roaming");
	tool_save_world(world_source, "save/roaming/world");			//OK even if overwrites
	tool_save_env_monster(env_monster_source, "save/roaming/env_monster");
}

void tool_save_player(char* source_directory, char *target_directory)
{
	char src_filepath[50];
	char dst_filepath[50];

	sprintf(src_filepath, "%s/player", source_directory);
	sprintf(dst_filepath, "%s/player", target_directory);

	copy_file(src_filepath, dst_filepath);
}

void tool_save_world(char *source_directory, char *target_directory) 	//save all the maps
{
	char src_filepath[50];
	char dst_filepath[50];

	//Current id : map id where player starts
	sprintf(src_filepath, "%s/current_id", source_directory);
	sprintf(dst_filepath, "%s/current_id", target_directory);
	copy_file(src_filepath, dst_filepath);

	//All maps
	for(int i = 0; i < 8; i++)
	{
		sprintf(src_filepath, "%s/map_%d", source_directory, i);
		sprintf(dst_filepath, "%s/map_%d", target_directory, i);			//overwrites entirely target_filepath each time
		copy_file(src_filepath, dst_filepath);
	}
}

void tool_save_env_monster(char *source_directory, char *target_directory) 	//save all the maps
{
	char src_filepath[50];
	char dst_filepath[50];

	//All
	for(int i = 0; i < 8; i++)
	{
		sprintf(src_filepath, "%s/env_monster_%d", source_directory, i);
		sprintf(dst_filepath, "%s/env_monster_%d", target_directory, i);			//overwrites entirely target_filepath each time
		copy_file(src_filepath, dst_filepath);
	}
}

void tool_save_game(struct player* player, struct map* map, char* profile)
{
	char save_path[50]; sprintf(save_path, "save/%s", profile);
	char world_path[50]; sprintf(world_path, "%s/world", save_path);
	char env_monster_path[50]; sprintf(env_monster_path, "%s/env_monster", save_path);
	struct stat buf;

	//Roaming Buffer
	player_save_current(player);
	map_save_current(map);
	env_monster_save_current(map);

	//Preparing save location : profile directory
	if( stat(save_path, &buf) == -1) {							//make this security at the menu level, ask if overwrite...etc
		mkdir(save_path, 0700);
		mkdir(world_path, 0700);
		mkdir(env_monster_path, 0700);
	}
	else {
		printf("Profile already present, we shall continue.\n");		//No problem to reuse cache cuz file_copy : fopen w option so overwrites ok  //To think of later: write maybe a procedure: recursive delete using opendir/readdir closedir or nftw, then unlink then rmdir
	}																														//Possible to upgrade later: when SDL menu, more choices, more profiles...etc possible to choose profile, ask permission before overwriting...etc

	//Transfering All : from roaming buffer to profile directory
	tool_save_player("save/roaming", save_path);
	tool_save_world("save/roaming/world", world_path);
	tool_save_env_monster("save/roaming/env_monster", env_monster_path);
}

void tool_load_savegame(char* profile, struct player* player, struct map* map)
{
	char save_source[50]; sprintf(save_source, "save/%s", profile);

	int map_current_id;
	FILE* current_id_file = NULL;
	char path_to_map[50];
	char env_monster_path[50];

	//Transfering All : From Profile to Roaming Buffer			(Except when loading from "roaming" in game_new(), all roaming stays idle )
	if(strcmp(profile, "roaming") != 0) {
		cache_roaming_init(save_source);
	}

	//Map Current id (= Map Level where the player was)
	current_id_file = fopen("save/roaming/world/current_id", "r");
	map_current_id = fgetc(current_id_file) - '0';		//Need to convert : ASCII to id number
	fclose(current_id_file);

	//Player profile
	player_playerdata_load("save/roaming/player", player);

	//Map
	sprintf( path_to_map, "save/roaming/world/map_%d", map_current_id );
	mapdata_load(path_to_map, map);																				//Updates map id as well so OK

	//Monster Environment
	sprintf(env_monster_path, "save/roaming/env_monster/env_monster_%d", map_current_id);
	env_monster_load(env_monster_path, map);

	//Bomb Environment
	//No need, this is the behavior of the game

}
