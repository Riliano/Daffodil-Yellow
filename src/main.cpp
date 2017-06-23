#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_net.h>
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

//SDL_Renderer* renderer;
/*
#include"simpleGeometry.h"
#include"struct.cpp"
#include"init.h"
#include"input.h"
#include"ai.cpp"
*/
#include "simpleGeometry.h"
#include "struct.cpp"

bool serverIsOn = false;
bool clientIsOn = false;

std::vector<human_t> humanTemplates;
std::vector<human_t> humans;
std::vector<obsticle_t> roadblockTemplates;
std::vector<obsticle_t> roadblock;

const int DEFAULT_PORT = 1234;
const int DEFAULT_SERVER_SIZE = 200;

#include "client.cpp"
#include "server.cpp"

/*
SDL_Event e;
std::vector<texture_t> textures;

//std::queue<int> tasks;
//int numThread = 2;
const int MAX_THREAD = 200;
std::vector<flag_t> tempMesh[MAX_THREAD];

//TTF_Font* gothic;

std::vector<human_t> humanTemplates;
std::vector<human_t> humans;
std::vector<obsticle_t> roadblockTemplates;
std::vector<obsticle_t> roadblock;

SDL_Rect backgroundPos;
SDL_Rect defBackgroundPos;
int backgroundTextureID;

std::vector<aoe_t> activeSpells;
std::vector<aoe_t> avalSpells;

float scale = 1;//(float)screenWidth/480.0;
int playerID = 0;
int nextAvalHumanID = -1;
int nextAvalTextureID = -1;
int nextAvalThreadID = -1;

int netIDTable[2000];
int roadblockIDTable[2000];
int humanIDTable[2000];
int textureIDTable[2000];
*/
//#include"move.cpp"
//#include"load.cpp"

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

	std::thread client;
	if( startClient )
		client = std::thread( ClientMain, address, port );
	if( startServer )
		ServerMain( port, serverSize );
	while( clientIsOn )
	{}
	client.join();
}

