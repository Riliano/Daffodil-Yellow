#include<iostream>
#include<fstream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include"init.h"
#include"struct.h"

SDL_Renderer* renderer;
SDL_Event e;

int main()
{
	init(renderer);
	if(ShouldITurnVSync())
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}
	else
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}
}
