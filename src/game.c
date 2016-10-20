#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <tool.h>
#include <window.h>
#include <sound.h>
#include <sprite.h>

struct game {
	struct map* map; // the game's map
	short nb_maps; // nb maps of the game
	struct player* player;
	enum status status;											//Possible to add more status, and for bigger games : possible to do a stack of all the game states, best design for multi-menus, stack of menus, views...etc
	int menu_cursor;
};

struct game* game_new() {
	struct game* game = malloc(sizeof(*game));
	sprite_load(); 	// load sprites into process memory
	game->map = NULL;
	game->player = NULL;
	game->nb_maps = 0;
	game->status = MENU;
	game->menu_cursor = 0;

	return game;
}
void game_init(struct game* game, enum cursor mode, char* load_profile) {

	//Allocation
	game->player = player_new();															//Allocate for player, need to free afterwards
	game->map = map_new(MAP_WIDTH, MAP_HEIGHT);								//Allocate for map, env_bomb, env_monster, need to free afterwards

	//Init
	env_monster_init(map_get_env_monster(game->map));					//Initialize environments
	env_bomb_init(map_get_env_bomb(game->map));

	//Mode
	if(mode == NEW_GAME_BUTTON) {
		cache_roaming_init("data/default_game");											//Prepare roaming cache, unified environment : everything will go through here before being saved, loaded...etc	//Possibility to do several games: just change source_directory from "data" to "data/game1" ...etc
		tool_load_savegame("roaming", game->player, game->map);				//We don't need : player_from_map() deprecated; everything revolves around virtual positions, not the map block placements
	}

	else if(mode == LOAD_GAME_BUTTON) {
		tool_load_savegame(load_profile, game->player, game->map);
	}

	game->nb_maps = 8;																						//Defined in the initial source code but not useful here
	game->status = ROAMING;

}

void game_free(struct game* game) {
	assert(game);

	if(game->player != NULL && game->map != NULL) {
		player_free(game->player);
		map_free(game->map);
	}
	free(game);
}

struct map* game_get_map(struct game* game) {
	assert(game);
	return game->map;
}


struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;

	window_display_image(sprite_get_number(map_get_id(game->map)), 0, y);

	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_life(game->player)), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_range(game->player)), x, y);

	if(player_get_key(game->player)) {
		x = 4 * white_bloc + 5 * SIZE_BLOC;
		window_display_image(sprite_get_key(), x, y);
	}

}

void game_display(struct game* game) {
	assert(game);
	window_clear();

	switch(game->status) {
		case ROAMING:
		case PAUSE:
			game_banner_display(game);
			map_display(game->map);
			env_monster_display(game->map);
			player_display(game->player);				//Draw last : on the foreground
			game_display_screen_effects(game);	//Draw last
			break;
		case MENU:
			menu_display(game);
			break;
		case WIN:
			win_display();
			break;
		case LOSE:
			lose_display();
			break;
		default:
			break;
	}

	window_refresh();
}

void input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_map(game);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			game->status = EXIT;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				game->status = EXIT;
				break;
			case SDLK_UP:
				if(game->status == ROAMING) {
					player_set_current_way(player, NORTH);
					player_move(player, map);
				}
				else if(game->status == MENU) {
					menu_affect_cursor(game, -1);
				}
				break;
			case SDLK_DOWN:
				if (game->status == ROAMING) {
					player_set_current_way(player, SOUTH);
					player_move(player, map);
				}
				else if(game->status == MENU) {
					menu_affect_cursor(game, 1);
				}
				break;
			case SDLK_RIGHT:
				if (game->status == ROAMING) {
					player_set_current_way(player, EAST);
					player_move(player, map);
				}
				break;
			case SDLK_LEFT:
				if (game->status == ROAMING) {
					player_set_current_way(player, WEST);
					player_move(player, map);
				}
				break;
			case SDLK_SPACE:
				if (game->status == ROAMING) {
					player_set_bomb(player, map);
				}
				break;
			case SDLK_RETURN:
				if (game->status == MENU) {
					menu_choice(game);
				}
				break;
			case SDLK_p:
				pause(game);
				break;
			case SDLK_s:
					if (game->status == ROAMING) {
						tool_save_game(player, map, "default");				//Possible to upgrade later: add multiple profiles...etc
						printf("Game has been saved as: default.\n");
						printf("You can load the savegame by choosing the option: Load Game at the main menu.\n");
					}
					break;
			default:
				break;
			}

			break;
		}
	}
}

