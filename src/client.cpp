#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_net.h>

SDL_Renderer* renderer;
SDL_Event e;

TCPsocket client;
SDLNet_SocketSet chkClient = SDLNet_AllocSocketSet( 1 );
bool ConnectToServer( const char* address = "localhost", Uint16 port = DEFAULT_PORT )
{
	IPaddress ip;
    std::cout<<"Connecting to: "<<address<<" on port "<<port<<std::endl;
    SDLNet_ResolveHost( &ip, address, port );	
    client = SDLNet_TCP_Open( &ip );
    if( !client )
	{
        std::cout<<"Failed to connect"<<std::endl;
		return false;
	}
    else
	{
        SDLNet_TCP_AddSocket( chkClient, client );
		return true;
	}
}

#include "init.h"
#include "input.h"

//std::vector<texture_t> textures;
void LoadTextures( texture_t &txtr )
{
	txtr.texture = IMG_LoadTexture( renderer, txtr.filename );
	free( txtr.binaryTexture);
}
/*
std::vector<human_t> humanTemplates;
std::vector<human_t> humans;
std::vector<obsticle_t> roadblockTemplates;
std::vector<obsticle_t> roadblock;
*/
int humanIDTable[2000];
int roadblockIDTable[4000];
int textureIDTable[1000];

SDL_Rect backgroundPos;
SDL_Rect defBackgroundPos;
texture_t background;
float scale = 1;

int netIDTable[2000];
int roadblockIDTable[2000];
int humanIDTable[2000];
int textureIDTable[2000];

int playerID = -1;

void GetMessage();

void ClientMain( const char* address = "localhost", Uint16 port = DEFAULT_PORT )
{
	clientIsOn = true;
    SDL_Init( 0 );
    InitInput();
    InitWindow();
    if( ShouldITurnVSync() )
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	else
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

	if( address == "localhost" )
		while( !serverIsOn ){}
    bool connection = ConnectToServer( address, port );
	if( !connection )
	{
		clientIsOn = false;
		SDL_Quit();
		SDL_DestroyWindow( window );
		return;
	}

	std::cout<<"Getting info from server"<<std::endl;
	while( playerID == -1 )
	{
		int active = SDLNet_CheckSockets( chkClient, 0 );
		while( active > 0 )
		{
			GetMessage();
			active = SDLNet_CheckSockets( chkClient, 0 );
		}
	}

	texture_t numbers( "Textures/numbers.png" );
	LoadTextures( numbers );
	for( int i=0;i<textures.size();i++ )
		LoadTextures( textures[i] );

	long long inputT = SDL_GetTicks();
	long long infoT = SDL_GetTicks();
	long long fpsT = SDL_GetTicks();
	long long drawSpellT = SDL_GetTicks();
	long long checkNetT = SDL_GetTicks();
    long long sendNetT = SDL_GetTicks();

    int frames;
    std::vector<int> sframes;
	sframes.push_back(0);

	std::cout<<"Client: Entering main loop"<<std::endl;
    while( true )
    {
        while( SDL_PollEvent( &e ) )
        {
            if( e.type == SDL_QUIT )
            {
                SDL_Quit();
				SDL_DestroyWindow( window );
				std::cout<<"Closing Client"<<std::endl;
				clientIsOn = false;
                return;
            }
        }
        if( SDL_GetTicks() - inputT >= 10 and humans.size() > 0 )
		{
			ScanKeyboard();
			AnalyzeInput( humans[playerID] );
			ResetKeyboard();

			inputT = SDL_GetTicks();
		}

        if( SDL_GetTicks() - sendNetT >= 10 )
		{
			if( humans.size() > 0 )
			{
				if( humans[playerID].active )
				{
					Uint8 meta[2] = {20, 4};
					SDLNet_TCP_Send( client, meta, 2 );
					char info[4] = {humans[playerID].movDirection[0], humans[playerID].movDirection[1], humans[playerID].attDirection, (char)humans[playerID].eqpSpell};
					SDLNet_TCP_Send( client, info, 4 );
					humans[playerID].movDirection[0] = 0;
					humans[playerID].movDirection[1] = 0;
					humans[playerID].attDirection = 0;
					humans[playerID].active = false;
				}
			}
			sendNetT = SDL_GetTicks();
		}

        if( SDL_GetTicks() - fpsT >= 1000 and ShouldIDisplayFPS() )
		{
			//std::cout<<"FPS: "<<frames<<std::endl;
			std::vector<int> bucket;
			sframes.clear();
			while( frames > 0 )
			{
				bucket.push_back( frames%10 );
				frames/=10;
			}
			for( int i=0;bucket.size()!=0;i++ )
			{
				sframes.push_back(bucket.back());
				bucket.pop_back();
			}
			frames = 0;
			fpsT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - infoT >= 1000 )
		{
			//std::cout<<SDL_GetError()<<std::endl;
			infoT = SDL_GetTicks();
		}

		if( background.texture != NULL )
	    	SDL_RenderCopy( renderer, background.texture, NULL, &backgroundPos );
		for( int i = 0;i<roadblock.size();i++ )
		{
			roadblock[i].pos.x = humans[playerID].pos.x - humans[playerID].x + roadblock[i].x;// + (roadblock[i].pos.w - roadblock[i].w)/2;
			roadblock[i].pos.y = humans[playerID].pos.y - humans[playerID].y + roadblock[i].y;// + (roadblock[i].pos.h - roadblock[i].h)/2;
			SDL_RenderCopy( renderer, textures[roadblock[i].textureID].texture, &roadblock[i].frame, &roadblock[i].pos );
		}
        for( int i = 0;i<humans.size();i++ )
		{
			if( i!=playerID )
			{
				humans[i].pos.x = humans[playerID].pos.x - humans[playerID].x + humans[i].x;// + (humans[i].pos.w - humans[i].w)/2;
				humans[i].pos.y = humans[playerID].pos.y - humans[playerID].y + humans[i].y;// + (humans[i].pos.h - humans[i].h)/2;
			}
			SDL_RenderCopy( renderer, textures[humans[i].textureID].texture, &humans[i].frame, &humans[i].pos );
		}
		
		if( ShouldIDisplayFPS() )
		{
			for( int i=0;i<sframes.size();i++ )
			{
				SDL_Rect nextNumber = {sframes[i]*10, 0, 10, 18};
				SDL_Rect nextNumberPos = {i*13 + 2, 5, 12, 20};
				SDL_RenderCopy( renderer, numbers.texture, &nextNumber, &nextNumberPos );
			}
			frames++;
		}

        SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
    }
}

