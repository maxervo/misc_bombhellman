#include <SDL/SDL_image.h>
#include <assert.h>

#include <sprite.h>
#include <misc.h>

//Attention Important : compile from main project directory cuz all paths to load map elements are relative to main project directory

//Possible to add even more tiles grass (several tiles of variations cf. perlin noise modelisation), other tiles...etc

// Menu
#define MENU_BACKGROUND "sprite/menu/background.jpg"
#define MENU_LAYOUT "sprite/menu/layout.png"
#define MENU_DOT "sprite/menu/dot.png"
#define MENU_WIN "sprite/menu/win.png"
#define MENU_LOSE "sprite/menu/lose.png"

// Sprites general
#define MAP_CASE        "sprite/wood_box.png"
#define MAP_KEY			"sprite/key.png"
#define MAP_DOOR_OPENED	"sprite/door.png"
#define MAP_DOOR_CLOSED	"sprite/closed_door.png"
#define MAP_FIRE "sprite/fire.png"
#define MAP_PRINCESS "sprite/bomberwoman.png"

// Scenery elements
#define MAP_STONE		"sprite/stone.png"
#define MAP_TREE        "sprite/tree.png"

// Sprites of Banner
#define BANNER_LINE		"sprite/banner_line.png"
#define BANNER_LIFE		"sprite/banner_life.png"
#define BANNER_BOMB		"sprite/bomb3.png"
#define BANNER_RANGE	"sprite/banner_range.png"
#define BANNER_0		"sprite/banner_0.jpg"
#define BANNER_1		"sprite/banner_1.jpg"
#define BANNER_2		"sprite/banner_2.jpg"
#define BANNER_3		"sprite/banner_3.jpg"
#define BANNER_4		"sprite/banner_4.jpg"
#define BANNER_5		"sprite/banner_5.jpg"
#define BANNER_6		"sprite/banner_6.jpg"
#define BANNER_7		"sprite/banner_7.jpg"
#define BANNER_8		"sprite/banner_8.jpg"
#define BANNER_9		"sprite/banner_9.jpg"

// Sprites of Bombs
#define BOMB_TTL1       "sprite/bomb1.png"
#define BOMB_TTL2       "sprite/bomb2.png"
#define BOMB_TTL3       "sprite/bomb3.png"
#define BOMB_TTL4       "sprite/bomb4.png"

// Sprites of Bonus
#define IMG_BONUS_BOMB_RANGE_INC  "sprite/bonus_bomb_range_inc.png"
#define IMG_BONUS_BOMB_RANGE_DEC  "sprite/bonus_bomb_range_dec.png"
#define IMG_BONUS_BOMB_NB_INC     "sprite/bonus_bomb_nb_inc.png"
#define IMG_BONUS_BOMB_NB_DEC     "sprite/bonus_bomb_nb_dec.png"

// Sprites of Players
#define PLAYER_LEFT     "sprite/player_left.png"
#define PLAYER_UP       "sprite/player_up.png"
#define PLAYER_RIGHT    "sprite/player_right.png"
#define PLAYER_DOWN     "sprite/player_down.png"

//Sprites of Monsters 		//New
#define MONSTER_LEFT     "sprite/monster_left.png"
#define MONSTER_UP       "sprite/monster_up.png"
#define MONSTER_RIGHT    "sprite/monster_right.png"
#define MONSTER_DOWN     "sprite/monster_down.png"

// menu

SDL_Surface* menu_background;
SDL_Surface* menu_layout;
SDL_Surface* menu_dot;
SDL_Surface* menu_win;
SDL_Surface* menu_lose;

// banner
SDL_Surface* numbers[10];
SDL_Surface* banner_life;
SDL_Surface* banner_bomb;
SDL_Surface* banner_range;
SDL_Surface* banner_line;

// map
SDL_Surface* box;
SDL_Surface* goal;
SDL_Surface* key;
SDL_Surface* door_opened;
SDL_Surface* door_closed;
SDL_Surface* stone;
SDL_Surface* tree;
SDL_Surface* fire;
SDL_Surface* princess;

