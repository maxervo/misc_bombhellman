#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>

struct map {
	int id;
	int width;
	int height;
	unsigned char* grid;
	struct bomb* env_bomb;
	struct monster* env_monster;
};

#define CELL(i,j) ( i + j * map->width)

struct map* map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof *map);

	//Bomb environment
	map->env_bomb = env_bomb_new();

	//Monster environment
	map->env_monster = env_monster_new();

	//Grid
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;

	map->grid = malloc(height * width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;

	return map;
}

int map_is_inside(struct map* map, int x, int y)
{
	assert(map);
	return (0 <= x && x < MAP_WIDTH) && (0 <= y && y < MAP_HEIGHT);
}

void map_free(struct map* map)
{
	if (map == NULL )
		return;
	free(map->grid);
	free(map->env_bomb);
	free(map->env_monster);
	free(map);
}

int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}

int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
}

enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x0f;
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}

void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type >> 4) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
	case BONUS_LIFE:
		window_display_image(sprite_get_banner_life(), x, y);
		break;
	}
}

void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type >> 4) { // sub-types are encoded with the 4 most significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	}
}

void map_display(struct map* map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];

	    switch (type & 0x0f) {
				case CELL_SCENERY:
				  display_scenery(map, x, y, type);
				  break;
			  case CELL_CASE:
			    window_display_image(sprite_get_box(), x, y);
			    break;
			  case CELL_BONUS:
			    display_bonus(map, x, y, type);
			    break;
			  case CELL_KEY:
			    window_display_image(sprite_get_key(), x, y);
			    break;
				case CELL_BOMB:
					display_bomb(map, x, y, type);
					break;
			  case CELL_DOOR:
			    display_door(map, x,  y, type);
			    break;
				case CELL_FIRE:
					window_display_image(sprite_get_fire(), x, y);
					break;
				case CELL_PRINCESS:
					window_display_image(sprite_get_princess(), x, y);
					break;
	    }
	  }
	}
}

//NEW

//the function has been made: void model
//model in assignement .pdf not elegant, better to implement first version made in class (assignement pdf v1 hexa / free choice of data saving format) with values Zero Filled
void mapdata_load(char *path_to_map, struct map* map)	//attention: path relative to "general directory" where "make" is done
{
	unsigned char map_parsed[MAP_WIDTH * MAP_HEIGHT];

	sscanf(path_to_map, "save/%*[a-zA-Z1-9]/world/map_%d", &(map->id));		//Format String: Extract ID
	parse_map(path_to_map, map_parsed);																		//Environments Init (monsters, bombs) done in game_new() or in other functions when needed (teleport..etc)
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		map->grid[i] = map_parsed[i];																				//Here grid = one dimension, all aligned e.i function CELL(i,j) finds the correct value in map 2 dimensions
	}
}

void parse_map(char *path_to_map, unsigned char *map_parsed)						//Several versions of parsing possible (using sscanf..etc)
{
	int width_extracted; int height_extracted;
	FILE *mapdata = fopen(path_to_map, "r");	//"save/roaming/world/map_1" for instance
	if(mapdata == NULL) {
		fprintf(stderr, "Error: unable to open file\n");
		exit(EXIT_FAILURE);
	}
	char node_buffer[3]; 									//One node OK 3 sufficient cuz 8bits -> max 255
	int node_index = -1;
	char c;																//One character in node_buffer ; no need unsigned cuz here string node_buffer it's atoi that will give final number directly taken by map_parsed[node_index]
	int c_index = 0;

	//Extract & Verify Map Format
	fscanf(mapdata, "%d:%d\n", &width_extracted, &height_extracted);
	if(width_extracted != MAP_WIDTH || height_extracted != MAP_HEIGHT) {
		fprintf(stderr, "Map format incompatible\n");
		exit(EXIT_FAILURE);
	}

	//Parsing
	for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {

		//Get one node value
		c_index = 0;
		node_buffer[0] = '\0'; node_buffer[1] = '\0'; node_buffer[2] = '\0';
		while( (c = fgetc(mapdata)) != EOF && c != ' ' && c != '\n' ) {	//condensed loop condition
			node_buffer[c_index]=c;
			c_index++;
		}
		node_index++;
		map_parsed[node_index]=atoi(node_buffer);

		//Flush: flush the extra spaces before starting buffer for new node
		while((c = fgetc(mapdata)) == ' ');
		fseek(mapdata, -1, SEEK_CUR); //re-adjust cursor due to flush

	}
	fclose(mapdata);
}

void display_door(struct map* map, int x, int  y, unsigned char type)
{
	switch ( (type & 0x80) >> 7 ) {  // AND mask 1000 0000
	case 1:
		window_display_image(sprite_get_door_opened(), x, y);
		break;

	case 0:
		window_display_image(sprite_get_door_closed(), x, y);
		break;
	}
}

int map_get_cell_SUBtype(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return ( (map->grid[CELL(x,y)] & 0xf0) >> 4 );	// AND mask 1111 0000
}

