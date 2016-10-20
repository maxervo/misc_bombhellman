#include <SDL/SDL_image.h> // IMG_Load
#include <window.h>
#include <assert.h>

#include <misc.h>

void error(const char *s, ...) {
	va_list ap;
	va_start(ap, s);

	vfprintf(stderr, s, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

SDL_Surface* load_image(const char* filename) {
	SDL_Surface* img = IMG_Load(filename);
	if (!img)
		error("IMG_Load: %s\n", IMG_GetError());

	return img;
}

int presence_array(int x, int *array, int len)
{
	for(int i=0; i<len; i++)
	{
		if(x == array[i]) {
			return 1;
		}
	}
	return 0;
}

void copy_file(char *src_path, char *dst_path)			//copy_file() overwrites if files already present : OK
{
	FILE *src = fopen(src_path, "r");
	FILE *dst = fopen(dst_path, "w");
	char c;

	assert(src); assert(dst);

	while( (c = fgetc(src)) != EOF ) {
		fputc(c, dst);
	}

	fclose(src); fclose(dst);
}

//Screen Effects Misc

void screen_effect(char* layer_name, int transp)
{
	char path_to_layer[50]; sprintf(path_to_layer, "sprite/%s.bmp", layer_name);
	SDL_Surface *layer = load_image(path_to_layer);

	for(int i = 0; i < MAP_WIDTH; i++) {
		for(int j = 0; j < MAP_HEIGHT; j++) {
			window_display_image_transparent(layer, i * SIZE_BLOC, j * SIZE_BLOC, transp);			
		}
	}

	SDL_FreeSurface(layer);
}
