//NEW

#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>

#include <env_bomb.h>
#include <misc.h>
#include <sound.h>

struct bomb {
  int exist;
	int x;
	int y;
	int date;
  int date_fire;
	int range;
};

struct bomb* env_bomb_new()
{
  struct bomb* env_bomb = malloc( MAX_NUM_BOMBS * sizeof(*env_bomb));
  return env_bomb;
}

void env_bomb_init(struct bomb* env_bomb)
{
  //Cleaning
  for(int i = 0; i < MAX_NUM_BOMBS; i++) {
    slot_bomb_reset(env_bomb+i);
  }
}

void slot_bomb_reset(struct bomb* bomb)
{
  bomb->exist = 0;      //The rest is optional
  bomb->x = 0;
  bomb->y = 0;
  bomb->date = 0;
  bomb->date_fire = 0;
  bomb->range = 0;
}

void env_bomb_free(struct bomb* env_bomb)
{
  free(env_bomb);
}

int env_bomb_add(int x, int y, int range, struct map* map)
{
  struct bomb* env_bomb = map_get_env_bomb(map);
  for(int i=0; i < MAX_NUM_BOMBS; i++) {
    if( !env_bomb[i].exist ) {
      env_bomb[i].exist = 1;
      env_bomb[i].x = x;
      env_bomb[i].y = y;
      env_bomb[i].date = SDL_GetTicks();                        //Bomb placement date (the time when it was placed on map) in milliseconds
      env_bomb[i].date_fire = BOMB_NOT_EXPLODED;                //Negative value -1 when not exploded //fire spread date (the time when the bomb exploded)
      env_bomb[i].range = range;
      return BOMB_ADD_SUCCESS;
    }
  }
  return BOMB_ADD_FAILURE;
}

void env_bomb_refresh(struct map* map, struct player* player)
{
  struct bomb* env_bomb = map_get_env_bomb(map);
  int bomb_lifespan;
  int bomb_fire_lifespan;

  for(int i = 0; i < MAX_NUM_BOMBS; i++) {

    if(env_bomb[i].exist) {
      //Bomb : Not Exploded
      if(env_bomb[i].date_fire == BOMB_NOT_EXPLODED) {
        bomb_lifespan = SDL_GetTicks() - env_bomb[i].date;

        //Explosion
        if(bomb_lifespan >= BOMB_MAX_LIFESPAN) {                          //By default: 4000 milliseconds = 4s the time the bomb explodes
          env_bomb[i].date_fire = BOMB_EXPLOSION;            //Bomb has exploded, effects will be shown in next env_bomb_refresh(...) in else condition
          sound_play_effect(sound_get_effect_bomb_explosion());
          player_affect_nb_bomb(player, 1);
        }

        //Place : Bomb
        else {
          map_set_bomb(env_bomb[i].x, env_bomb[i].y, bomb_lifespan, map, player);                   //To think of later: idem comment
        }
      }

      //Bomb : Explosion
      else if (env_bomb[i].date_fire == BOMB_EXPLOSION) {
        env_fire_spread(env_bomb[i].x, env_bomb[i].y, env_bomb[i].range, map, player);
        env_bomb[i].date_fire = SDL_GetTicks();
      }

      //Bomb : Exploded
      else {
        bomb_fire_lifespan = SDL_GetTicks() - env_bomb[i].date_fire;

        //Extinguish
        if(bomb_fire_lifespan >= BOMB_FIRE_MAX_LIFESPAN) {                  //By default: 2000 milliseconds
          env_fire_clean(env_bomb[i].x, env_bomb[i].y, env_bomb[i].range, map, player);
          slot_bomb_reset(env_bomb+i);
        }
      }
    }
  }
}

void env_fire_spread(int x_bomb, int y_bomb, int range_bomb, struct map* map, struct player *player)
{
  int success; int i;

  //Center
  env_fire_create(x_bomb, y_bomb, map, player);

  //UP
  success = 1; i = 1;
  while(success && i <= range_bomb) {                                                        //Possible to do as well: recursive function for propagation instead of while loop
    success = env_fire_create(x_bomb, y_bomb-i, map, player);
    i++;
  }
  //LEFT
  success = 1; i = 1;
  while(success && i <= range_bomb) {
    success = env_fire_create(x_bomb-i, y_bomb, map, player);
    i++;
  }
  //DOWN
  success = 1; i = 1;
  while(success && i <= range_bomb) {
    success = env_fire_create(x_bomb, y_bomb+i, map, player);
    i++;
  }
  //RIGHT
  success = 1; i = 1;
  while(success && i <= range_bomb) {
    success = env_fire_create(x_bomb+i, y_bomb, map, player);
    i++;
  }

}

