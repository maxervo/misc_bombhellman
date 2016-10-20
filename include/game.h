#ifndef GAME_H_
#define GAME_H_

#include <player.h>
#include <map.h>
#include <env_bomb.h>
#include <env_monster.h>

// Abstract data type
struct game;

// Create a new game
struct game* game_new();
void game_init(struct game* game, enum cursor mode, char* load_profile);

// Free a game
void game_free(struct game* game);

// Return the player of the current game
struct player* game_get_player(struct game* game);

// Return the current map
struct map* game_get_map(struct game* game);

// Display the game on the screen
void game_display(struct game* game);

// update
void game_update(struct game* game);

//NEW
void game_control(struct game* game);
void pause(struct game* game);
void game_display_screen_effects(struct game* game);
void game_hear(struct game* game);
void game_input(struct game* game);
enum status game_get_status(struct game* game);
void input_keyboard(struct game* game);
void lose_display();
void win_display();
void menu_display(struct game* game);
void menu_affect_cursor(struct game* game, int cursor_mod);
void menu_choice(struct game* game);

#endif /* GAME_H_ */
