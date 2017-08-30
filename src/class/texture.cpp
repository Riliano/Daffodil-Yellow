#include "../class.hpp"
#include <SDL2/SDL.h>

struct textureBin_t
{
	public:
		int id;
		unsigned char* bin;
		unsigned int size;
		unsigned char* hash;
};

struct texture_t
{
	public:
		int id;
		SDL_Texture *texture;
};