int env_fire_create(int x, int y, struct map* map, struct player *player)
{
  if(map_is_inside(map, x, y)) {

    int x_player = player_get_x(player);
    int y_player = player_get_y(player);
    int cell_type = map_get_cell_type(map, x, y);

    //Player : Virtual Position (Not set on map, showed in foreground)
    if( (x == x_player) && (y == y_player) ) {
      if( player_get_state(player) != BURNT ) {          //Damage dealt only once
        player_affect_life(player, -1);
      }
      player_affect_state(player, BURNT);       //Unable to move until fire extinguished
      player_affect_date_last_state(player, SDL_GetTicks());
      map_set_cell_type(map, x, y, CELL_FIRE);  //Player will be overwritten on map OK (other option do transparent effect and no node placement on map) but function extinguish will make him reappear
      return FIRE_CREATE_SUCCESS;
    }

    //Map Elements : Real Position (Set on map)
    switch( cell_type ) {

      case CELL_BOMB:
        map_set_cell_type(map, x, y, CELL_FIRE);
        return FIRE_CREATE_SUCCESS;
        break;
      case CELL_EMPTY:
        map_set_cell_type(map, x, y, CELL_FIRE);
        return FIRE_CREATE_SUCCESS;
        break;
      case CELL_FIRE:                                //Already on fire -> keep it on fire
        map_set_cell_type(map, x, y, CELL_FIRE);
        return FIRE_CREATE_FAILURE;
        break;

      case CELL_CASE:
        env_case_transform(map, x, y);
        return FIRE_CREATE_FAILURE;
        break;
      case CELL_MONSTER:
        monster_handle_fire(x,y,map);
        return FIRE_CREATE_FAILURE;
        break;

      case CELL_BONUS:
        map_set_cell_type(map, x, y, CELL_FIRE);
        return FIRE_CREATE_SUCCESS;
        break;

      case CELL_KEY:
        return FIRE_CREATE_SUCCESS;     //To allow propagation, even if key not affected ; but doors: block propagation...etc
      break;

      default:
          return FIRE_CREATE_FAILURE;
        break;
    }
  }
  return FIRE_CREATE_FAILURE; //out of the map
}

void env_fire_clean(int x_bomb, int y_bomb, int range_bomb, struct map* map, struct player *player)
{
  int success; int i;

  //Center
  env_fire_extinguish(x_bomb, y_bomb, map, player);

  //UP
  success = 1; i = 1;
  while(success && i <= range_bomb) {

    success = env_fire_extinguish(x_bomb, y_bomb-i, map, player);
    i++;
  }
  //LEFT
  success = 1; i = 1;
  while(success && i <= range_bomb) {
    success = env_fire_extinguish(x_bomb-i, y_bomb, map, player);
    i++;
  }
  //DOWN
  success = 1; i = 1;
  while(success && i <= range_bomb) {
    success = env_fire_extinguish(x_bomb, y_bomb+i, map, player);
    i++;
  }
  //RIGHT
  success = 1; i = 1;
  while(success && i <= range_bomb) {
    success = env_fire_extinguish(x_bomb+i, y_bomb, map, player);
    i++;
  }
}

int env_fire_extinguish(int x, int y, struct map* map, struct player *player)
{
  if(map_is_inside(map, x, y)) {

    int x_player = player_get_x(player);
    int y_player = player_get_y(player);
    int cell_type = map_get_cell_type(map, x, y);

    //Player : Virtual Position (Not set on map, showed in foreground)               //Need to check if cell_type = cell_fire cuz if case_exploded leaves a bonus -> when player having a condition "poisoned"..etc comes and takes bonus and waits for extinguish to go on him then he is healed of his condition
    if( (cell_type == CELL_FIRE) && (x == x_player) && (y == y_player) ) {          //Player: if caught, he disappears from map, replaced by flames, exist virtually through position
      map_set_cell_type(map, x, y, CELL_PLAYER);
      player_affect_state(player, NORMAL);
      player_affect_date_last_state(player, SDL_GetTicks());
      return FIRE_EXTINGUISH_SUCCESS;
    }

    //Map Elements : Real Position (Set on map)         //Optional to end extinguish when propagation meets hindrance, however if extinguish all : can cure some of player's state if standing on cell affected by extinguish
    switch( cell_type ) {

      case CELL_FIRE:
        map_set_cell_type(map, x, y, CELL_EMPTY);
        return FIRE_EXTINGUISH_SUCCESS;
        break;

      case CELL_BONUS:
        return FIRE_EXTINGUISH_SUCCESS;
        break;
      case CELL_KEY:
        return FIRE_EXTINGUISH_SUCCESS;
        break;
      case CELL_EMPTY:
        return FIRE_EXTINGUISH_SUCCESS;                   //Continue extinguish even if EMPTY_CELL cuz monster can move even if set on fire thus extinguishing the flames (and the rest of the flames must be extinguished)
        break;
      case CELL_MONSTER:
        monster_handle_extinguish(x, y, map);
        return FIRE_EXTINGUISH_SUCCESS;
        break;

      default:
        return FIRE_EXTINGUISH_FAILURE;
        break;
    }
  }
  return FIRE_EXTINGUISH_FAILURE;   //Out of the map
}

void env_case_transform(struct map* map, int x, int y)
{
  int raw_subtype = map_get_cell_SUBtype(map, x, y);
  switch(raw_subtype)
  {
    case BONUS_EMPTY:
      map_set_cell_type(map, x, y, CELL_EMPTY);
      break;

    case BONUS_BOMB_RANGE_DEC:            //All included here thanks to raw_subtype -> generalize
    case BONUS_BOMB_RANGE_INC:
    case BONUS_BOMB_NB_DEC:
    case BONUS_BOMB_NB_INC:
    case BONUS_LIFE:
      map_set_cell_type(map, x, y, CELL_BONUS);
      map_set_cell_SUBtype(map, x, y, raw_subtype);
      break;

    case BONUS_MONSTER:
      env_monster_add(x, y, SOUTH, 3, 2, map);
      map_set_cell_type(map, x, y, CELL_MONSTER);
      break;

    default:
      break;
  }

}

void env_bomb_affect_time(struct map* map, int modifier)
{
  struct bomb* env_bomb = map_get_env_bomb(map);
  for(int i = 0; i < MAX_NUM_BOMBS; i++) {
    if(env_bomb[i].exist) {
      env_bomb[i].date = env_bomb[i].date + modifier;
      if(env_bomb[i].date_fire != BOMB_NOT_EXPLODED) {                      //Means date_fire > 0 (it's a date) thus bomb has already exploded
        env_bomb[i].date_fire = env_bomb[i].date_fire + modifier;
      }
    }
  }
}