///OLD MAIN
/*
int main( int argc, char **argv )
{
	SDL_Init(0);
	//std::thread worker[numThread];
	worker_t threads[MAX_THREAD];
	InitInput();

	SDLNet_Init();
	TCPsocket sock;
	SDLNet_SocketSet chkNet = SDLNet_AllocSocketSet( 1 );
	IPaddress ip;
	char address[40];
	Uint16 port = 1234;
	if( argc < 2 )
	{
		std::cout<<"Enter address: ";
		std::cin>>address;
	}else
	{
		for( int i=0;argv[1][i]!='\0';i++ )
			address[i] = argv[1][i];
	}
	bool ignoreNet = false;
	if( address[0] == 'n' and address[1] == 'o' and address[2] == '\0' )
		ignoreNet = true;
	if( !ignoreNet )
	{
		SDLNet_ResolveHost( &ip, address, port );
		sock = SDLNet_TCP_Open( &ip );
		if( !sock )
		{
			ignoreNet = true;
			std::cout<<"Failed to connect"<<std::endl;
		}
		else
			SDLNet_TCP_AddSocket( chkNet, sock );
	}
	Init( renderer );
	//TTF_Init();
	if( ShouldITurnVSync() )
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	else
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

///UI textures
	texture_t numbers( "Textures/numbers.png" );//, ++nextAvalTextureID );
//	textures.push_back( numbers );
	texture_t select( "Textures/select.png" );//, ++nextAvalTextureID );
//	textures.push_back( select );
	texture_t loading( "Textures/loading.png" );

//	numbers.CreateFromInfo( "numbers", ++nextAvalTextureID );
//	texture_t select;
//	select.CreateFromInfo( "");
//	textures.push_back( IMG_LoadTexture(renderer, "Textures/numbers.png") );
//	textures.push_back( IMG_LoadTexture( renderer, "Textures/select.png" ) );
//	int numbersTextureID = numbers.id;
//	int selectTextureID = select.id;
	//gothic = TTF_OpenFont( "Fonts/MS Gothic.ttf", 20 );
	//backgroundPos = {0, 0, (int)scale*screenWidth, (int)scale*screenHeight};
//	SDL_Texture* loading = IMG_LoadTexture( renderer, "Textures/loading.png" );
	char level[] = "Levels/1.lvl";
	bool levelLoaded = false;

	long long movT = SDL_GetTicks();
	long long inputT = SDL_GetTicks();
//	long long drawT = SDL_GetTicks();
	long long infoT = SDL_GetTicks();
	long long fpsT = SDL_GetTicks();
	long long attT = SDL_GetTicks();
	long long spellT = SDL_GetTicks();
	long long checkFlagsT = SDL_GetTicks();
	long long drawSpellT = SDL_GetTicks();
	long long BOTpathFindT = SDL_GetTicks();
	long long BOTattackT = SDL_GetTicks();
	long long BOTvisionT = SDL_GetTicks();
	long long checkNetT = SDL_GetTicks();
	long long sendNetT = SDL_GetTicks();
	int spellchngTimeout = 0;

	long long send=0, recievd = 0;

	int frames = 0;
	std::vector<int> sframes;
	sframes.push_back(0);
	SDL_Rect pos = {0, 0, 500, 500}, frame = {0, 0, 500, 500};
	SDL_RenderCopy( renderer, loading.texture, &pos, &frame );
	while( true )
	{
		if( !levelLoaded )
		{
			SDL_RenderCopy( renderer, loading.texture, NULL, NULL );
			SDL_RenderPresent( renderer );
			if( ignoreNet )	
				LoadLevel( level, scale );
			
			std::cout<<roadblock.size()<<" "<<roadblockTemplates.size()<<" "<<humans.size()<<" "<<humanTemplates.size()<<std::endl;
			std::cout<<textures.size()<<std::endl;
			levelLoaded = true;
			SDL_RenderClear( renderer );
		}
		while( SDL_PollEvent(&e) )
		{
			if( e.type == SDL_QUIT )
			{
				SDL_Quit();
				for( int i=0;i<MAX_THREAD;i++ )
				{
					if( threads[i].trd.joinable() )
					{
						threads[i].quit = true;
						threads[i].trd.join();
					}
				}
				return 0;
			}
			
		}
		if( SDL_GetTicks() - inputT >= 10 and humans.size() > 0 )
		{
			ScanKeyboard();
			AnalyzeInput( humans[playerID] );
			ResetKeyboard();

			inputT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - checkNetT >= 0 and !ignoreNet )
		{
			int active = SDLNet_CheckSockets( chkNet, 0 );
			while( active > 0 )
			{
				Uint8 meta[2];
				SDLNet_TCP_Recv( sock, meta, 2 );
				int info[ meta[1] ];
				recievd++;
				int size;
				int recievd = 0;
				if( meta[0] == 0 )
				{
					char text[ meta[1] ];
					while(  recievd<meta[1] )
						recievd = SDLNet_TCP_Recv( sock, text+recievd, meta[1]-recievd );
					for( int i=0;i<meta[1];i++ )
						info[i] = text[i];
				}else
				{
					size = meta[1]*4;
					while(  recievd<size )
						recievd = SDLNet_TCP_Recv( sock, info+recievd, size-recievd );
				}
				if( meta[0] == 0 )
				{
					char newTextureName[310];
					int newTextureNameSize = 0;
					int numTexture = 0;
					char texturesIDontHave[ meta[1] ];
					int numTexturesIDontHave = 0;
					for( int i=0;i<meta[1];i++ )
					{
						newTextureName[newTextureNameSize] = info[i];
						newTextureNameSize++;
						if( (char)info[i] == '\0' )
						{
							texture_t newTexture( newTextureName, ++nextAvalTextureID );
							textureIDTable[numTexture] = nextAvalTextureID;
							if( newTexture.texture == NULL )
							{							
								//add to requested
								//std::cout<<"I dont have that"<<std::endl;
								texturesIDontHave[ numTexturesIDontHave ] = numTexture;
								numTexturesIDontHave++;
							}
							numTexture++;
							textures.push_back( newTexture );
							newTextureNameSize = 0;
						}
					}
					if( numTexturesIDontHave > 0 )
					{
						Uint8 myMeta[2] = {2, (Uint8)numTexturesIDontHave};
						SDLNet_TCP_Send( sock, myMeta, 2 );
						SDLNet_TCP_Send( sock, texturesIDontHave, numTexturesIDontHave );
					}
				}
				
				if( meta[0] == 1 )
				{
					humans.clear();
					human_t player;
					player.eqpSpell = -1;
					player.state = -1;
					player.id = ++nextAvalHumanID;
					player.normSpeed = 4;
					player.speed = player.normSpeed;
					player.netID = info[0];
					player.textureID = textureIDTable[ info[1] ];
					player.curHealth = 1;
					player.x = info[2];
					player.y = info[3];
					player.w = info[4];
					player.h = info[5];

					player.pos.x = info[6];
					player.pos.y = info[7];
					player.pos.w = info[8];
					player.pos.h = info[9];

					player.frame.x = info[10];
					player.frame.y = info[11];
					player.frame.w = info[12];
					player.frame.h = info[13];

					humans.push_back( player );

					for( int i=14;i<meta[1];i+=3 )
					{
						human_t newGuy = humans[playerID];
						newGuy.netID = info[i];
						newGuy.state = -9;
						newGuy.id = ++nextAvalHumanID;
						newGuy.x = info[i+1];
						newGuy.y = info[i+2];
						netIDTable[newGuy.netID] = humans.size();
						humans.push_back( newGuy );
					}
				}
				if( meta[0] == 2 )
				{
					for( int i=0;i<meta[1];i+=12 )
					{
						obsticle_t newRoadblock;
						newRoadblock.curHealth = 1;
						newRoadblock.id = info[i];
						roadblockIDTable[ newRoadblock.id ] = roadblock.size();
						newRoadblock.textureID = textureIDTable[ info[i+1] ];
						newRoadblock.x = info[i+2];
						newRoadblock.y = info[i+3];
						newRoadblock.w = info[i+4];
						newRoadblock.h = info[i+5];
						newRoadblock.pos.w = info[i+6];
						newRoadblock.pos.h = info[i+7];
						newRoadblock.frame.x = info[i+8];
						newRoadblock.frame.y = info[i+9];
						newRoadblock.frame.w = info[i+10];
						newRoadblock.frame.h = info[i+11];
						roadblockIDTable[newRoadblock.id] = roadblock.size();
						roadblock.push_back( newRoadblock );
					}
				}
				if( meta[0] == 3 )
				{
					for( int i=0;i<textures.size();i++ )
					{
						if( textures[i].texture == NULL )
						{
							std::cout<<"Getting: "<<textures[i].filename;
							std::ofstream file;
							file.open( textures[i].filename, std::ofstream::binary );
							int textureSize = info[0];
							char binTexture[textureSize];
							int fetched = 0;
							while( fetched < textureSize )
							{
								std::cout<<"Fetching: "<<fetched<<"/"<<textureSize<<std::endl;
								fetched = SDLNet_TCP_Recv( sock, binTexture+fetched, textureSize-fetched );
							}
							std::cout<<"Done"<<std::endl;
							for( int i=0;i<textureSize;i++ )
							{
								file<<binTexture[i];
								std::cout<<"Writing: "<<i<<"/"<<textureSize<<std::endl;
							}
							std::cout<<"Done"<<std::endl;
							file.close();
							textures[i].texture = IMG_LoadTexture( renderer, textures[i].filename );
							break;
						}
					}
				}
				if( meta[0] == 4 )
				{
					backgroundTextureID = textureIDTable[ info[0] ];
					backgroundPos.x = info[1];	
					backgroundPos.y = info[2];
					backgroundPos.w = info[3];
					backgroundPos.h = info[4];
					defBackgroundPos = backgroundPos;
				}
				if( meta[0] == 5 )
				{
					for( int i=0;i<meta[1];i+=9 )
					{
						aoe_t newAoe;
						newAoe.duration = 1;
						newAoe.speed = 1;
						newAoe.textureID = textureIDTable[ info[i] ];
						newAoe.w = info[i+1];
						newAoe.h = info[i+2]; 
						newAoe.pos.w = info[i+3];
						newAoe.pos.h = info[i+4];
						newAoe.frame.x = info[i+5];
						newAoe.frame.y = info[i+6];
						newAoe.frame.w = info[i+7];
						newAoe.frame.h = info[i+8];
						for( int i=0;i<humans.size();i++ )
							humans[i].spellWaitTime[ avalSpells.size() ] = 0;
						humans[playerID].avalSpells.push_back( avalSpells.size() );				
						avalSpells.push_back( newAoe );

					}
				}
				if( meta[0] == 9 )
				{
					human_t newGuy = humans[playerID];
					newGuy.id = ++nextAvalHumanID;
					newGuy.netID = info[0];
					newGuy.state = -9;
					newGuy.x = info[1];
					newGuy.y = info[2];
					netIDTable[newGuy.netID] = humans.size();
					humans.push_back( newGuy );
				}
				if( meta[0] == 10 )
				{
					bool updatedPlayer = false;
					for( int i=0;i<meta[1];i+=4 )
					{
						int updateThisGuy = netIDTable[info[i]];
						if( updateThisGuy == playerID )
						{
							updatedPlayer = true;
							int bgOffsetX = info[i+1] - humans[playerID].x;
							int bgOffsetY = info[i+2] - humans[playerID].y;
							if( ( bgOffsetX > 0 and humans[playerID].pos.x + bgOffsetX <= (screenWidth - humans[playerID].pos.w*8) )
							 or ( bgOffsetX < 0 and humans[playerID].pos.x + bgOffsetX >= humans[playerID].pos.w*7 ) )
								humans[playerID].pos.x+=bgOffsetX;
							else
								backgroundPos.x-=bgOffsetX;
							if( ( bgOffsetY > 0 and  humans[playerID].pos.y + bgOffsetY <= (screenHeight - humans[playerID].pos.h*6) )
							 or ( bgOffsetY < 0 and humans[playerID].pos.y + bgOffsetY >= humans[playerID].pos.h*5 ) )
								humans[playerID].pos.y+=bgOffsetY;
							else
								backgroundPos.y-=bgOffsetY;

						}
						humans[updateThisGuy].x = info[i+1];
						humans[updateThisGuy].y = info[i+2];
						humans[updateThisGuy].drawDirection = info[i+3];
					}
					if( !updatedPlayer )
						humans[playerID].drawDirection = 0;
				}

				if( meta[0] == 12 )
				{
					activeSpells.clear();
					for( int i=0;i<meta[1];i+=4 )
					{
						aoe_t newAttack = avalSpells[info[i]];
						newAttack.speed = 0;
						newAttack.dmg = 0;
						newAttack.duration = 1;
						newAttack.x = info[i+1];
						newAttack.y = info[i+2];
						newAttack.angle = (float)info[i+3];
						activeSpells.push_back( newAttack );
					}
				}

				if( meta[0] == 13 )
				{
					for( int i=0;i<meta[1];i++ )
					{
						int roadblockToRemove = roadblockIDTable[ info[i] ];
						roadblockIDTable[ roadblock[roadblock.size()-1].id ] = roadblockToRemove;
						std::swap( roadblock[ roadblockToRemove ], roadblock[roadblock.size()-1] );
						roadblock.pop_back();
					}
				}

				if( meta[0] == 20 )
				{
					for( int i=0;i<meta[1];i+=5 )
					{
						int deadGuy = netIDTable[info[i]];
						if( deadGuy == playerID )
						{
							std::cout<<"You have died"<<std::endl;
							backgroundPos = defBackgroundPos;
							humans[deadGuy].pos.x = info[i+3];
							humans[deadGuy].pos.y = info[i+4];
						}
						humans[ deadGuy ].x = info[i+1];
						humans[ deadGuy ].y = info[i+2];
					}

				}

				active = SDLNet_CheckSockets( chkNet, 0 );
			}
			checkNetT = SDL_GetTicks();
		}
		if( ignoreNet )
		{
			if( SDL_GetTicks() - checkFlagsT >= 10 )
			{
				for( int i=0;i<humans.size();i++ )
				{
					if( humans[i].id != humans[playerID].id and threads[humans[i].threadID].done and humans[i].state > 0 )
					{
						for( int j=0;j<tempMesh[humans[i].threadID].size();j++ )
							humans[i].navMesh.push_back( tempMesh[humans[i].threadID][j] );

						for( int j=0;j<humans[i].navMesh.size();j++ )
						{
							if( humans[i].x == humans[i].navMesh[j].x and humans[i].y == humans[i].navMesh[j].y )
							{
								humans[i].movDirection[0] = humans[i].navMesh[j].goTo[0];
								humans[i].movDirection[1] = humans[i].navMesh[j].goTo[1];
								break;
							}
						}
						if( Distance( humans[i].x, humans[i].y, humans[i].patrolPoint[humans[i].patrolCycle].x, humans[i].patrolPoint[humans[i].patrolCycle].y ) <= 40 )
							humans[i].patrolCycle+=humans[i].switchToNextPoint;
						if( humans[i].cycle )
						{
							if( humans[i].patrolCycle >= humans[i].patrolPoint.size() )
								humans[i].patrolCycle = 0;
						}else
						{
							if( humans[i].patrolCycle >= humans[i].patrolPoint.size()-1 )
								humans[i].switchToNextPoint = -1;
							if( humans[i].patrolCycle <= 0 )
								humans[i].switchToNextPoint = 1;
						}
					
					}
				}
				checkFlagsT = SDL_GetTicks();
			}
			if( SDL_GetTicks() - BOTpathFindT >= 400 )
			{	
				for( int i=0;i<humans.size();i++ )
				{
					if( humans[i].state >= 1 and humans[i].id != humans[playerID].id )
					{
						if( threads[humans[i].threadID].done )
						{
							if( humans[i].state == 1 )
							{
								humans[i].targetX = humans[i].patrolPoint[ humans[i].patrolCycle ].x;
								humans[i].targetY = humans[i].patrolPoint[ humans[i].patrolCycle ].y;
							}
							if( humans[i].state == 2 )
							{
								for( int j=0;j<humans.size();j++ )
								{
									if( humans[i].targetID == humans[j].id )
									{
										humans[i].targetX = humans[j].x;
										humans[i].targetY = humans[j].y;
										break;
									}
								}
							}
							humans[i].navMesh.clear();
							tempMesh[humans[i].threadID].clear();
							threads[humans[i].threadID].done = false;
							if( threads[humans[i].threadID].trd.joinable() )
								threads[humans[i].threadID].trd.join();
							threads[humans[i].threadID].trd = std::thread( PathBuilder, &humans[i], &tempMesh[humans[i].threadID], humans, roadblock, &threads[humans[i].threadID].done, &threads[humans[i].threadID].quit );
						}
					}
					
				}
				BOTpathFindT = SDL_GetTicks();
			}
			if( SDL_GetTicks() - BOTvisionT >= 10 )
			{
				for( int i=0;i<humans.size();i++ )
					if( i != playerID and humans[i].state > 0 )
						CheckVision( humans[i], humans );
				BOTvisionT = SDL_GetTicks();
			}
			if( SDL_GetTicks() - BOTattackT >= 10 )
			{
				for( int i=0;i<humans.size();i++ )
				{
					if( i!=playerID and humans[i].state == 2 )
					{
							if( (avalSpells[humans[i].eqpSpell].speed == 0 and Distance( humans[i].x, humans[i].y, humans[i].targetX, humans[i].targetY ) < 50 )or avalSpells[humans[i].eqpSpell].speed != 0 )
								humans[i].attDirection = humans[i].prevDrawDirection;
					}
				}
				BOTattackT = SDL_GetTicks();
			}
		}
		if( SDL_GetTicks() - attT >= 1000/60 and ignoreNet )
		{
			for( int i = 0;i<humans.size();i++ )
				Attack( humans[i] );
			attT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - movT >= 1000/60 )
		{
			for( int i=0;i<humans.size();i++ )
			{
				humans[i].DrawDir();
				bool followHuman = false;
				if( i == playerID )
					followHuman = true;
				if( ignoreNet )
					Move( humans[i], humans, roadblock, followHuman );
			}
			movT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - sendNetT >= 10 and !ignoreNet )
		{
			if( humans.size() > 0 )
			{
				if( humans[playerID].active )
				{
					Uint8 meta[2] = {1, 4};
					SDLNet_TCP_Send( sock, meta, 2 );
					char info[4] = {humans[playerID].movDirection[0], humans[playerID].movDirection[1], humans[playerID].attDirection, (char)humans[playerID].eqpSpell};
					SDLNet_TCP_Send( sock, info, 4 );
					humans[playerID].movDirection[0] = 0;
					humans[playerID].movDirection[1] = 0;
					humans[playerID].attDirection = 0;
					send++;
					humans[playerID].active = false;
				}
			}
			sendNetT = SDL_GetTicks();
		}
		for( int i=0;i<humans.size();i++ )
		{
			if( SDL_GetTicks() - humans[i].drawT >= 1000/(humans[i].speed+1) )
			{
				humans[i].Draw();
				humans[i].drawT = SDL_GetTicks();
			}
		}
		if( SDL_GetTicks() - spellT >= 10 and ignoreNet )
		{

			for( int i = 0;i<activeSpells.size();i++ )
			{
				if( activeSpells[i].duration > 0 )
				{
					std::vector<int> destroyedHumans, destroyedRoadblocks;
					Spell( activeSpells[i], humans, roadblock, destroyedHumans, destroyedRoadblocks );
					for( int j=0;j<destroyedRoadblocks.size();j++ )
					{
						std::swap( roadblock[ destroyedRoadblocks[j] ], roadblock[roadblock.size()-1] );
						roadblock.pop_back();
					}
					for( int j=0;j<destroyedHumans.size();j++ )
					{
						if( destroyedHumans[j] == playerID )
							std::cout<<"Game Over"<<std::endl;
						std::swap( humans[ destroyedHumans[j] ], humans[humans.size()-1] );
						humans.pop_back();
					}
				}
			}
			for( int j = 0;j < humans.size(); j++ )
			{
				for( int i = 0;i<avalSpells.size();i++ )
				{
					if( humans[j].spellWaitTime[i] > 0 )
					{
						humans[j].spellWaitTime[i]--;
					}
				}
			}
			spellT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - drawSpellT >= 1000/15 )
		{
			for( int i = 0;i<activeSpells.size();i++ )
				activeSpells[i].Draw();
			drawSpellT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - infoT >= 800 )
		{
#ifdef SDL_GETERROR 
			std::cout<<SDL_GetError()<<std::endl;
#endif
		//	std::cout<<SDLNet_GetError()<<std::endl;
			infoT = SDL_GetTicks();
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
	//		std::cout<<"Send: "<<send<<std::endl<<"Recieved: "<<recievd<<std::endl;
			send = 0;
			recievd = 0;
			frames = 0;
			fpsT = SDL_GetTicks();
		}

		SDL_RenderCopy( renderer, textures[backgroundTextureID].texture, NULL, &backgroundPos );
		for( int i = 0;i<roadblock.size();i++ )
		{
			roadblock[i].pos.x = humans[playerID].pos.x - humans[playerID].x + roadblock[i].x;// + (roadblock[i].pos.w - roadblock[i].w)/2;
			roadblock[i].pos.y = humans[playerID].pos.y - humans[playerID].y + roadblock[i].y;// + (roadblock[i].pos.h - roadblock[i].h)/2;
			SDL_RenderCopy( renderer, textures[roadblock[i].textureID].texture, &roadblock[i].frame, &roadblock[i].pos );
		}
		for( int i = 0;i<activeSpells.size();i++ )
		{
			activeSpells[i].pos.x = humans[playerID].pos.x - humans[playerID].x + activeSpells[i].x;// + (activeSpells[i].pos.w - activeSpells[i].w)/2;
			activeSpells[i].pos.y = humans[playerID].pos.y - humans[playerID].y + activeSpells[i].y;// + (activeSpells[i].pos.h - activeSpells[i].h)/2;
			SDL_RenderCopyEx( renderer, textures[ activeSpells[i].textureID ].texture, &activeSpells[i].frame, &activeSpells[i].pos, activeSpells[i].angle, activeSpells[i].point, activeSpells[i].flip );
			if( activeSpells[i].duration <= -5 )
			{
				std::swap( activeSpells[i], activeSpells[activeSpells.size()-1] );
				activeSpells.pop_back();	
			}
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
		if( humans.size() > 0 )
		{
			for( int i=0;i<humans[playerID].avalSpells.size();i++ )
			{
				SDL_Rect frame = avalSpells[ humans[playerID].avalSpells[i] ].frame;
				int start = (screenWidth - 60*humans[playerID].avalSpells.size())/2;
				SDL_Rect pos = {start + i*60, screenHeight-70, 40, 40};
				if( humans[playerID].avalSpells[i] == humans[playerID].eqpSpell )
				{
					SDL_RenderCopy( renderer, select.texture, NULL, &pos );
				}
				SDL_RenderCopy( renderer, textures[ avalSpells[ humans[playerID].avalSpells[i] ].textureID ].texture, &frame, &pos );
			}
		}

		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
}
*/
