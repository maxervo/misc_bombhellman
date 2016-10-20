#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <assert.h>
#include <string.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <sound.h>
#include <misc.h>
#include <constant.h>

struct player {
	//Position
	int x, y;
	enum direction current_direction;
	//aura possible																	//Possible to upgrade later: aura implement as well, ...etc superposed, stas modifiers temp +/-

	//Status
	int life;
	enum state state;
	int date_last_state;
	int range;

	//Inventory
	int nb_bomb;
	int key;
};

struct player* player_new() {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = 0;

	player->life = 0;
	player->state = 0;
	player->date_last_state = 0;
	player->range = 0;

	player->nb_bomb = 0;
	player->key = 0;

	return player;
}

void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->current_direction = way;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bomb;
}

//Deprecated, no need thanks to tool_load_savegame()
void player_from_map(struct player* player, struct map* map) {
	assert(player);
	assert(map);

	int i, j;
	for (i = 0; i < map_get_width(map); i++) {
	  for (j = 0; j < map_get_height(map); j++) {
	    if (map_get_cell_type(map, i, j) == CELL_PLAYER) {
	      player->x = i;
	      player->y = j;
	    }
	  }
	}
}

static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	if(player_mouvement_inhibition(player))			//Player mvt inhibition for instance for some states : paralyzed, burnt...etc			//possible to upgrade later: more states...etc
	{
		return 0;
	}

	switch (map_get_cell_type(map, x, y)) {

	case CELL_SCENERY:
		sound_play_effect(sound_get_effect_bang());
		return 0;
		break;

	case CELL_CASE:
		if(player_push(player, map, x, y)) {
			return 1;
		}
		else {
			return 0;
		}
		break;

	case CELL_KEY:
		player->key = 1;
		return 1;
		break;

	case CELL_DOOR:
		if(player->current_direction == NORTH) {
			door_handle(player, map, x, y);
		}
		else {
			sound_play_effect(sound_get_effect_bang());
		}
		return 0;
		break;

	case CELL_BONUS:
		player_get_bonus(player, map, x, y);
		return 1;
		break;

	case CELL_MONSTER:
		player_handle_monster(player);
		return 0;
		break;

	case CELL_PLAYER:
		break;

	case CELL_PRINCESS:
		player_affect_state(player, LOVE_PRINCESS);		// Has kissed princess and in love -> win, princess saved
		player_affect_date_last_state(player, SDL_GetTicks());
		break;

	case CELL_FIRE:
		player_affect_life(player, -1);
		return 0;
		break;

	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->current_direction) {
	case NORTH:
		if (player_move_aux(player, map, x, y - 1)) {
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:
		if (player_move_aux(player, map, x, y + 1)) {
			player->y++;
			move = 1;
		}
		break;

	case WEST:
		if (player_move_aux(player, map, x - 1, y)) {
			player->x--;
			move = 1;
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y)) {
			player->x++;
			move = 1;
		}
		break;
	}

	if (move) {
		//To think later: a bit redundant cuz player already coded on map and mainloop prints image, there was a bug of images replicating if not this action cuz otherwise : player moves virtually (x,y) but map node not erased from previous location //Think better design: Virtual System (for positions) + Map Node Placement System (based on virtual system, refresh map nodees) + Display System (based on map nodes)
		map_set_cell_type(map, x, y, CELL_EMPTY);
		map_set_cell_type(map, player->x, player->y, CELL_PLAYER);
	}
	return move;

}

