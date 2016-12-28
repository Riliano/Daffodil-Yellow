#include<fstream>
#include<string>
#include<iostream>
#include<SDL2/SDL.h>

const char PROJECT_NAME[] = "Daffodil Yellow";

SDL_Window* window;

bool vSync = false;
bool fullScreen = false;
bool fps = false;
int screenWidth = 640;
int screenHeight = 480;
const char CFG_FILE[] = "Configs/screen.cfg";
const int CFG_LINE = 5;

bool ShouldITurnVSync()
{
	return vSync;
}

bool ShouldIDisplayFPS()
{
	return fps;
}

//std::string config[CFG_LINE];
std::vector<std::string> config;

void ReadConfig()
{
	std::ifstream cfg (CFG_FILE);
	if( cfg.is_open() )
	{
		std::string temp;
		while( std::getline(cfg, temp, '\n') )
			config.push_back(temp);
		cfg.close();
	}else
	{
		std::cout<<"Error opening screen.cfg file: using default settings"<<std::endl;
	}
}

void Init( SDL_Renderer* renderer )
{

	ReadConfig();
	
	for( int i = 0;i<config.size();i++ )
	{
		if( config[i][0] != 'R' )
		{
			bool temp;
			if( config[i][config[i].length()-1] == 'f' )
				temp = false;
			else
				temp = true;
			if( config[i][0] == 'V' )
				vSync = temp;
			if( config[i][0] == 'F' and config[i][1] == 'u' )
				fullScreen = temp;
			if( config[i][0] == 'F' and config[i][1] == 'P' )
				fps = temp;
		}else
		{
			bool foundX = false;
			int numberPosition = 1;
			screenHeight = 0;
			screenWidth = 0;
			for( int j = config[i].length() - 1; config[i][j]!=' ';j-- )
			{
				if( config[i][j]=='x' )
				{
					foundX = true;
					numberPosition = 1;
				}
				else
				{
					if( !foundX )
						screenHeight += ( config[i][j]-'0' )*numberPosition;
					else
						screenWidth += ( config[i][j]-'0' )*numberPosition;
					numberPosition = numberPosition*10;
				}
			}
		}
	}



	std::cout<<"Vsync: "<<vSync<<std::endl;;
	std::cout<<"FullScreen: "<<fullScreen<<std::endl;
	std::cout<<"FPS: "<<fps<<std::endl;
	std::cout<<"Res: "<<screenWidth<<" x "<<screenHeight<<std::endl;

	window = SDL_CreateWindow( PROJECT_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN );
	if( fullScreen )
		SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN );
}
