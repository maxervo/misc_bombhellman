#ifndef PLAYER_H_
#define PLAYER_H_

#include <constant.h>

struct player;              //To think of later: comment idem

#include <map.h>
#include <env_bomb.h>       //To think of later: Important to place here, if placed before struct player then warning all over the place: functions using struct player in env_bomb.h will be placed here so need to know that struct exists; is there a better design? Here .h are mutually included; consequences: all over the place warnings cuz player.h is included in other files so pb

// Creates a new player with a given number of available bombs
struct player* player_new();
void   player_free(struct player* player);

// Returns the current position of the player
int player_get_x(struct player* player);
int player_get_y(struct player* player);

// Set the direction of the next move of the player
void player_set_current_way(struct player * player, enum direction direction);

// Set, Increase, Decrease the number of bomb that player can put
int  player_get_nb_bomb(struct player * player);
void player_inc_nb_bomb(struct player * player);
void player_dec_nb_bomb(struct player * player);

// Load the player position from the map
void player_from_map(struct player* player, struct map* map);

// Move the player according to the current direction
int player_move(struct player* player, struct map* map);

// Display the player on the screen
void player_display(struct player* player);

//NEW
int player_push(struct player* player, struct map* map, int x_target, int y_target);
void door_handle(struct player *player, struct map* map, int x, int y);
void player_teleport(struct player *player, struct map* map, int portal_id_enter);
int player_get_key(struct player* player);
void player_set_bomb(struct player* player, struct map* map);
enum state player_get_state(struct player* player);
void player_affect_state(struct player* player, enum state new_state);

int player_mouvement_inhibition(struct player* player);
void player_affect_life(struct player* player, int modifier);
void player_affect_nb_bomb(struct player* player, int modifier);
void player_affect_range(struct player* player, int modifier);
int player_get_range(struct player* player);
void player_get_bonus(struct player* player, struct map* map, int x, int y);
void player_refresh(struct player* player, struct map* map);
void player_update_state(struct player* player);
int player_get_date_last_state(struct player* player);
void player_affect_date_last_state(struct player* player, int new_date);
void player_handle_monster(struct player* player);
void player_save_current(struct player* player);
void player_playerdata_load(char *player_path, struct player* player);

#endif /* PLAYER_H_ */
