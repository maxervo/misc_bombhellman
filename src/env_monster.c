#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>

#include <env_monster.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>

struct monster {
  int exist;
  //Position
	int x;
	int y;
  enum direction current_direction;
  //Status
	int life;
  enum state state;
  enum aura aura;
  int date_last_action;
	int range;                 //Possible to upgrade later: when monster -> spitting fire
};

struct monster* env_monster_new()
{
  struct monster* env_monster = malloc( MAX_NUM_MONSTERS * sizeof(*env_monster));
  return env_monster;
}

void env_monster_init(struct monster* env_monster) {
  //Cleaning
  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {
    slot_monster_reset(env_monster+i);
  }
}

void slot_monster_reset(struct monster* monster)
{
  monster->exist = 0;                               //The rest is optional
  monster->x = 0;
  monster->y = 0;
  monster->current_direction = NORTH;

  monster->life = 0;
  monster->state = 0;
  monster->aura = 0;
  monster->date_last_action = 0;
  monster->range = 0;
}

void env_monster_free(struct monster* env_monster)
{
  free(env_monster);
}

void env_monster_add(int x, int y, enum direction current_direction, int life, int range, struct map* map)    //To think later: maybe add types of monsters, add more arguments...etc
{
  struct monster* env_monster = map_get_env_monster(map);
  for(int i=0; i < MAX_NUM_MONSTERS; i++) {
    if( !env_monster[i].exist ) {
      env_monster[i].exist = 1;
      env_monster[i].x = x;
      env_monster[i].y = y;
      env_monster[i].current_direction = current_direction;
      env_monster[i].life = life;
      env_monster[i].state = NORMAL;
      env_monster[i].aura = NONE;
      env_monster[i].date_last_action = SDL_GetTicks();
      env_monster[i].range = range;
      break;
    }
  }
}

void env_monster_refresh(struct map* map, struct player* player)
{
  struct monster* env_monster = map_get_env_monster(map);
  int monster_latency;                              //Time delay between last attack and current time

  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {

    if(env_monster[i].exist) {
      monster_latency = SDL_GetTicks() - env_monster[i].date_last_action;     //Possible to upgrade later: make a max latency specific for each type of monster

      if(env_monster[i].life <= 1) {          //Possible to upgrade later: more auras
        env_monster[i].aura = TOXIC;
      }

      if(env_monster[i].life <= 0) {                    //OK never goes below 0 cuz monster_affect_life secured
        env_monster[i].exist = 0;
        map_set_cell_type(map, env_monster[i].x, env_monster[i].y, CELL_EMPTY);
        slot_monster_reset(env_monster+i);
      }
      else if(monster_latency >= MONSTER_MAX_LATENCY) {
        monster_move_rand(env_monster+i, map, player);
        env_monster[i].date_last_action = SDL_GetTicks();
      }
      else {

        map_set_monster(env_monster[i].x, env_monster[i].y, map);
      }
    }

  }
}

void env_monster_display(struct map* map)
{
	assert(map);
	struct monster* env_monster = map_get_env_monster(map);

  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {
    if(env_monster[i].exist)
    {
      window_display_image(sprite_get_monster(env_monster[i].current_direction), env_monster[i].x * SIZE_BLOC, env_monster[i].y * SIZE_BLOC);
      if(env_monster[i].aura == TOXIC) {
        window_display_image_transparent(sprite_get_aura_toxic(), env_monster[i].x * SIZE_BLOC, env_monster[i].y * SIZE_BLOC, DEFAULT_TRANSPARENCY);     //128 : default transparency //To think later: make better design to avoid magic numbers
      }
    }
  }

}

void monster_move_rand(struct monster* monster, struct map* map, struct player* player) {
	int x = monster->x;      //Initially before moving
	int y = monster->y;
	int move = 0;
  int rand_choice = (rand() % 4);               //Random Indicator: tells monster where to go
                                                //possible to upgrade later: make intelligent pathfinding mouvements, going towards player + sometimes being random (depending on state...etc) : use djikstraa algorithm + queues implementation with linked lists

  switch (rand_choice) {
  case NORTH:
    if (monster_move_aux(monster, map, x, y - 1, player)) {
      monster->current_direction = NORTH;
      monster->y--;
      move = 1;
    }
    break;
  case SOUTH:
    if (monster_move_aux(monster, map, x, y + 1, player)) {
      monster->current_direction = SOUTH;
      monster->y++;
      move = 1;
    }
    break;
  case WEST:
    if (monster_move_aux(monster, map, x - 1, y, player)) {
      monster->current_direction = WEST;
      monster->x--;
      move = 1;
    }
    break;
  case EAST:
    if (monster_move_aux(monster, map, x + 1, y, player)) {
      monster->current_direction = EAST;
      monster->x++;
      move = 1;
    }
    break;
  }

  if(move) {
    map_set_cell_type(map, x, y, CELL_EMPTY);
		map_set_cell_type(map, monster->x, monster->y, CELL_MONSTER);
  }

}

