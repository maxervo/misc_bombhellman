#ifndef SPRITE_H_
#define SPRITE_H_

#include <SDL/SDL.h>
#include <map.h>
#include <constant.h>

// load game's sprites
void		 sprite_load();

SDL_Surface* sprite_get_player(enum direction direction);
SDL_Surface* sprite_get_princess();
SDL_Surface* sprite_get_bonus(enum bonus_type bonus_type);
SDL_Surface* sprite_get_tree();
SDL_Surface* sprite_get_box();
SDL_Surface* sprite_get_key();
SDL_Surface* sprite_get_stone();
SDL_Surface* sprite_get_door_opened();
SDL_Surface* sprite_get_door_closed();

SDL_Surface* sprite_get_number(short number);
SDL_Surface* sprite_get_banner_life();
SDL_Surface* sprite_get_banner_bomb();
SDL_Surface* sprite_get_banner_line();
SDL_Surface* sprite_get_banner_range();

//NEW
SDL_Surface* sprite_get_fire();
SDL_Surface* sprite_get_bomb(int bomb_type);
void bomb_load();
void bomb_unload();
SDL_Surface* sprite_get_monster(enum direction direction);
void monster_load();
void monster_unload();
SDL_Surface* sprite_get_aura_toxic();
void others_load();
void others_unload();

void menu_load();
void menu_unload();
SDL_Surface* sprite_get_menu_background();
SDL_Surface* sprite_get_menu_layout();
SDL_Surface* sprite_get_menu_dot();
SDL_Surface* sprite_get_menu_win();
SDL_Surface* sprite_get_menu_lose();

#endif /* SPRITE_H_ */
