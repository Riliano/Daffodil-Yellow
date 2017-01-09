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

void Spell( aoe_t &magic )
{
	switch( magic.dir )
	{
		case 0 : break;
		case 'n' : magic.y-=magic.speed;break;
		case 's' : magic.y+=magic.speed;break;
		case 'e' : magic.x+=magic.speed;break;
		case 'w' : magic.x-=magic.speed;break;
	}
	magic.duration--;
	SDL_Rect spell, possibleHit;
	spell.x = magic.x;
	spell.y = magic.y;
	spell.w = magic.w;
	spell.h = magic.h;
	for( int i = 0;i<roadblock.size();i++ )
	{
		possibleHit.x = roadblock[i].x;
		possibleHit.y = roadblock[i].y;
		possibleHit.w = roadblock[i].w;
		possibleHit.h = roadblock[i].h;
		if( RectCollision(spell, possibleHit) and roadblock[i].stopsSpells )
		{
			if( roadblock[i].destroyable )
			{
				roadblock[i].curHealth-=magic.dmg;
				if( roadblock[i].curHealth <= 0 )
				{
					std::swap( roadblock[i], roadblock[roadblock.size()-1] );
					roadblock.pop_back();
				}
			}
			magic.duration = 0;
		}
	}
	for( int i = 0;i<humans.size();i++ )
	{
		if( humans[i].id != magic.castByID )
		{
			possibleHit.x = humans[i].x;
			possibleHit.y = humans[i].y;
			possibleHit.w = humans[i].w;
			possibleHit.h = humans[i].h;
			if( RectCollision( spell, possibleHit ) )
			{
				humans[i].curHealth -= magic.dmg;
				magic.duration = 0;
				if( humans[i].curHealth <= 0 and i != playerID )
				{
					std::swap( humans[i], humans[humans.size()-1] );
					humans.pop_back();
				}
			}
		}else
		{
			if( magic.usrSlowDown > humans[i].normSpeed )
				magic.usrSlowDown = humans[i].normSpeed;
			if( magic.usrSlowDownDur > 0 )
			{
				magic.usrSlowDownDur--;
				humans[i].speed = humans[i].normSpeed - magic.usrSlowDown;
			}
		}
	}
}
void Move( human_t &someone, bool moveBackground )
{
	SDL_Rect oldPos = {someone.x, someone.y, someone.w, someone.h};
	if( someone.movDirection[0] != 0 and someone.movDirection[1] != 0 )
		someone.speed -= someone.speed/3;
	switch( someone.movDirection[0] )
	{
		case 'n' : someone.y-=someone.speed;break;
		case 's' : someone.y+=someone.speed;break;
	}
	switch( someone.movDirection[1] )
	{
		case 'w' : someone.x-=someone.speed;break;
		case 'e' : someone.x+=someone.speed;break;
	}
	if( someone.prevDrawDirection == someone.movDirection[0] or someone.prevDrawDirection == someone.movDirection[1] )
		someone.drawDirection = someone.prevDrawDirection;
	else
	{
		if( someone.movDirection[0] != 0 )
			someone.drawDirection = someone.movDirection[0];
		if( someone.movDirection[1] != 0 )
			someone.drawDirection = someone.movDirection[1];
	}
	SDL_Rect human, pathBlocker;
	human.x = someone.x;
	human.y = someone.y;
	human.w = someone.w;
	human.h = someone.h;
	for( int i = 0;i<roadblock.size();i++ )
	{
		pathBlocker.x = roadblock[i].x;
		pathBlocker.y = roadblock[i].y;
		pathBlocker.w = roadblock[i].w;
		pathBlocker.h = roadblock[i].h;	
		if( RectCollision( human, pathBlocker ) and roadblock[i].stopsHumans )
		{
			someone.x = oldPos.x;
			someone.y = oldPos.y;
			someone.w = oldPos.w;
			someone.h = oldPos.h;	
			someone.movDirection[0] = 0;
			someone.movDirection[1] = 0;
			return;
		}
	}
	for( int i = 0;i<humans.size();i++ )
	{
		if( humans[i].id != someone.id )
		{
			pathBlocker.x = humans[i].x;
			pathBlocker.y = humans[i].y;
			pathBlocker.w = humans[i].w;
			pathBlocker.h = humans[i].h;
			if( RectCollision( human, pathBlocker ) )
			{
				someone.x = oldPos.x;
				someone.y = oldPos.y;
				someone.w = oldPos.w;
				someone.h = oldPos.h;	
				someone.movDirection[0] = 0;
				someone.movDirection[1] = 0;
				return;			
			}
		}
	}
	if( moveBackground )
	{
		switch(someone.movDirection[0])
		{
			case 'n' : if( someone.pos.y > someone.pos.h*5 ){ someone.pos.y -= someone.speed; }else{ backgroundPos.y += someone.speed; }break;
			case 's' : if( someone.pos.y < (screenHeight-someone.pos.h*6) ){ someone.pos.y+=someone.speed; }else{ backgroundPos.y -= someone.speed; }break;
		}
		switch(someone.movDirection[1])
		{
			case 'e' : if( someone.pos.x < (screenWidth-someone.pos.w*8) ){ someone.pos.x+=someone.speed; }else{ backgroundPos.x -= someone.speed; }break;
			case 'w' : if( someone.pos.x > someone.pos.w*7 ){ someone.pos.x-=someone.speed; }else{ backgroundPos.x += someone.speed; }break;
		}
	}
	someone.movDirection[0] = 0;
	someone.movDirection[1] = 0;
	someone.speed = someone.normSpeed;
}
void Attack(human_t &someone)
{
	if(someone.attDirection == 0)
		return;
	if(someone.eqpSpell == -1)
	{
		someone.drawDirection = someone.attDirection;
		return;
	}
	aoe_t attack = avalSpells[someone.eqpSpell];
	attack.castByID = someone.id;
	attack.x = someone.x;
	attack.y = someone.y;
	attack.dir = someone.attDirection;
	switch( someone.attDirection )
	{
		case 'n' : attack.y-=attack.h;attack.angle=270;break;
		case 's' : attack.y+=someone.h;attack.angle=90;break;
		case 'e' : attack.x+=someone.w;attack.angle=0;break;
		case 'w' : attack.x-=attack.w;attack.angle=180;break;
	}
	attack.x += (someone.w-attack.w)/2;
	attack.y += (someone.h-attack.h)/2;
	if( someone.attDirection == 'n' or someone.attDirection == 's' )
	{
		std::swap( attack.w, attack.h );
//		std::swap( attack.pos.w, attack.pos.h );
	}
//	attack.flip = SDL_FLIP_NONE;
	if( someone.spellWaitTime[someone.eqpSpell] <= 0 )
	{
		activeSpells.push_back( attack );
		someone.spellWaitTime[someone.eqpSpell] = attack.waitTime;
	}
	someone.drawDirection = someone.attDirection;
	someone.attDirection = 0;
}
void LoadLevel( char levelToLoad[] )
{
	humans.clear();
	roadblock.clear();
	avalSpells.clear();
	activeSpells.clear();
	std::ifstream level( levelToLoad );
	std::vector< std::pair<int, int> > loadedTextures;
	if( level.is_open() )
	{
		while( !level.eof() )
		{
			std::string line;
			do
				std::getline( level, line );
			while( ( line[0] == '#' or line.length() < 2 ) and !level.eof() );
			if( level.eof() )
				break;
			int i=0;
			char type, fileName[20];
			while( line[i] == ' ' )
				i++;
			type = line[i];
			i++;
			while( line[i] == ' ' )
				i++;
			int f = 0;
			int pseudoHash = 0;
			while( line[i] != ' ' and type != 'p' )
			{
				fileName[f] = line[i];
				pseudoHash+=fileName[f]*(f+1);
				f++;
				i++;
			}
			fileName[f] = '\0';
			while( line[i] == ' ' )
				i++;
			std::vector<int> info;
			int num = 0;
			std::vector<int> bucket;
			int negativ = 1;
			for( ;i<=line.length();i++ )
			{
				if( line[i] == ' ' or i == line.length() )
				{
					for( int b=0;b<bucket.size();b++ )
						num += bucket[b] * pow(10, (bucket.size()-1-b) );
					num*=negativ;
					info.push_back(num);
					num = 0;
					negativ = 1;
					bucket.clear();
					while( line[i] == ' ' and i<line.length() )
						i++;
				}
				if( line[i] == '-' )
					negativ = -1;
				else
					bucket.push_back( line[i]-'0' );
			}
			bucket.clear();
			bool newTexture = true;
			int textureIDToGive;
			for( int t=0;t<loadedTextures.size();t++ )
			{
				if( pseudoHash == loadedTextures[t].first )
				{
					textureIDToGive = loadedTextures[t].second;
					newTexture = false;
					break;
				}
			}
			if( newTexture and type != 'p' )
			{
				textureIDToGive = ++nextAvalTextureID;
				char fullFileName[30] = "Textures/";
				char format[] = ".png\0";
				std::copy( format, format+5, fileName+f );
				std::copy( std::begin(fileName), std::end(fileName), fullFileName+9 );
				textures.push_back( IMG_LoadTexture( renderer, fullFileName ) );
				loadedTextures.push_back( {pseudoHash, textureIDToGive} );
			}
			if( type == 'o' )
			{
				obsticle_t toPush( info, textureIDToGive, scale );
				roadblock.push_back(toPush);
			}
			if( type == 'a' )
			{
				aoe_t toPush;
				toPush.CreateFromInfo( info, textureIDToGive, scale );
				avalSpells.push_back( toPush );
			}
			if( type == 'h' )
			{
				human_t toPush( info, ++nextAvalHumanID, textureIDToGive, ++nextAvalThreadID, scale );
				humans.push_back( toPush );
				if( toPush.state == -1 )
				{
					playerID = 0;
					std::swap( humans[ humans.size()-1 ], humans[0] );
				}
			}
			if( type == 'b' )
			{
				backgroundTextureID = textureIDToGive;
				backgroundPos = {info[0], info[1], info[2], info[3]};
			}
			if( type == 'p' )
			{
				humans[humans.size()-1].cycle = info[0];
				for( int j=1;j<info.size();j+=2 )
				{
					flag_t newPoint;
					newPoint.x = info[j];
					newPoint.y = info[j+1];
					humans[humans.size()-1].patrolPoint.push_back( newPoint );
				}
			}
			info.clear();
		}
	}else
	{
		std::cout<<"Error loading level!"<<std::endl;
	}
#ifdef TEXTURE_TEST
	std::cout<<"Begin"<<std::endl;
	for( int i=0;i<textures.size();i++ )
	{
		std::cout<<i<<std::endl;
		SDL_RenderCopy( renderer, textures[i], NULL, NULL );
		SDL_RenderPresent(renderer);
		SDL_Delay(1000);
		SDL_RenderClear(renderer);
	}
	std::cout<<"End"<<std::endl;
#endif
}
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
	int spellchngTimeout = 0;
	int frames = 0;
	std::vector<int> sframes;
	sframes.push_back(0);
	while( true )
	{
		if( !levelLoaded )
		{
			SDL_RenderCopy( renderer, loading, NULL, NULL );
			SDL_RenderPresent( renderer );
			LoadLevel( level );

			if( !ignoreNet )
			{
				int active = SDLNet_CheckSockets( chkNet, 0 );
				int recv[1000];
				if( active > 0 )
				{
					int recvLen = SDLNet_TCP_Recv( sock, recv, 1000 );
					recvLen/=4;
					humans[playerID].netID = recv[0];
					for( int i=1;i<recvLen;i+=3 )
					{
						if( recv[i] == -1 )
							break;
						human_t newGuy = humans[playerID];
						newGuy.netID = recv[i];
						newGuy.state = -9;
						newGuy.id = ++nextAvalHumanID;
						newGuy.x = recv[i+1];
						newGuy.y = recv[i+2];
						humans.push_back( newGuy );
					}
					int info[4] = {humans[playerID].netID, humans[playerID].x, humans[playerID].y};
					SDLNet_TCP_Send( sock, info, 16 );
				}
			}

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
			int info[4] = {humans[playerID].netID, humans[playerID].x, humans[playerID].y};
			SDLNet_TCP_Send( sock, info, 16 );
			int active = SDLNet_CheckSockets( chkNet, 0 );
			if( active > 0 )
			{
				int rcv[5];
				SDLNet_TCP_Recv( sock, rcv, 20 );
				if( rcv[0] == -1 )
				{
					human_t newGuy = humans[playerID];
					newGuy.id = ++nextAvalHumanID;
					newGuy.state = -9;
					newGuy.netID = rcv[1];
					newGuy.x = rcv[2];
					newGuy.y = rcv[3];
					humans.push_back( newGuy );
				}
				for( int i=0;i<humans.size();i++ )
				{
					if( humans[i].netID == rcv[0] and i!=playerID )
					{
						humans[i].speed = 0;
						if( rcv[1] > humans[i].x )
							humans[i].movDirection[1] = 'e';
						if( rcv[1] < humans[i].x )
							humans[i].movDirection[1] = 'w';
						if( rcv[1] == humans[i].x )
							humans[i].movDirection[1] = 0;
						if( rcv[2] > humans[i].y )
							humans[i].movDirection[0] = 's';
						if( rcv[2] < humans[i].y )
							humans[i].movDirection[0] = 'n';
						if( rcv[2] == humans[i].y )
							humans[i].movDirection[0] = 0;
						humans[i].x = rcv[1];
						humans[i].y = rcv[2];
						break;
					}
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
				Move( humans[i], followHuman );
			}
			movT = SDL_GetTicks();
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
					Spell( activeSpells[i] );
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
			//std::cout<<SDL_GetError()<<std::endl;
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
				humans[i].pos.x = humans[playerID].pos.x - humans[playerID].x + humans[i].x + (humans[i].pos.w - humans[i].w)/2;
				humans[i].pos.y = humans[playerID].pos.y - humans[playerID].y + humans[i].y + (humans[i].pos.h - humans[i].h)/2;
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
