#ifndef CONSTANT_H_
#define CONSTANT_H_

#define WINDOW_NAME "BombHellMan 2016"

// Size (# of pixels) of a cell of the map
#define SIZE_BLOC       40

// Size (# of pixels) of banner
#define LINE_HEIGHT		4
#define BANNER_HEIGHT	40

// Time management
#define DEFAULT_GAME_FPS 30

// menu
#define CURSOR_START_X 265
#define CURSOR_START_Y 205
#define CURSOR_GAP 115

// map size
#define MAP_WIDTH  12
#define MAP_HEIGHT 12

#define SCREEN_NATIVE_WIDTH MAP_WIDTH				//in blocks
#define SCREEN_NATIVE_HEIGHT MAP_HEIGHT + LINE_HEIGHT + BANNER_HEIGHT
#define MAX_NUM_BOMBS 9
#define BOMB_NOT_EXPLODED -1
#define BOMB_EXPLOSION 0
#define BOMB_ADD_SUCCESS 1
#define BOMB_ADD_FAILURE 0
#define FIRE_CREATE_SUCCESS 1
#define FIRE_CREATE_FAILURE 0
#define FIRE_EXTINGUISH_SUCCESS 1
#define FIRE_EXTINGUISH_FAILURE 0
#define MAX_SAVE_NAME_LENGTH 20
#define MAX_STATE_LATENCY_DEFAULT 4000

#define MAX_NUM_MONSTERS 9
#define MONSTER_MAX_LATENCY 2000

#define DEFAULT_TRANSPARENCY 128
#define BLINK_CYCLE 1000

enum cursor {
	NEW_GAME_BUTTON = 0,
	LOAD_GAME_BUTTON,
	QUIT_BUTTON
};

enum status {
	ROAMING = 0,
	PAUSE,
	MENU,					//To upgrade later: design, ingame menu forks from pause...etc
	EXIT,
	WIN,
	LOSE
};

enum direction {
	NORTH = 0,
	SOUTH, // 1
	WEST, // 2
	EAST //3
};

enum state {
	NORMAL = 0,
	BURNT,
	PARALYZED,
	GODMODE,
	LOVE_PRINCESS
};

enum aura {
	NONE = 0,
	TOXIC
};

#endif /* CONSTANT */
