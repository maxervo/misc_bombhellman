#include <SDL/SDL_image.h> // IMG_Load
#include <assert.h>

#include <misc.h>

SDL_Surface* window;

void window_create(int width, int height) {
	assert(width > 0 && height > 0);

	SDL_WM_SetCaption(WINDOW_NAME, NULL );
	window = SDL_SetVideoMode(width, height, 0, // If bits-per-pixel is 0, it is treated as the current display bits per pixel.
			SDL_HWSURFACE);

	if (window == NULL ) {
		error("Can't set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

void window_display_image(SDL_Surface* sprite, int x, int y) {
	assert(window);
	assert(sprite);

	SDL_Rect place;
	place.x = x;
	place.y = y;

	SDL_BlitSurface(sprite, NULL, window, &place);
}

void window_clear() {
	assert(window);
	SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 30, 35, 35));	//instead of white 255, 255, 255
}

void window_refresh() {
	assert(window);
	SDL_Flip(window);
}

//NEW

//Other solution : make a .png sprite with already alpha transparency, then load it as usual;
//To think of: here need to add free() function for this transparent sdl_surface (individually at end of this function (possible affects spead, load then free everytime, affects performance, And when loading bitmap, redoing bypassing even if not necessary; Here OK for the moment cuz procedure used only once in a while) or possible to integrate it in sprite_unload() )
void window_display_image_transparent(SDL_Surface* sprite, int x, int y, int transp) 	//often: sprite is in png already loaded with IMG_Load(...)		
{
	assert(window);
	assert(sprite);

	SDL_Rect place;
	place.x = x;
	place.y = y;

	//Removing Alpha Channel of .png sprite : bypassing through bitmap, then Alpha
	SDL_Surface* transp_sprite = SDL_DisplayFormat(sprite);
	SDL_SetColorKey(transp_sprite, SDL_SRCCOLORKEY, SDL_MapRGB(transp_sprite->format, 0, 0, 0));
	SDL_SetAlpha(transp_sprite, SDL_SRCALPHA | SDL_RLEACCEL, transp);

	SDL_BlitSurface(transp_sprite, NULL, window, &place);
	SDL_FreeSurface(transp_sprite);
}