//aura
SDL_Surface* aura_toxic;

// bonus
#define NB_BONUS 4
SDL_Surface* bonus[NB_BONUS + 1];

// bomb
#define NB_BOMB 4
SDL_Surface* bomb[NB_BOMB + 1];

// player
SDL_Surface* player_img[4];

//monster
SDL_Surface* monster_img[4];

void menu_load() {
	menu_background = load_image(MENU_BACKGROUND);
	menu_layout = load_image(MENU_LAYOUT);
	menu_dot = load_image(MENU_DOT);
	menu_win = load_image(MENU_WIN);
	menu_lose = load_image(MENU_LOSE);
}

void menu_unload() {
	SDL_FreeSurface(menu_background);
	SDL_FreeSurface(menu_layout);
	SDL_FreeSurface(menu_dot);
	SDL_FreeSurface(menu_win);
	SDL_FreeSurface(menu_lose);
}

void banner_load() {
	// numbers imgs
	numbers[0] = load_image(BANNER_0);
	numbers[1] = load_image(BANNER_1);
	numbers[2] = load_image(BANNER_2);
	numbers[3] = load_image(BANNER_3);
	numbers[4] = load_image(BANNER_4);
	numbers[5] = load_image(BANNER_5);
	numbers[6] = load_image(BANNER_6);
	numbers[7] = load_image(BANNER_7);
	numbers[8] = load_image(BANNER_8);
	numbers[9] = load_image(BANNER_9);

	// other banner sprites
	banner_life = load_image(BANNER_LIFE);
	banner_bomb = load_image(BANNER_BOMB);
	banner_range = load_image(BANNER_RANGE);
	banner_line = load_image(BANNER_LINE);
}

void banner_unload() {
	// numbers imgs
	for (int i = 0; i < 10; i++) {
		SDL_FreeSurface(numbers[i]);
	}

	// other banner sprites
	SDL_FreeSurface(banner_life);
	SDL_FreeSurface(banner_bomb);
	SDL_FreeSurface(banner_range);
	SDL_FreeSurface(banner_life);
}

void map_load() {
	// Sprite loading
	tree = load_image(MAP_TREE);
	box = load_image(MAP_CASE);
	key = load_image(MAP_KEY);
	stone = load_image(MAP_STONE);
	door_opened = load_image(MAP_DOOR_OPENED);
	door_closed = load_image(MAP_DOOR_CLOSED);
	fire = load_image(MAP_FIRE);
	princess = load_image(MAP_PRINCESS);
}

void map_unload() {
	SDL_FreeSurface(tree);
	SDL_FreeSurface(box);
	SDL_FreeSurface(goal);
	SDL_FreeSurface(key);
	SDL_FreeSurface(stone);
	SDL_FreeSurface(door_opened);
	SDL_FreeSurface(door_closed);
	SDL_FreeSurface(fire);
	SDL_FreeSurface(princess);
}

void bonus_load() {
	bonus[0] = NULL;
	bonus[BONUS_BOMB_RANGE_INC] = load_image(IMG_BONUS_BOMB_RANGE_INC);
	bonus[BONUS_BOMB_RANGE_DEC] = load_image(IMG_BONUS_BOMB_RANGE_DEC);
	bonus[BONUS_BOMB_NB_INC] = load_image(IMG_BONUS_BOMB_NB_INC);
	bonus[BONUS_BOMB_NB_DEC] = load_image(IMG_BONUS_BOMB_NB_DEC);
}

void bonus_unload() {
	for (int i = 0; i < NB_BONUS; i++)
		if(bonus[i])
			SDL_FreeSurface(bonus[i]);
}

void bomb_load() {
	bomb[0] = NULL;
	bomb[BOMB_1] = load_image(BOMB_TTL1);
	bomb[BOMB_2] = load_image(BOMB_TTL2);
	bomb[BOMB_3] = load_image(BOMB_TTL3);
	bomb[BOMB_4] = load_image(BOMB_TTL4);
}