int map_get_id(struct map* map)
{
	assert(map);
	return map->id;
}

struct bomb* map_get_env_bomb(struct map* map)
{
	assert(map);
	return map->env_bomb;
}

void map_seek_portal(int portal_id, struct map* map, int *p_x_portal, int *p_y_portal) 							//portal_id : 3bits coding world target : 110...etc
{
	assert(map);

	int i, j;
	for (i = 0; i < map_get_width(map); i++) {
	  for (j = 0; j < map_get_height(map); j++) {
	    if( (map_get_cell_type(map, i, j) == CELL_DOOR) && ( (map_get_cell_SUBtype(map, i, j) & 0x7) == portal_id) ) {	// & 0111 to extract only portal_id
				*p_x_portal = i;
	      *p_y_portal = j;
	    }
	  }
	}
}

void map_set_cell_SUBtype(struct map* map, int x, int y, int raw_subtype)				// raw: 0001 (coded on 4 bits here)		//Attention: only call it after being sure that cell_type ok (e.g ok from the start or call map_set_cell_type first...etc)
{
	assert(map && map_is_inside(map, x, y));
	int container_subtype = raw_subtype << 4; // container:	e.g 0001 0000							//Bitwise Operation:
																																					    		//0001 0000
	map->grid[CELL(x,y)] = (container_subtype | map_get_cell_type(map, x, y)); 	 //OR 0000 0101
}


void map_set_bomb(int x_bomb, int y_bomb, int lifespan, struct map* map, struct player* player)
{
	int quarter_lifespan = BOMB_MAX_LIFESPAN / 4;					//By default: we use quarter fractions
	int bomb_type = BOMB_4;																//Attention: reverse order (due to name given in assignement pdf) //possible to rename all the bombs in a more logical manner

	if(lifespan >= quarter_lifespan) {
		bomb_type = BOMB_3;
	}
	if(lifespan >= 2*quarter_lifespan) {
		bomb_type = BOMB_2;
	}
	if(lifespan >= 3*quarter_lifespan) {
		bomb_type = BOMB_1;
	}

	map_set_cell_type(map, x_bomb, y_bomb, CELL_BOMB);
	map_set_cell_SUBtype(map, x_bomb, y_bomb, bomb_type);

}

unsigned char map_get_cell_FULLtype(struct map* map, int x, int y)
{
	return map->grid[CELL(x,y)];
}

void display_bomb(struct map* map, int x, int y, unsigned char type)
{
	switch (type >> 4)
	{
		case BOMB_1:		//bomb almost exploded
			window_display_image(sprite_get_bomb(BOMB_1), x, y);
			break;
		case BOMB_2:
			window_display_image(sprite_get_bomb(BOMB_2), x, y);
			break;
		case BOMB_3:
			window_display_image(sprite_get_bomb(BOMB_3), x, y);
			break;
		case BOMB_4:		//bomb recently set
			window_display_image(sprite_get_bomb(BOMB_4), x, y);
			break;
	}
}

struct monster* map_get_env_monster(struct map* map)
{
	assert(map);
	return map->env_monster;
}

void map_set_monster(int x, int y, struct map* map)
{
	map_set_cell_type(map, x, y, CELL_MONSTER);
}

void map_save_current(struct map* map)		//Only the grid, other components carried out by proper authorized functions in their own module
{
	int width = map_get_width(map);
	int height = map_get_height(map);
	int current_id = map_get_id(map);
	unsigned char fulltype;
	char path_to_map[50]; sprintf( path_to_map, "save/roaming/world/map_%d", current_id );
	FILE* current_id_file = fopen("save/roaming/world/current_id", "w");
	FILE* map_file = fopen(path_to_map, "w");

	fputc(current_id + '0', current_id_file);		//convert to ASCII before
	fclose(current_id_file);

	fprintf(map_file, "%d:%d\n", width, height);
	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++) {
			fulltype = map->grid[CELL(i,j)];
			fprintf(map_file, "%d", fulltype);																//One space " " between each value 	//No need to rearrange as in assignement .pdf, not mentionned for saving...etc
			if(i != (width - 1) ) { fprintf(map_file, " "); }									//Security to prevent a space at the end of line
		}
		fprintf(map_file, "\n");			//end of the line when one line of the map is saved
	}
	fclose(map_file);

}



/* Quick tools */
void map_clean_env(struct map* map, struct player* player)
{
	assert(map != NULL);
	int nb_bomb_set = 0;

	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {

	    unsigned char type = map->grid[CELL(i,j)];

	    switch (type & 0x0f) {
			case CELL_BOMB:
				map_set_cell_type(map, i, j, CELL_EMPTY);
				nb_bomb_set++;
				break;
			case CELL_FIRE:
				map_set_cell_type(map, i, j, CELL_EMPTY);
				break;
	    }
	  }
	}
	player_affect_nb_bomb(player, nb_bomb_set);			//re-adjust player's nb_bomb
}
