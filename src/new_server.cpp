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
	TCPsocket server;
	SDLNet_SocketSet allSockets = SDLNet_AllocSocketSet( 1 );
	IPaddress ip;
	char address[40];
	Uint16 port = 1234;
	SDLNet_ResolveHost( &ip, NULL, port );
	server = SDLNet_TCP_Open( &ip );
	SDLNet_TCP_AddSocket( allSockets, server );
	
	char level[] = "Levels/1.lvl";
	bool levelLoaded = false;

	long long movT = SDL_GetTicks();
	long long infoT = SDL_GetTicks();
	long long attT = SDL_GetTicks();
	long long spellT = SDL_GetTicks();
	long long checkFlagsT = SDL_GetTicks();
	long long BOTpathFindT = SDL_GetTicks();
	long long BOTattackT = SDL_GetTicks();
	long long BOTvisionT = SDL_GetTicks();
	long long checkNetT = SDL_GetTicks();
	long long sendNetT = SDL_GetTicks();
	int spellchngTimeout = 0;

	long long send=0, recievd = 0;
	char levelInfo[1000][300];
	int numLines = 0;


	int frames = 0;
	std::vector<int> sframes;
	sframes.push_back(0);
	while( true )
	{
		if( !levelLoaded )
		{
			
			ReadFromFile( level, levelInfo, numLines );
			
			LoadLevel( levelInfo, numLines );
			levelLoaded = true;
		}
		if( SDL_GetTicks() - checkNetT >= 10 )
		{
			int active = SDLNet_CheckSockets( allSockets, 0 );
			
			if( SDLNet_SocketReady( server ) )
			{
				TCPsocket newSocket = SDLNet_TCP_Accept( server );
				if( newSocket )
				{
					std::cout<<"New player"<<std::endl;

					for( int i=0;i<numLines;i++ )
						SDLNet_TCP_Send( newSocket, levelInfo[i], 300 );
					char endOfLevel[3] = { '-', '1', '\0' };
					SDLNet_TCP_Send( newSocket, endOfLevel, 4 );
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
				bool followHuman = false;
				if( i == playerID )
					followHuman = true;
				Move( humans[i], humans, roadblock, followHuman );
			}
			movT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - sendNetT >= 1 ) 
		{
			sendNetT = SDL_GetTicks();
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
		if( SDL_GetTicks() - infoT >= 800 )
		{
			//std::cout<<SDL_GetError()<<std::endl;
			//std::cout<<SDLNet_GetError()<<std::endl;
			infoT = SDL_GetTicks();
		}
	}
}