void player_display(struct player* player) {
	assert(player);
		window_display_image(sprite_get_player(player->current_direction), player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}

//NEW

//If succeeds to push -> 1, else -> 0 ; target = the box we want to move for instance
int player_push(struct player* player, struct map* map, int x_target, int y_target) {
	int case_subtype = map_get_cell_SUBtype(map, x_target, y_target);			//It is on: target
	int x_neighbor = x_target; int y_neighbor = y_target;						//target = the object we want to move ; neighbor = surroundings of object

	switch(player->current_direction) {
		case NORTH:
			y_neighbor--;
			break;
		case SOUTH:
			y_neighbor++;
			break;
		case WEST:
			x_neighbor--;
			break;
		case EAST:
			x_neighbor++;
			break;
	}
	if(map_is_inside(map, x_neighbor, y_neighbor) && map_get_cell_type(map, x_neighbor, y_neighbor) == CELL_EMPTY) {	//OK lazy condition evaluation -> neighbor well defined assert for getCellType ok
		map_set_cell_type(map, x_neighbor, y_neighbor, CELL_CASE);						//Previous node = overwritten by CELL_PLAYER with funtion player_move()
		map_set_cell_SUBtype(map, x_neighbor, y_neighbor, case_subtype);
		return 1;	//push success
	}
	else {
		return 0; //push fail
	}
}

//Possible to upgrade later: design maybe cuz functions structure design given by assignement pdf not rigourous : get_cell_"type" -> only "primary type", whereas in map_display they use "type" for "complete type"
void door_handle(struct player *player, struct map* map, int x, int y)
{
	int door_subtype = map_get_cell_SUBtype(map, x, y);
	int door_open = door_subtype & 0x8; // (AND mask 1000)
	int door_id_enter = door_subtype & 0x7; // (AND mask 0111)

	if(door_open) {
		player_teleport(player, map, door_id_enter);
	}
	else if(player->key) {
		map_set_cell_SUBtype(map, x, y, (door_subtype ^ 0x8) );		// XOR (mask 1000)
		player->key = 0;
		sound_play_effect(sound_get_effect_unlocked());
	}
	else {
		sound_play_effect(sound_get_effect_locked());
	}

}

void player_teleport(struct player *player, struct map* map, int portal_id_enter)		// map targets coded on 3bits -> possibilities 0-7 ; int will never be more than 1 digit
{
	int portal_id_exit = map_get_id(map);			//Attention: portal_enter: id of world_target ; portal_exit: id of world_origin
	int x_portal_exit; int y_portal_exit;
	char path_to_map[50]; sprintf(path_to_map, "save/roaming/world/map_%d", portal_id_enter);
	char env_monster_path[50]; sprintf(env_monster_path, "save/roaming/env_monster/env_monster_%d", portal_id_enter);

	//Cleaning																				//Other possibility: implement map cleaning in env_bomb_init() (using extinguish() ok), and env_monster_init()
	env_bomb_init(map_get_env_bomb(map));							//For env_bomb : not saved OK, just clear it
	map_clean_env(map, player);

	//Saving all map/env before teleportation
	map_save_current(map);
	env_monster_save_current(map);

	//Loading all map/env during teleportation
	mapdata_load(path_to_map, map);
	env_monster_load(env_monster_path, map);					//OK no need to adjust date_last_action, they will only have a maximum of 1 move or action

	//Position player after teleportation
	map_seek_portal(portal_id_exit, map, &x_portal_exit, &y_portal_exit);			//We suppose: two portals linked; one has id of other world and vice versa
	player->x = x_portal_exit;																								//Player isn't placed on map, just his virtual position then when he moves he will be placed on map thanks to player_move()
	player->y = y_portal_exit+1;																							//Attention: Portals musn't be placed on lower limit of map otherwise spawnpoint outside
	player->current_direction = SOUTH;
}

int player_get_key(struct player* player)
{
	assert(player);
	return player->key;
}

void player_set_bomb(struct player* player, struct map* map)				//Control (nb_bomb, decrement...etc) done in player_set_bomb() cuz env_bomb_add() generic/general : can be used by other entites than player
{
	int x_bomb = player->x; int y_bomb = player->y;

	if( player->nb_bomb == 0) {
		printf("You don't have any bombs left\n");
	}
	else if( env_bomb_add(x_bomb,y_bomb,player->range, map) ) {
		player_affect_nb_bomb(player, -1);
	}
	else {
		printf("Max number of bombs placed\n");
	}
}

enum state player_get_state(struct player* player)
{
	assert(player);
	return player->state;
}

void player_affect_state(struct player* player, enum state new_state)
{
	assert(player);
	player->state = new_state;
}

int player_mouvement_inhibition(struct player* player)
{
	assert(player);
	enum state mvt_inhibitors[2] = { BURNT, PARALYZED };

	return presence_array(player->state, (int *) mvt_inhibitors, 2);
}

void player_affect_life(struct player* player, int modifier)
{
	assert(player);
	enum state state = player_get_state(player);
	int result = player->life + modifier;

	if(state == GODMODE) {
		printf("Badass godmode unstoppable (invulnerability)\n");
	}
	else if(result >= 0) {										//Security is done here; Always use this procedure to affect life of player, otherwise risks of negative life and crash (e.g display_banner...etc)
		player->life = result;
	}
	else {
		player->life = 0;
	}
}

void player_affect_nb_bomb(struct player* player, int modifier)
{
	assert(player);
	int result = player->nb_bomb + modifier;
	if(result >= 0) {										//Security is done here; Always use this procedure to affect nb_bombs of player, otherwise risks of negative life and crash (e.g display_banner...etc)
		player->nb_bomb = result;
	}
	else {
		player->nb_bomb = 0;
	}
}

void player_affect_range(struct player* player, int modifier)
{
	assert(player);
	int result = player->range + modifier;
	if(result >= 0) {										//Security is done here; Always use this procedure to affect range of player, otherwise risks of negative life and crash (e.g display_banner...etc)
		player->range = result;
	}
	else {
		player->range = 0;
	}
}

int player_get_range(struct player* player)
{
	assert(player);
	return player->range;
}

int player_get_life(struct player* player)
{
	assert(player);
	return player->life;
}

void player_get_bonus(struct player* player, struct map* map, int x, int y)
{
	switch(map_get_cell_SUBtype(map, x, y))
	{
    case BONUS_BOMB_RANGE_DEC:
			player_affect_range(player, -1);
		  break;
    case BONUS_BOMB_RANGE_INC:
			player_affect_range(player, 1);
			break;
    case BONUS_BOMB_NB_DEC:
			player_affect_nb_bomb(player, -1);		//To think later: quick additional security here or not? if(player_get_nb_bomb(player) != 1) {player_affect_nb_bomb(player, -1);} otherwise player won't be able to have bombs during rest of the play (cuz won't be able to break boxes) //upgrade later on maybe to be included in gamemplay
			break;
    case BONUS_BOMB_NB_INC:
			player_affect_nb_bomb(player, 1);
			break;
    case BONUS_LIFE:
      player_affect_life(player, 1);
      break;
	}
}

void player_refresh(struct player* player, struct map* map)
{
	player_update_state(player);
}

void player_update_state(struct player* player)
{
	int state = player_get_state(player);
	int state_latency = SDL_GetTicks() - player_get_date_last_state(player);
	int max_latency;

	switch(state)									//to upgrade later: add more states
	{
		case GODMODE:
			max_latency = 4000;
			break;
		default:
			max_latency = MAX_STATE_LATENCY_DEFAULT;
			break;
	}

	if(state_latency >= max_latency) {
		player_affect_state(player, NORMAL);
		player_affect_date_last_state(player, SDL_GetTicks());	//To think later: optional?
	}

}

int player_get_date_last_state(struct player* player)
{
	assert(player);
	return player->date_last_state;
}

void player_affect_date_last_state(struct player* player, int new_date)
{
	assert(player);
	player->date_last_state = new_date;
}

void player_handle_monster(struct player* player)
{
	player_affect_life(player, -1);									//OK security of GODMODE inside player_affect_life()
	if(player_get_state(player) != GODMODE)
	{
		player_affect_state(player, GODMODE);
		player_affect_date_last_state(player, SDL_GetTicks());
	}
}

void player_save_current(struct player* player)
{
	FILE* player_file = fopen("save/roaming/player", "w");	//OK overwrites
	fprintf(player_file, "x=%d\n", player->x);
	fprintf(player_file, "y=%d\n", player->y);
	fprintf(player_file, "current_direction=%d\n", player->current_direction);
	fprintf(player_file, "life=%d\n", player->life);
	fprintf(player_file, "state=%d\n", player->state);
	fprintf(player_file, "date_last_state=%d\n", player->date_last_state);
	fprintf(player_file, "range=%d\n", player->range);
	fprintf(player_file, "nb_bomb=%d\n", player->nb_bomb);
	fprintf(player_file, "key=%d\n", player->key);

	fclose(player_file);
}

void player_playerdata_load(char *player_path, struct player* player)
{
	int current_direction_raw;
	int state_raw;

	FILE* player_file = fopen(player_path, "r");

	fscanf(player_file, "x=%d\n", &(player->x));
	fscanf(player_file, "y=%d\n", &(player->y));
	fscanf(player_file, "current_direction=%d\n", &current_direction_raw);
	fscanf(player_file, "life=%d\n", &(player->life));
	fscanf(player_file, "state=%d\n", &state_raw);
	fscanf(player_file, "date_last_state=%d\n", &(player->date_last_state));
	fscanf(player_file, "range=%d\n", &(player->range));
	fscanf(player_file, "nb_bomb=%d\n", &(player->nb_bomb));
	fscanf(player_file, "key=%d\n", &(player->key));

	player->current_direction = (enum direction) current_direction_raw;			
	player->state = (enum state) state_raw;

	fclose(player_file);
}