int monster_move_aux(struct monster* monster, struct map* map, int x, int y, struct player* player)
{
  if(!map_is_inside(map, x, y))
    return 0;
  if(monster_mouvement_inhibition(monster)) {
    return 0;
  }

  switch (map_get_cell_type(map, x, y)) {

    case CELL_EMPTY:
      return 1;
      break;
    case CELL_PLAYER:
      player_handle_monster(player);
      return 0;
      break;
    case CELL_FIRE:
      monster_affect_life(monster, -1);
      return 0;
      break;
    default:
      return 0;
      break;
  }
}

void monster_affect_life(struct monster* monster, int modifier)
{
	assert(monster);
  int result = monster->life + modifier;
	if(result >= 0) {										        //Security is done here; Always use this procedure to affect life of monster, otherwise risks of negative life and crash (e.g display_banner...etc)
		monster->life = result;
	}
	else {
		monster->life = 0;
	}
}

struct monster* monster_locate(int x, int y, struct map* map)
{
  struct monster* env_monster = map_get_env_monster(map);

  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {
    if(env_monster[i].x == x && env_monster[i].y == y) {
      return env_monster+i;
    }
  }
  return NULL;
}

void monster_handle_fire(int x, int y, struct map* map)
{
  struct monster* monster = monster_locate(x, y, map);
  if(monster->state == NORMAL) {
    monster_affect_life(monster, -1);
    monster->state = BURNT;
  }
}

void monster_handle_extinguish(int x, int y, struct map* map)
{
  struct monster* monster = monster_locate(x, y, map);
  monster->state = NORMAL;                                      //Possible to upgrade later: design when there will be several states...etc
}

int monster_mouvement_inhibition(struct monster* monster)
{
	assert(monster);
	enum state mvt_inhibitors[2] = { BURNT, PARALYZED };					//Possible to upgrade later: idem comment

	return presence_array(monster->state, (int *) mvt_inhibitors, 2);
}

void env_monster_save_current(struct map* map)
{
  struct monster* env_monster = map_get_env_monster(map);
  char env_monster_path[50]; sprintf(env_monster_path, "save/roaming/env_monster/env_monster_%d", map_get_id(map));
	FILE* env_monster_file = fopen(env_monster_path, "w");	//OK overwrites				//To think of later: idem comment

  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {             //OK: we save all, even those which exist=0
    fprintf(env_monster_file, "Monster %d:\n", i);
    fprintf(env_monster_file, "exist=%d\n", env_monster[i].exist);
    fprintf(env_monster_file, "x=%d\n", env_monster[i].x);
    fprintf(env_monster_file, "y=%d\n", env_monster[i].y);
    fprintf(env_monster_file, "current_direction=%d\n", env_monster[i].current_direction);
    fprintf(env_monster_file, "life=%d\n", env_monster[i].life);
    fprintf(env_monster_file, "state=%d\n", env_monster[i].state);
    fprintf(env_monster_file, "aura=%d\n", env_monster[i].aura);
    fprintf(env_monster_file, "date_last_action=%d\n", env_monster[i].date_last_action);
    fprintf(env_monster_file, "range=%d\n", env_monster[i].range);
    fprintf(env_monster_file, "\n");
  }

	fclose(env_monster_file);
}

void env_monster_load(char *env_monster_path, struct map* map)
{
  struct monster* env_monster = map_get_env_monster(map);
  int current_direction_raw;
  int state_raw;
  int aura_raw;
	FILE* env_monster_file = fopen(env_monster_path, "r");

  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {         //The whole environment is overwritten OK
    fscanf(env_monster_file, "Monster %*d:\n");       //Flush : %*d wildcard, matches but does not assign it, OK
    fscanf(env_monster_file, "exist=%d\n", &(env_monster[i].exist));
    fscanf(env_monster_file, "x=%d\n", &(env_monster[i].x));
    fscanf(env_monster_file, "y=%d\n", &(env_monster[i].y));
    fscanf(env_monster_file, "current_direction=%d\n", &current_direction_raw);
    fscanf(env_monster_file, "life=%d\n", &(env_monster[i].life));
    fscanf(env_monster_file, "state=%d\n", &state_raw);
    fscanf(env_monster_file, "aura=%d\n", &aura_raw);
    fscanf(env_monster_file, "date_last_action=%d\n", &(env_monster[i].date_last_action));
    fscanf(env_monster_file, "range=%d\n", &(env_monster[i].range));
    fscanf(env_monster_file, "\n");             //Flush : the separator

    env_monster[i].current_direction = (enum direction) current_direction_raw;
    env_monster[i].state = (enum state) state_raw;
    env_monster[i].aura = (enum aura) aura_raw;
    env_monster[i].date_last_action = 0;
  }

	fclose(env_monster_file);
}

/* Not necessary, OK because even during pause : monsters will only one action at a time, no need to adjust time_last_action
void env_monster_affect_time(struct map* map, int modifier)
{
  struct monster* env_monster = map_get_env_monster(map);
  for(int i = 0; i < MAX_NUM_MONSTERS; i++) {
    if(env_monster[i].exist) {
      env_monster[i].date_last_action = env_monster[i].date_last_action + modifier;
    }
  }
}
*/
