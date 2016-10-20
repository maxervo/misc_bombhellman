#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>					//To think later: is it good design here?

#include <constant.h>
#include <game.h>
#include <window.h>
#include <sound.h>
#include <misc.h>
#include <tool.h>


int main(int argc, char *argv[]) {

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		error("Can't init SDL:  %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}


	struct game* game = game_new();

	//Sound Init/Load
	mixer_init();
	sound_play_music(sound_get_music_ambient());

	//Window
	window_create(SIZE_BLOC * MAP_WIDTH,
	SIZE_BLOC * MAP_HEIGHT + BANNER_HEIGHT + LINE_HEIGHT);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// to obtain the DEFAULT_GAME_FPS, we have to reach a loop duration of (1000 / DEFAULT_GAME_FPS) ms
	int ideal_speed = 1000 / DEFAULT_GAME_FPS;
	int timer, execution_speed;

	// game loop
	// static time rate implementation						// game loop with refresh rate enables stable processor activity
	while (game_get_status(game) != EXIT) {
		timer = SDL_GetTicks();

		game_input(game);
		if(game_get_status(game) == ROAMING) {			//maintains refresh rate even in pause...etc no processing power 99%...etc
			game_update(game);
		}
		game_display(game);
		game_hear(game);

		execution_speed = SDL_GetTicks() - timer;
		if (execution_speed < ideal_speed)
			SDL_Delay(ideal_speed - execution_speed); // we are ahead of ideal time. let's wait.
	}

	game_free(game);

	//Sound Free
	mixer_free();

	SDL_Quit();

	return EXIT_SUCCESS;
}