void GetMessage()
{
	Uint8 meta[2];
	SDLNet_TCP_Recv( client, meta, 2 );
	//It will recieve either chars or ints
//	bool intMessage = true;
//	if( meta[0] == 1 or meta[0] == 10 )// texture hash or a texture
//		intMessage = false;

	int message[ meta[1] ];

	int recieved = 0;
	while( recieve < meta[1] )
		recived = SDLNet_TCP_Recv( client, message+recived, meta[1]-recived );
	
	// Recieved ID
	if( meta[0] == 0 )
	{
		playerID = message[0];
	}
	// Recived list of textures
	if( meta[0] == 1 )
	{

	}
	// Recived human templates
	if( meta[0] == 2 )
	{

	}
	/*
	// Recived rest of human templates
	if( meta[0] == 3 )
	{

	}
	*/
	// Recived info on currently connected players
	if( meta[0] == 4 )
	{

	}
	// Recived roadblock templates
	if( meta[0] == 5 )
	{

	}
	// Recieve loaded roadblocks
	if( meta[0] == 6 )
	{

	}
	// Recieve texture
	if( meta[0] == 10 )
	{

	}
	// Update position of humans
	if( meta[0] == 20 )
	{
		for( int i=0;i<meta[1];i+=3 )
		{
			humans[ humanIDTable[ message[i] ] ].x = message[i+1];
			humans[ humanIDTable[ message[i] ] ].y = message[i+2];
		}
	}
	// Remove human
	if( meta[0] == 21 )
	{

	}
	// New human
	if( meta[0] == 22 )
	{

	}
	// Remove roadblock
	if( meta[0] == 30 )
	{

	}

}