void bomb_unload() {
	for (int i = 0; i < NB_BOMB; i++) {
		if(bomb[i])
			SDL_FreeSurface(bomb[i]);
	}
}

void player_load() {
	player_img[WEST] = load_image(PLAYER_LEFT);
	player_img[EAST] = load_image(PLAYER_RIGHT);
	player_img[NORTH] = load_image(PLAYER_UP);
	player_img[SOUTH] = load_image(PLAYER_DOWN);
}

void player_unload() {
	for (int i = 0; i < 4; i++)
		SDL_FreeSurface(player_img[i]);
}

void monster_load() {
	monster_img[WEST] = load_image(MONSTER_LEFT);
	monster_img[EAST] = load_image(MONSTER_RIGHT);
	monster_img[NORTH] = load_image(MONSTER_UP);
	monster_img[SOUTH] = load_image(MONSTER_DOWN);
}

void monster_unload() {
	for (int i = 0; i < 4; i++)
		SDL_FreeSurface(monster_img[i]);
}

void others_load() {
	aura_toxic = load_image("sprite/aura_toxic.png");
}

void others_unload() {
	SDL_FreeSurface(aura_toxic);
}

void sprite_load() {
	menu_load();	
	map_load();
	bonus_load();
	bomb_load();
	monster_load();
	banner_load();
	player_load();
	others_load();
}

void sprite_free() {
	menu_unload();
	map_unload();
	bomb_unload();
	monster_unload();
	bonus_unload();
	banner_unload();
	player_unload();
	others_unload();
}

SDL_Surface* sprite_get_number(short number) {
	assert(number >= 0 && number < 9);
	return numbers[number];
}

SDL_Surface* sprite_get_player(enum direction direction) {
	assert(player_img[direction]);
	return player_img[direction];
}

SDL_Surface* sprite_get_princess() {
	assert(princess);
	return princess;
}

SDL_Surface* sprite_get_banner_life() {
	assert(banner_life);
	return banner_life;
}

SDL_Surface* sprite_get_banner_bomb() {
	assert(banner_bomb);
	return banner_bomb;
}

SDL_Surface* sprite_get_banner_line() {
	assert(banner_line);
	return banner_line;
}

SDL_Surface* sprite_get_banner_range() {
	assert(banner_range);
	return banner_range;
}

SDL_Surface* sprite_get_bonus(enum bonus_type bonus_type) {
	assert(bonus[bonus_type]);
	return bonus[bonus_type];
}

SDL_Surface* sprite_get_tree() {
	assert(tree);
	return tree;
}

SDL_Surface* sprite_get_box() {
	assert(box);
	return box;
}

SDL_Surface* sprite_get_key() {
	assert(key);
	return key;
}

SDL_Surface* sprite_get_stone() {
	assert(stone);
	return stone;
}

SDL_Surface* sprite_get_door_opened() {
	assert(door_opened);
	return door_opened;
}

SDL_Surface* sprite_get_door_closed() {
	assert(door_closed);
	return door_closed;
}

//NEW

SDL_Surface* sprite_get_fire() {
	assert(fire);
	return fire;
}

SDL_Surface* sprite_get_bomb(int bomb_type) {
	assert(fire);
	return bomb[bomb_type];
}

SDL_Surface* sprite_get_monster(enum direction direction) {
	assert(monster_img[direction]);
	return monster_img[direction];
}

SDL_Surface* sprite_get_aura_toxic() {
	assert(aura_toxic);
	return aura_toxic;
}

SDL_Surface* sprite_get_menu_background() {
	assert(menu_background);
	return menu_background;
}

SDL_Surface* sprite_get_menu_layout() {
	assert(menu_layout);
	return menu_layout;
}

SDL_Surface* sprite_get_menu_dot() {
	assert(menu_dot);
	return menu_dot;
}

SDL_Surface* sprite_get_menu_win() {
	assert(menu_win);
	return menu_win;
}

SDL_Surface* sprite_get_menu_lose() {
	assert(menu_lose);
	return menu_lose;
}
