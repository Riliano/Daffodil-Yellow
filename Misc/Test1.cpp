#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"init.h"
#include<iostream>

SDL_Renderer* renderer;
SDL_Event e;

SDL_Texture* looks;

struct human
{
//	SDL_Texture* looks;
	SDL_Rect pos;
	SDL_Rect frame;
};

human person[100];

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
	int j = 0;
	int p = 0;
	looks = IMG_LoadTexture(renderer, "RedMage.png");
	for(int i = 0;i<100;i++)
	{
		person[i].pos.x = j*32;
		if(j==10)
		{
			j=0;
			p+=32;
		}
		j++;
		person[i].pos.y = p;
		person[i].pos.w = 32;
		person[i].pos.h = 32;
		person[i].frame.x = 0;
		person[i].frame.y = 0;
		person[i].frame.w = 32;
		person[i].frame.h = 32;
	}
	for(int i = 0;i<100;i++)
	{
		SDL_RenderCopy(renderer, looks, &person[i].frame, &person[i].pos);
		std::cout<<person[i].pos.x<<std::endl;
		SDL_RenderPresent(renderer);
	}
	while(true)
	{
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
		}
	}
}
