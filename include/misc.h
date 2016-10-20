#ifndef MISC_H_
#define MISC_H_

#include <SDL/SDL.h>
#include <constant.h>

// Print an error message on stderr and exit
void error(const char *s, ...);

// Load an image, raise an error in case of failure
SDL_Surface* load_image(const char *filename);

//NEW
int presence_array(int x, int *array, int len);
void copy_file(char *src_path, char *dst_path);
void screen_effect(char* layer_name, int transp);

#endif /* MISC_H_ */
