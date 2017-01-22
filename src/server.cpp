#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>
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
std::vector<SDL_Texture*> textures;

const int MAX_THREAD = 200;
std::vector<flag_t> tempMesh[MAX_THREAD];

std::vector<human_t> humans;
std::vector <obsticle_t> roadblock;

SDL_Rect backgroundPos;
int backgroundTextureID;

std::vector<aoe_t> activeSpells;
std::vector<aoe_t> avalSpells;

int nextAvalHumanID = -1;
int nextAvalTextureID = -1;
int nextAvalThreadID = -1;
int playerID = 0;

int netIDTable[2000];

#include"move.cpp"
#include"load.cpp"

int main()
{
	worker_t threads[MAX_THREAD];

	SDLNet_Init();
	TCPsocket server;
	SDLNet_SocketSet allSockets = SDLNet_AllocSocketSet( 1000 );
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

	long long send=0, recievd = 0;
	char levelInfo[1000][300];
	int numLines = 0;

	human_t humanTemplate;

	int frames = 0;
	std::cout<<"Start"<<std::endl;
	while( true )
	{
		if( !levelLoaded )
		{
			
			ReadFromFile( level, levelInfo, numLines );
			
			LoadLevel( levelInfo, numLines );
			humanTemplate = humans[0];
			humans.clear();
			levelLoaded = true;
		}
		if( SDL_GetTicks() - checkNetT >= 0 )
		{
			int active = SDLNet_CheckSockets( allSockets, 0 );
			
			if( SDLNet_SocketReady( server ) )
			{
				TCPsocket newSocket = SDLNet_TCP_Accept( server );
				if( newSocket )
				{
					std::cout<<"New player"<<std::endl;
					active--;
					human_t newGuy = humanTemplate;
					newGuy.socket = newSocket;
					SDLNet_TCP_AddSocket( allSockets, newGuy.socket );
					newGuy.id = ++ nextAvalHumanID;
					newGuy.state = -9;
					newGuy.active = false;

					for( int i=0;i<numLines;i++ )
						SDLNet_TCP_Send( newGuy.socket, levelInfo[i], 300 );
					int endOfLevel[1] = { -1 };
					SDL_Delay(1000);
					SDLNet_TCP_Send( newGuy.socket, endOfLevel, 4 );
					SDL_Delay(200);

					int msg[humans.size()*3+1] = { newGuy.id };
					int msgLen = 1;
					for( int i=0;i<humans.size();i++ )
					{
						msg[msgLen] = humans[i].id;
						msg[msgLen+1] = humans[i].x;
						msg[msgLen+2] = humans[i].y;
						msgLen+=3;
					}
					Uint8 meta[2] = {1, (Uint8)msgLen};
					SDLNet_TCP_Send( newGuy.socket, meta, 2 );
					SDLNet_TCP_Send( newGuy.socket, msg, msgLen*4 );

					Uint8 othrmeta[2] = {2, 3};
					int othrmsg[3] = {newGuy.id, newGuy.x, newGuy.y};
					for( int i=0;i<humans.size();i++ )
					{
						SDLNet_TCP_Send( humans[i].socket, othrmeta, 2 );
						SDLNet_TCP_Send( humans[i].socket, othrmsg, 12 );
					}

					humans.push_back( newGuy );
				}
			}
			
			for( int i=0;i<humans.size() and active > 0;i++ )
			{
				if( SDLNet_SocketReady( humans[i].socket ) )
				{
					active--;
					char msg[4];
					int result = SDLNet_TCP_Recv( humans[i].socket, msg, 3 );
					if( result > 0 )
					{
						humans[i].movDirection[0] = msg[0];
						humans[i].movDirection[1] = msg[1];
						humans[i].attDirection = msg[2];
						humans[i].active = true;
					}else
					{
						Uint8 meta[2] = {4, 1};
						int msg[1] = {humans[i].id};
						std::swap( humans[i], humans[humans.size()-1] );
						humans.pop_back();
						for( int j=0;j<humans.size();j++ )
						{
							SDLNet_TCP_Send( humans[j].socket, meta, 2 );
							SDLNet_TCP_Send( humans[j].socket, msg, 4 );
						}
					}
				}
			}

			checkNetT = SDL_GetTicks();
		}
		
		if( SDL_GetTicks() - sendNetT >= 10 ) 
		{
			int msgH[300];
			int msgHLen = 0;
			for( int i=0;i<humans.size();i++ )
			{
				if( humans[i].active )
				{
					msgH[msgHLen] = humans[i].id;
					msgH[msgHLen+1] = humans[i].x;
					msgH[msgHLen+2] = humans[i].y;
					msgH[msgHLen+3] = humans[i].drawDirection;
					humans[i].active = false;
					msgHLen+=4;
				}
			}
			if( msgHLen > 0 )
			{
				Uint8 meta[2] = {3, (Uint8)msgHLen};
				for( int i=0;i<humans.size();i++ )
				{
					SDLNet_TCP_Send( humans[i].socket, meta, 2 );
					SDLNet_TCP_Send( humans[i].socket, msgH, msgHLen*4 );
				}
			}
			int msgA[300];
			int msgALen = 0;
			for( int i=0;i<activeSpells.size();i++ )
			{
				msgA[msgALen] = activeSpells[i].id;
				msgA[msgALen+1] = activeSpells[i].x;
				msgA[msgALen+2] = activeSpells[i].y;
				msgA[msgALen+3] = (int)activeSpells[i].angle;
				msgALen+=4;
			}
			if( msgALen > 0 )
			{
				Uint8 meta[2] = {5, (Uint8)msgALen};
				for( int i=0;i<humans.size();i++ )
				{
					SDLNet_TCP_Send( humans[i].socket, meta, 2 );
					SDLNet_TCP_Send( humans[i].socket, msgA, msgALen*4 );
				}
			}

			sendNetT = SDL_GetTicks();
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
				Move( humans[i], humans, roadblock );
			}
			movT = SDL_GetTicks();
		}
		
		if( SDL_GetTicks() - spellT >= 10 )
		{
			for( int i = 0;i<activeSpells.size();i++ )
				if( activeSpells[i].duration > 0 )
					Spell( activeSpells[i], humans, roadblock );
			for( int j = 0;j < humans.size(); j++ )
				for( int i = 0;i<avalSpells.size();i++ )
					if( humans[j].spellWaitTime[i] > 0 )
						humans[j].spellWaitTime[i]--;
			spellT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - infoT >= 800 )
		{
			for( int i=0;i<activeSpells.size();i++ )
				std::cout<<activeSpells[i].duration<<std::endl;
			//std::cout<<SDL_GetError()<<std::endl;
			//std::cout<<SDLNet_GetError()<<std::endl;
			infoT = SDL_GetTicks();
		}
		for( int i=0;i<activeSpells.size();i++ )
		{
			if( activeSpells[i].duration<=0 )
			{
				std::swap( activeSpells[i], avalSpells[ avalSpells.size()-1 ] );
				activeSpells.pop_back();
			}
		}
	}
}

/*
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
*/
