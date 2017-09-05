#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_net.h>
#include<openssl/sha.h>
//#include<SDL2/SDL_ttf.h>
#include<queue>
#include<stack>
#include<vector>
#include<thread>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<time.h>
#include<string>
#include<cstring>
#include<sstream>

//#include "struct.cpp"
//#include "msgMeta.cpp"
//#include "simpleGeometry.h"

bool serverIsOn = false;
bool clientIsOn = false;

//std::vector<humanTemplate_t> humanTemplates;
//std::vector<int> playableTemplates;
//std::vector<obsticle_t> objectTemplates;
//std::vector<texture_t> textures;

//const int DEFAULT_PORT = 1234;
//const int DEFAULT_SERVER_SIZE = 200;

//#include "client.cpp"
#include "server.cpp"

int StringToInt( char* str )
{
	int num = 0;
	for( int i=0;str[i] != '\0' and str[i] != ' ';i++ )
	{
		num = num*10 + str[i]-'0';
	}
	return num;
}

int main( int argc, char **argv )
{
	bool startClient = true;
	bool startServer = true;
	Uint16 port = DEFAULT_PORT;
	int serverSize = DEFAULT_SERVER_SIZE;
	char address[100] = "localhost";
	char textures[500] = "textures.list";
	for( int i=1;i<argc;i++ )
	{
		if( std::strcmp( argv[i], "-p" ) == 0 )
		{
			port = (Uint16) StringToInt( argv[i+1] );
			i++;
		}
		if( std::strcmp( argv[i], "-a" ) == 0 )
		{
			int j;
			for( j=0;argv[i+1][j]!=' ' and argv[i+1][j]!='\0';j++ )
				address[j] = argv[i+1][j];
			address[j] = '\0';
			i++;
			startServer = false;
		}
		if( std::strcmp( argv[i], "--no-server" ) == 0 )
			startServer = false;

		if( std::strcmp( argv[i], "--no-client" ) == 0 )
			startClient = false;
	}

//	std::thread client;
//	if( startClient )
//		client = std::thread( ClientMain, address, port, textures );
//	if( startServer )
		ServerMain( port, serverSize );
	while( clientIsOn )
	{}
//	client.join();
	SDLNet_Quit();
	return 0;
}