void game_update(struct game* game) {
	game_control(game);
	player_refresh(game->player, game->map);
	env_monster_refresh(game->map, game->player);
	env_bomb_refresh(game->map, game->player);
}

//NEW

void game_input(struct game* game) {
		input_keyboard(game);
	//other types of input possible : mouse events...etc
}

void game_control(struct game* game)
{
	int player_life = player_get_life(game->player);
	enum state player_state = player_get_state(game->player);

	if(player_life <= 0) {																				//Game Over Condition
		printf("Game over\n");																			//OK never goes below 0 cuz player_affect_life() secured
		sound_play_effect(sound_get_evil_laugh());
		game->status = LOSE;
	}
	else if(player_state == LOVE_PRINCESS) {												//Victory Condition
		printf("You won\n");
		sound_play_effect(sound_get_congrats());
		game->status = WIN;
	}

}

void pause(struct game* game) {			//Efficient pause : uses the main loop, and thus the refresh rate -> processor is stable, (not 99% in the case of infinite loop without refresh rate)
	struct map* map = game->map;
	static int date_pause = 0;			//persistent in local scope to build up pause time
	int pause_time = 0;

	if (game->status == ROAMING) {
		date_pause = SDL_GetTicks();
		game->status = PAUSE;
	}
	else if (game->status == PAUSE) {
		pause_time = SDL_GetTicks() - date_pause;
		env_bomb_affect_time(map, pause_time);								//Possible to do env_monster_affect_time but OK, monsters only do maximum of one action after pause, good enough
		game->status = ROAMING;
	}
}

void game_display_screen_effects(struct game* game)						//Possible to upgrade later: add more colors, screen states...etc
{
	//Danger Blink
	static int date_danger = 0;																	//Static must be init to constant, OK 0, no need to do SDL_GetTicks() rigourously, then there will be red flash but we will avoid it
	int cycle_current_span = SDL_GetTicks() - date_danger;
	int transp_danger = cycle_current_span/8;

	//Effects
	if(game->status == PAUSE) {
		screen_effect("blank_blue", DEFAULT_TRANSPARENCY);
	}
	else if(player_get_life(game->player) <= 1) {								//		|________|________|________|___etc...___| linear progression
		if(cycle_current_span >= BLINK_CYCLE) {										//   0																				128
			date_danger = SDL_GetTicks();
			transp_danger = 0; 																//optional but OK to avoid red flash (max opacity) at the beginning of danger, cuz static = 0 at first
		}
		screen_effect("blank_red", transp_danger);
	}

}

void game_hear(struct game* game)
{
	if(game->status == ROAMING && player_get_life(game->player) <= 1) {							//OK Short Circuit Evaluation, Lazy Evaluation
		sound_play_music(sound_get_music_danger());
	}
	else {
		sound_play_music(sound_get_music_ambient());
	}
}

enum status game_get_status(struct game* game) {
	assert(game);
	return game->status;
}

/***********************************************************************************
:: Menu
***********************************************************************************/


void lose_display() {
	window_display_image(sprite_get_menu_background(), 0, 0);
	window_display_image(sprite_get_menu_lose(), 0, 0);
}

void win_display() {
	window_display_image(sprite_get_menu_background(), 0, 0);
	window_display_image(sprite_get_menu_win(), 0, 0);
}

void menu_display(struct game* game) {
	assert(game);
	int x_dot = CURSOR_START_X;
	int y_dot = CURSOR_START_Y + (game->menu_cursor) * CURSOR_GAP;
	window_display_image(sprite_get_menu_background(), 0, 0);
	window_display_image(sprite_get_menu_layout(), 0, 0);
	window_display_image(sprite_get_menu_dot(), x_dot, y_dot);
}


void menu_affect_cursor(struct game* game, int cursor_mod) {
	assert(game);
	int new_position = (game->menu_cursor) + cursor_mod;
	if(new_position >= NEW_GAME_BUTTON && new_position <= QUIT_BUTTON)  {
		game->menu_cursor = new_position;
	}
}


//When we press enter button on selected button
void menu_choice(struct game* game) {
	assert(game);
	switch(game->menu_cursor) {
		case NEW_GAME_BUTTON:														//Possible to upgrade later : more games available, several stories possible, each with their own world of maps...etc, for the moment it is "default_game"
			game_init(game, NEW_GAME_BUTTON, NULL);
			break;
		case LOAD_GAME_BUTTON:
			game_init(game, LOAD_GAME_BUTTON, "default");
			break;
		case QUIT_BUTTON:
			game->status = EXIT;
			break;
		default:
			fprintf(stderr, "Please input a correct option\n");
			break;
	}
}
