#include"init.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

SDL_Renderer* renderer;

SDL_Event e;

int main()
{
	init(renderer);
	if(ShouldITurnVSync())
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}else
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}
	SDL_Texture* asskey = IMG_LoadTexture(renderer, "AssignKey.png");
	SDL_Texture* keyassed = IMG_LoadTexture(renderer, "KeyAssigned.png");
	SDL_Texture* keypressed = IMG_LoadTexture(renderer, "KeyPressed.png");
	SDL_Texture* display = asskey;
	SDL_Rect pos;
	pos.x = 0;
	pos.y = 0;
	pos.w = screen_width;
	pos.h = screen_height;
	int key = 0;
	const Uint8* state = SDL_GetKeyboardState(NULL);
	while(true)
	{
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
			if(key==0)
			{
				for(int i = 1;i<=282;i++)
				{
					if(state[i])
					{
						key = i;
						display = keyassed;
					}
				}
			}
			else
			{
				if(state[key])
					display = keypressed;
				else
					display = keyassed;
			}
		}
		
		SDL_RenderCopy(renderer, display, NULL, &pos);
		SDL_RenderPresent(renderer);
	}
	SDL_Quit();
	return 0;
}
