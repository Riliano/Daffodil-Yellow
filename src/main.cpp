#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_net.h>
//#include<SDL2/SDL_ttf.h>
#include<queue>
#include<vector>
#include<thread>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<time.h>

#include"simpleGeometry.h"
#include"struct.cpp"
#include"init.h"
#include"input.h"
#include"ai.cpp"

SDL_Renderer* renderer;
SDL_Event e;
std::vector<SDL_Texture*> textures;

//std::queue<int> tasks;
//int numThread = 2;
const int MAX_THREAD = 200;
std::vector<flag_t> tempMesh[MAX_THREAD];

//TTF_Font* gothic;

std::vector<human_t> humans;
std::vector <obsticle_t> roadblock;

SDL_Rect backgroundPos;
int backgroundTextureID;

std::vector<aoe_t> activeSpells;
std::vector<aoe_t> avalSpells;
const int NUM_SPELLS = 5;

float scale = 1;//(float)screenWidth/480.0;
int playerID = 0;
int nextAvalHumanID = -1;
int nextAvalTextureID = -1;
int nextAvalThreadID = -1;

int netIDTable[2000];

#include"move.cpp"
#include"load.cpp"

int main()
{
	//std::thread worker[numThread];
	worker_t threads[MAX_THREAD];
	InitInput();

	SDLNet_Init();
	TCPsocket sock;
	SDLNet_SocketSet chkNet = SDLNet_AllocSocketSet( 1 );
	IPaddress ip;
	char address[40];
	Uint16 port = 1234;
	std::cout<<"Enter address: ";
	std::cin>>address;
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

	textures.push_back( IMG_LoadTexture(renderer, "Textures/numbers.png") );
	textures.push_back( IMG_LoadTexture( renderer, "Textures/select.png" ) );
	int numbersTextureID = ++nextAvalTextureID;
	int selectTextureID = ++nextAvalTextureID;
	//gothic = TTF_OpenFont( "Fonts/MS Gothic.ttf", 20 );
	//backgroundPos = {0, 0, (int)scale*screenWidth, (int)scale*screenHeight};
	SDL_Texture* loading = IMG_LoadTexture( renderer, "Textures/loading.png" );
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
	while( true )
	{
		if( !levelLoaded )
		{
			SDL_RenderCopy( renderer, loading, NULL, NULL );
			SDL_RenderPresent( renderer );
			
			char levelInfo[1000][300];
			int numLines = 0;
			if( ignoreNet )
				ReadFromFile( level, levelInfo, numLines );
			else
				RecieveFromNet( sock, levelInfo, numLines );
			LoadLevel( levelInfo, numLines );

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
		if( SDL_GetTicks() - inputT >= 10 )
		{
			ScanKeyboard();
			AnalyzeInput( humans[playerID] );
			ResetKeyboard();

			inputT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - checkNetT >= 10 and !ignoreNet )
		{
			int active = SDLNet_CheckSockets( chkNet, 0 );
			if( active > 0 )
			{
				int rcv[1000];
				int r = SDLNet_TCP_Recv( sock, rcv, 1200 );
				recievd++;
				if( rcv[0] == 1 )
				{
					humans[playerID].netID = rcv[1];
					for( int i=2;rcv[i]!=-1;i+=3 )
					{
						human_t newGuy = humans[playerID];
						newGuy.netID = rcv[i];
						newGuy.state = -9;
						newGuy.id = ++nextAvalHumanID;
						newGuy.x = rcv[i+1];
						newGuy.y = rcv[i+2];
						netIDTable[newGuy.netID] = humans.size();
						humans.push_back( newGuy );
					}
					int msg[3] = {humans[playerID].netID, humans[playerID].x, humans[playerID].y};
					SDLNet_TCP_Send( sock, msg, 14 );
				}
				if( rcv[0] == 2 )
				{
					human_t newGuy = humans[playerID];
					newGuy.id = ++nextAvalHumanID;
					newGuy.netID = rcv[1];
					newGuy.state = -9;
					newGuy.x = rcv[2];
					newGuy.y = rcv[3];
					netIDTable[newGuy.netID] = humans.size();
					humans.push_back( newGuy );
				}
				if( rcv[0] == 3 )
				{
					for( int i=1;rcv[i]!=-1;i+=3 )
					{
						int updateThisGuy = netIDTable[rcv[i]];
						if( updateThisGuy != playerID )
						{
							humans[updateThisGuy].speed = 0;
							if( rcv[i+2] > humans[updateThisGuy].y )
								humans[updateThisGuy].movDirection[0] = 's';
							if( rcv[i+2] < humans[updateThisGuy].y )
								humans[updateThisGuy].movDirection[0] = 'n';
							if( rcv[i+2] == humans[updateThisGuy].y )
								humans[updateThisGuy].movDirection[0] = 0;

							if( rcv[i+1] > humans[updateThisGuy].x )
								humans[updateThisGuy].movDirection[1] = 'e';
							if( rcv[i+1] < humans[updateThisGuy].x )
								humans[updateThisGuy].movDirection[1] = 'w';
							if( rcv[i+1] == humans[updateThisGuy].x )
								humans[updateThisGuy].movDirection[1] = 0;
							humans[updateThisGuy].x = rcv[i+1];
							humans[updateThisGuy].y = rcv[i+2];
						}
					}
				}
				if( rcv[0] == 4 )
				{
					int guyToRemove = netIDTable[rcv[1]];
					std::swap( humans[guyToRemove], humans[humans.size()-1] );
					netIDTable[humans[guyToRemove].netID] = guyToRemove;
					humans.pop_back();
				}
			}
			checkNetT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - checkFlagsT >= 10 )
		{
			for( int i=0;i<humans.size();i++ )
			{
				if( humans[i].id != humans[playerID].id and threads[humans[i].threadID].done )
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
				if( i != playerID )
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

		if( SDL_GetTicks() - attT >= 1000/60 )
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
				Move( humans[i], humans, roadblock, followHuman );
			}
			movT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - sendNetT >= 1 and !ignoreNet and humans[playerID].netID != -1 and humans[playerID].active )
		{
			int info[4] = {humans[playerID].netID, humans[playerID].x, humans[playerID].y};
			SDLNet_TCP_Send( sock, info, 16 );
			send++;
			humans[playerID].active = false;
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
		if( SDL_GetTicks() - spellT >= 10 )
		{
			for( int i = 0;i<activeSpells.size();i++ )
				if( activeSpells[i].duration > 0 )
					Spell( activeSpells[i], humans, roadblock );
			for( int j = 0;j < humans.size(); j++ )
				for( int i = 0;i<NUM_SPELLS;i++ )
					if( humans[j].spellWaitTime[i] > 0 )
						humans[j].spellWaitTime[i]--;
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
	//		std::cout<<SDL_GetError()<<std::endl;
	//		std::cout<<SDLNet_GetError()<<std::endl;
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
			std::cout<<"Send: "<<send<<std::endl<<"Recieved: "<<recievd<<std::endl;
			send = 0;
			recievd = 0;
			frames = 0;
			fpsT = SDL_GetTicks();
		}
		SDL_RenderCopy( renderer, textures[backgroundTextureID], NULL, &backgroundPos );
		for( int i = 0;i<roadblock.size();i++ )
		{
			roadblock[i].pos.x = humans[playerID].pos.x - humans[playerID].x + roadblock[i].x + (roadblock[i].pos.w - roadblock[i].w)/2;
			roadblock[i].pos.y = humans[playerID].pos.y - humans[playerID].y + roadblock[i].y + (roadblock[i].pos.h - roadblock[i].h)/2;
			SDL_RenderCopy( renderer, textures[roadblock[i].textureID], &roadblock[i].frame, &roadblock[i].pos );
		}
		for( int i = 0;i<activeSpells.size();i++ )
		{
			activeSpells[i].pos.x = humans[playerID].pos.x - humans[playerID].x + activeSpells[i].x + (activeSpells[i].pos.w - activeSpells[i].w)/2;
			activeSpells[i].pos.y = humans[playerID].pos.y - humans[playerID].y + activeSpells[i].y + (activeSpells[i].pos.h - activeSpells[i].h)/2;
			SDL_RenderCopyEx( renderer, textures[ activeSpells[i].textureID ], &activeSpells[i].frame, &activeSpells[i].pos, activeSpells[i].angle, activeSpells[i].point, activeSpells[i].flip );
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
			SDL_RenderCopy( renderer, textures[humans[i].textureID], &humans[i].frame, &humans[i].pos );
		}
		if( ShouldIDisplayFPS() )
		{
			for( int i=0;i<sframes.size();i++ )
			{
				SDL_Rect nextNumber = {sframes[i]*10, 0, 10, 18};
				SDL_Rect nextNumberPos = {i*13 + 2, 5, 12, 20};
				SDL_RenderCopy( renderer, textures[numbersTextureID], &nextNumber, &nextNumberPos );
			}
			frames++;
		}
		for( int i=0;i<humans[playerID].avalSpells.size();i++ )
		{
			SDL_Rect frame = avalSpells[ humans[playerID].avalSpells[i] ].frame;
			int start = (screenWidth - 60*humans[playerID].avalSpells.size())/2;
			SDL_Rect pos = {start + i*60, screenHeight-70, 40, 40};
			if( humans[playerID].avalSpells[i] == humans[playerID].eqpSpell )
			{
				SDL_RenderCopy( renderer, textures[ selectTextureID ], NULL, &pos );
			}
			SDL_RenderCopy( renderer, textures[ avalSpells[ humans[playerID].avalSpells[i] ].textureID ], &frame, &pos );
		}
		if( humans[playerID].curHealth <= 0 )
		{
			std::cout<<"Game Over"<<std::endl;
			SDL_Delay( 1000 );
			levelLoaded = false;			
		}
		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
}
