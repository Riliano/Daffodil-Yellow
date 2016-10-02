#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
//#include<queue>
#include<vector>
//#include<thread>
#include<cmath>
#include<algorithm>
#include<fstream>

#include"init.h"
#include"input.h"
#include"struct.cpp"

SDL_Renderer* renderer;
SDL_Event e;
//std::queue<int> tasks;
int numThread = 2;

TTF_Font* gothic;

human_t player;
SDL_Texture* playerTexture;
std::vector<human_t> enemy;
SDL_Texture* enemyTexture;
SDL_Rect backgroundPos;
SDL_Rect backgroundFrame;
SDL_Texture* backgroundTexture;
std::vector <obsticle_t> roadblock;
SDL_Texture* rockTexture;
SDL_Texture* treeTexture;
std::vector<aoe_t> activeSpells;
SDL_Texture* slashTexture;
SDL_Texture* fireballTexture;
SDL_Rect dummy;
const int NUM_SPELLS = 5;
int spellWaitTime[NUM_SPELLS];
float scale = 1;//(float)screenWidth/480.0;
int nextAvalID;

bool RectCollision( SDL_Rect first, SDL_Rect second )
{
	if( ( first.x<=second.x+second.w and first.x+first.w >=second.x ) and ( first.y<=second.y+second.h and first.y+first.h >=second.y ) )
		return true;
	else
		return false;
}

int Distance( int x1, int y1, int x2, int y2 )
{
	int ans = (y2-y1)*(y2-y1) + (x2-x1)*(x2-x1);
	ans = sqrt( ans );
	return ans;
}

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
		if( RectCollision(spell, possibleHit) )
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
	if( player.id != magic.castByID)
	{
		possibleHit.x = player.x;
		possibleHit.y = player.y;
		possibleHit.w = player.w;
		possibleHit.h = player.h;
		if( RectCollision( spell, possibleHit ) )
		{
			player.curHealth -= magic.dmg;
			magic.duration = 0;
		}
	}else
	{
		if( magic.usrSlowDown > player.normSpeed )
			magic.usrSlowDown = player.normSpeed;
		if( magic.usrSlowDownDur > 0 )
		{
			magic.usrSlowDownDur --;
			player.speed = player.normSpeed - magic.usrSlowDown;
		}
	}
	for( int i = 0;i<enemy.size();i++ )
	{
		if( enemy[i].id != magic.castByID )
		{
			possibleHit.x = enemy[i].x;
			possibleHit.y = enemy[i].y;
			possibleHit.w = enemy[i].w;
			possibleHit.h = enemy[i].h;
			if( RectCollision( spell, possibleHit ) )
			{
				enemy[i].curHealth -= magic.dmg;
				magic.duration = 0;
				if( enemy[i].curHealth <= 0 )
				{
					std::swap( enemy[i], enemy[enemy.size()-1] );
					enemy.pop_back();
				}
			}
		}else
		{
			if( magic.usrSlowDown > enemy[i].normSpeed )
				magic.usrSlowDown = enemy[i].normSpeed;
			if( magic.usrSlowDownDur > 0 )
			{
				magic.usrSlowDownDur--;
				enemy[i].speed = enemy[i].normSpeed - magic.usrSlowDown;
			}
		}
	}
}

void Move(human_t &someone, SDL_Rect &backgroundPos = dummy)
{
	SDL_Rect oldPos = {someone.x, someone.y, someone.w, someone.h};
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
	std::vector<human_t> otherHumans;
	otherHumans.push_back( player );
	otherHumans.insert( otherHumans.end(), enemy.begin(), enemy.end() );
	for( int i = 0;i<otherHumans.size();i++ )
	{
		if( otherHumans[i].id != someone.id )
		{
			pathBlocker.x = otherHumans[i].x;
			pathBlocker.y = otherHumans[i].y;
			pathBlocker.w = otherHumans[i].w;
			pathBlocker.h = otherHumans[i].h;
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
	switch(someone.movDirection[0])
	{
		case 'n' : if( someone.pos.y > someone.pos.h*2 ){ someone.pos.y -= someone.speed; }else{ backgroundPos.y += someone.speed; }break;
		case 's' : if( someone.pos.y < (screenHeight-someone.pos.h*3) ){ someone.pos.y+=someone.speed; }else{ backgroundPos.y -= someone.speed; }break;
	}
	switch(someone.movDirection[1])
	{
		case 'e' : if( someone.pos.x < (screenWidth-someone.pos.w*3) ){ someone.pos.x+=someone.speed; }else{ backgroundPos.x -= someone.speed; }break;
		case 'w' : if( someone.pos.x > someone.pos.w*2 ){ someone.pos.x-=someone.speed; }else{ backgroundPos.x += someone.speed; }break;
	}

	someone.movDirection[0] = 0;
	someone.movDirection[1] = 0;
	someone.speed = someone.normSpeed;
}
void DrawDir( human_t &someone )
{
	switch(someone.drawDirection)
	{
		case 'n' : someone.frame.y=3*someone.frame.h;break;
		case 's' : someone.frame.y=0*someone.frame.h;break;
		case 'e' : someone.frame.y=2*someone.frame.h;break;
		case 'w' : someone.frame.y=1*someone.frame.h;break;
		case 0 : return;
	}
	someone.prevDrawDirection = someone.drawDirection;
	someone.drawDirection = 0;
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
	aoe_t attack;
	if(someone.eqpSpell == 0)
	{

		attack.id = 1;
		attack.dir = 0;
		attack.speed = 0;
		attack.duration = 30;
		attack.dmg = 2;
		attack.usrSlowDown = 3;
		attack.usrSlowDownDur = 30;
		attack.frame.x = -16;
		attack.frame.y = 0;
		attack.frame.w = 16;
		attack.frame.h = 20;
		attack.pos.w = scale*25;
		attack.pos.h = scale*40;
		attack.waitTime = 30;
		switch (someone.attDirection)
		{
			case 'n' : attack.h = 30;attack.w = 20;attack.x = someone.x+15;attack.y=someone.y-attack.h;attack.angle = 0;attack.flip = SDL_FLIP_NONE;break;
			case 's' : attack.h = 30;attack.w = 20;attack.x = someone.x+15;attack.y=someone.y+someone.h;attack.angle = 180;attack.flip = SDL_FLIP_NONE;break;
			case 'w' : attack.h = 20;attack.w = 30;attack.x = someone.x-attack.w;attack.y = someone.y+15;attack.angle = 90;attack.flip = SDL_FLIP_VERTICAL;break;
			case 'e' : attack.h = 20;attack.w = 30;attack.x = someone.x+someone.w;attack.y = someone.y+15;attack.angle = 90;attack.flip = SDL_FLIP_NONE;break;
		}
		

	}
	if(someone.eqpSpell == 1)
	{
		attack.id = 2;
		attack.dir = someone.attDirection;
		attack.speed = 15;
		attack.duration = 1000;
		attack.dmg = 5;
		attack.usrSlowDown = 1;
		attack.usrSlowDownDur = 5;
		attack.frame.x = -32;
		attack.frame.y = 0;
		attack.frame.w = 32;
		attack.frame.h = 32;
		attack.pos.w = scale*32;
		attack.pos.h = scale*32;
		attack.waitTime = 80;
		attack.h = 30;
		attack.w = 30;
		attack.x = someone.x;
		attack.y = someone.y;
		switch(someone.attDirection)
		{
			case 's' : attack.angle = 90;attack.flip = SDL_FLIP_NONE;break;
			case 'n' : attack.angle = 90;attack.flip = SDL_FLIP_HORIZONTAL;break;
			case 'w' : attack.angle = 0;attack.flip = SDL_FLIP_HORIZONTAL;break;
			case 'e' : attack.angle = 0;attack.flip = SDL_FLIP_NONE;break;
		}
	}
	if(someone.eqpSpell == 2)
	{
		attack.id = 3;
		attack.dir = someone.attDirection;
		attack.speed = 5;
		attack.duration = 20;
		attack.dmg = 1;
		attack.usrSlowDown = 2;
		attack.usrSlowDownDur = 10;
		attack.frame.x = -32;
		attack.frame.y = 0;
		attack.frame.w = 32;
		attack.frame.h = 32;
		attack.pos.w = scale*32;
		attack.pos.h = scale*32;
		attack.waitTime = 5;
		attack.h = 30;
		attack.w = 30;
		attack.x = someone.x;
		attack.y = someone.y;
		switch(someone.attDirection)
		{
			case 's' : attack.angle = 90;attack.flip = SDL_FLIP_NONE;break;
			case 'n' : attack.angle = 90;attack.flip = SDL_FLIP_HORIZONTAL;break;
			case 'w' : attack.angle = 0;attack.flip = SDL_FLIP_HORIZONTAL;break;
			case 'e' : attack.angle = 0;attack.flip = SDL_FLIP_NONE;break;
		}
	}
	if( someone.spellWaitTime[attack.id]<=0 )
	{
		attack.castByID = someone.id;
		activeSpells.push_back( attack );
		someone.spellWaitTime[attack.id] = attack.waitTime;
	}
	someone.drawDirection = someone.attDirection;
	//someone.prevAttDir = someone.attDirection;
	someone.attDirection = 0;

}
void Draw( human_t &someone )
{
	if( someone.drawDirection == 0 )
	{
		someone.frame.x = 1*someone.frame.w;
		return;
	}
	if( someone.drawDirection == someone.prevDrawDirection or someone.prevDrawDirection == someone.attDirection )
	{
		someone.frame.x+=someone.frame.w*someone.frameDirection;
		if( someone.frame.x >=someone.frame.w*2 or someone.frame.x == 0 )
			someone.frameDirection = someone.frameDirection*(-1);
	}else
	{
		someone.frame.x = 0;
		someone.frameDirection = 1;
	}
	someone.prevDrawDirection = someone.drawDirection;
	someone.drawDirection = 0;
}

void DrawSpell( aoe_t &magic )
{
	if(magic.duration<=-1)
	{
		magic.duration--;
		return;
	}
	magic.frame.x += magic.frame.w;
	int temp;
	switch ( magic.id )
	{
		case 1 : temp = 3;break;
		case 2 : temp = 4;break;
	}
	if( magic.frame.x == temp*magic.frame.w )
	{
		magic.frame.x = 0;
		if( magic.duration == 0 )
		{
			magic.frame.x = temp*magic.frame.w;
			magic.duration = -1;
		}
	}
}

void LoadLevel( char level[] )
{
	std::ifstream levelLayout ( level );
	std::string temp;
	if(levelLayout.is_open())
	{
		int push[10] = {0, 0, 0, 0, 0, 0,  0, 0, 0, 0};
		int thisIs = 0;
		while( levelLayout >> temp )
		{
			if((temp[0] < '0' and temp[0] > '9') or thisIs >9)
			{
				obsticle_t toPush;
				toPush.frame.x = 0;
				toPush.frame.y = 0;
				toPush.pos.w = push[0]*scale;
				toPush.pos.h = push[1]*scale;
				toPush.frame.w = push[2];
				toPush.frame.h = push[3];
				toPush.curHealth = push[4];
				toPush.destroyable = push[5];
				toPush.x = push[6];
				toPush.y = push[7];
				toPush.w = push[8];
				toPush.h = push[9];
				switch ( temp[0] )
				{
					case 'r' : toPush.type = 'r';break;
					case 't' : toPush.type = 't';break;
				}
				roadblock.push_back( toPush );
				thisIs = 0;
				for( int i = 0;i<10;i++ )
					push[i] = 0;
			}else
			{
				int times = 1;
				int reverse = 1;
				for(int i = temp.size() - 1;i>=0;i--)
				{
					if( temp[i]=='-' )
						reverse = -1;
					else
						push[thisIs] = push[thisIs] + (temp[i]-'0')*times;
					times=times*10;
				}
				push[thisIs] *= reverse;
				thisIs++;
			}
		}

	}else
		std::cout<<"Error loading level "<<level[0]<<std::endl;
	char enemyPos[] = "Levels/1.lvlenemy";
	std::ifstream enemyLayout (enemyPos);
	if(enemyLayout.is_open())
	{
		int push[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0};
		int thisIs = 0;
		while( !enemyLayout.eof() )
		{
			if(thisIs > 10)
			{
				human_t toPush;
				toPush.frame.x = 0;
				toPush.frame.y = 0;
				toPush.pos.w = push[0]*scale;
				toPush.pos.h = push[1]*scale;
				toPush.frame.w = push[2];
				toPush.frame.h = push[3];
				toPush.maxHealth = push[4];
				toPush.curHealth = toPush.maxHealth;
				toPush.normSpeed = push[5];
				toPush.speed = toPush.normSpeed;
				toPush.x = push[6];
				toPush.y = push[7];
				toPush.w = push[8];
				toPush.h = push[9];
				toPush.eqpSpell = push[10];
				toPush.id = nextAvalID;
				nextAvalID++;
				enemy.push_back( toPush );
				thisIs = 0;
				for(int i = 0;i<=10;i++)
					push[i] = 0;
			}else
			{
				enemyLayout >> temp;
				int times = 1;
				int reverse = 1;
				for(int i = temp.size() - 1;i>=0;i--)
				{
					if(temp[i] == '-')
						reverse = -1;
					else
						push[thisIs] = push[thisIs] + (temp[i]-'0')*times;
					times=times*10;
				}
				push[thisIs]*=reverse;
				thisIs++;
			}
		}


	}else
		std::cout<<"Error loading level "<<level[0]<<std::endl;

}

void Patrol( human_t &someone )
{
	someone.eqpSpell = -1;
	SDL_Rect target = {player.x, player.y, player.w, player.h};
	SDL_Rect vision = {0, 0, someone.visionW, someone.visionH};
	switch(someone.prevDrawDirection)
	{
		case 'w' : someone.attDirection = 'n';vision.x=someone.x+someone.w/2-someone.visionW/2;vision.y=someone.y-someone.visionH;break;
		case 'n' : someone.attDirection = 'e';vision.x=someone.x+someone.w;someone.y+someone.h/2-someone.visionW/2;std::swap( vision.w, vision.h );break;
		case 'e' : someone.attDirection = 's';vision.x=someone.x+someone.w/2-someone.visionW/2;vision.y=someone.y+someone.w;break;
		case 's' : someone.attDirection = 'w';vision.x=someone.x-someone.visionH;someone.y+someone.h/2-someone.visionW/2;std::swap( vision.w, vision.h );break;
		case 0 : someone.attDirection = 'n';break;
	}
	
	if( RectCollision( target, vision ) )
	{
		someone.targetX = target.x;
		someone.targetY = target.y;
		someone.state = 1;
		someone.eqpSpell = someone.avalSpells[0];
		someone.prevAttDir = someone.attDirection;
	}
}

int main()
{
	//std::thread worker[numThread];
	Init( renderer );
	TTF_Init();
	if( ShouldITurnVSync() )
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	else
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

	playerTexture = IMG_LoadTexture(renderer, "Textures/RedMage.png");
	enemyTexture = IMG_LoadTexture(renderer, "Textures/knight.png");
	backgroundTexture = IMG_LoadTexture(renderer, "Textures/background.png");
	treeTexture = IMG_LoadTexture(renderer, "Textures/tree.png");
	rockTexture = IMG_LoadTexture(renderer, "Textures/rock.png");
	slashTexture = IMG_LoadTexture(renderer, "Textures/slash.png");
	fireballTexture = IMG_LoadTexture(renderer, "Textures/fireball.png");
	std::cout<<SDL_GetError()<<std::endl;
	gothic = TTF_OpenFont( "Fonts/MS Gothic.ttf", 20 );

	player.pos.x = 150;
	player.pos.y = 150;
	player.pos.w = scale*50;
	player.pos.h = scale*50;
	player.frame.x = 0;
	player.frame.y = 0;
	player.frame.w = 32;
	player.frame.h = 32;
	player.maxHealth = 20;
	player.curHealth = 20;
	player.speed = 3;
	player.normSpeed = 3;
	player.x = 145;
	player.y = 55;
	player.w = 40;
	player.h = 40;
	player.avalSpells.push_back(0);
	player.avalSpells.push_back(1);
	player.avalSpells.push_back(2);
	player.eqpSpell = player.avalSpells[0];
	player.curSpellNum = 0;
	player.id = nextAvalID;
	nextAvalID++;

	backgroundPos.w = scale*screenWidth;
	backgroundPos.h = scale*screenHeight;
	backgroundPos.x = 0;
	backgroundPos.y = 0;
	backgroundFrame.h = screenHeight;
	backgroundFrame.w = screenWidth;
	backgroundFrame.x = 0;
	backgroundFrame.y = 0;

	SDL_Rect curEquipSpellPos;
	curEquipSpellPos.w = 32;
	curEquipSpellPos.h = 32;
	curEquipSpellPos.x = 0;
	curEquipSpellPos.y = screenHeight - curEquipSpellPos.h;
	SDL_Rect curEquipSpellFrame;
	curEquipSpellFrame.x = 32;
	curEquipSpellFrame.y = 0;
	curEquipSpellFrame.w = 32;
	curEquipSpellFrame.h = 32;
	SDL_Texture* curEquipSpell;
	
	SDL_Texture* loading = IMG_LoadTexture( renderer, "Textures/loading.png" );
	char level[] = "Levels/1.lvl";
	bool levelLoaded = false;

	long long movT = SDL_GetTicks();
	long long inputT = SDL_GetTicks();
	long long drawT = SDL_GetTicks();
	long long infoT = SDL_GetTicks();
	long long fpsT = SDL_GetTicks();
	long long attT = SDL_GetTicks();
	long long spellT = SDL_GetTicks();
	long long checkFlagsT = SDL_GetTicks();
	long long drawSpellT = SDL_GetTicks();
	long long BOTpatrolT = SDL_GetTicks();
	long long BOTattackT = SDL_GetTicks();
	int spellchngTimeout = 0;
	int frames = 0;
	char cframes[5];
	
	flag_t navMesh[10000];
	
	while(true)
	{
		if( !levelLoaded )
		{
			SDL_RenderCopy( renderer, loading, NULL, NULL );
			SDL_RenderPresent( renderer );
			LoadLevel( level );
			
			int tempX = enemy[0].x, tempY = enemy[0].y;
			int numFlag = 0;
			for( int i=0;i<100;i++ )
			{
				navMesh[numFlag].belongsToID = enemy[0].id;
				navMesh[numFlag].x = tempX;
				navMesh[numFlag].y = tempY;
				navMesh[numFlag].w = enemy[0].w;
				navMesh[numFlag].h = enemy[0].h;
				navMesh[numFlag].type = 1;
				navMesh[numFlag].goTo[1] = 'w';	
				tempX-=enemy[0].speed;
				numFlag++;
			}
			for( int i=0;i<100;i++ )
			{
				navMesh[numFlag].belongsToID = enemy[0].id;
				navMesh[numFlag].x = tempX;
				navMesh[numFlag].y = tempY;
				navMesh[numFlag].w = enemy[0].w;
				navMesh[numFlag].h = enemy[0].h;
				navMesh[numFlag].type = 1;
				navMesh[numFlag].goTo[0] = 's';	
				tempY+=enemy[0].speed;
				numFlag++;
			}
			for( int i=0;i<100;i++ )
			{
				navMesh[numFlag].belongsToID = enemy[0].id;
				navMesh[numFlag].x = tempX;
				navMesh[numFlag].y = tempY;
				navMesh[numFlag].w = enemy[0].w;
				navMesh[numFlag].h = enemy[0].h;
				navMesh[numFlag].type = 1;
				navMesh[numFlag].goTo[1] = 'e';	
				tempX+=enemy[0].speed;
				numFlag++;
			}
			for( int i=0;i<100;i++ )
			{
				navMesh[numFlag].belongsToID = enemy[0].id;
				navMesh[numFlag].x = tempX;
				navMesh[numFlag].y = tempY;
				navMesh[numFlag].w = enemy[0].w;
				navMesh[numFlag].h = enemy[0].h;
				navMesh[numFlag].type = 1;
				navMesh[numFlag].goTo[0] = 'n';	
				tempY-=enemy[0].speed;
				numFlag++;
			}

			levelLoaded = true;
			SDL_RenderClear( renderer );
		}
		while( SDL_PollEvent(&e) )
		{
			if( e.type == SDL_QUIT )
			{
				SDL_Quit();
				return 0;
			}
			
		}
		if( SDL_GetTicks() - inputT >= 10 )
		{
			ScanKeyboard();			
			if( input.movUp )
				player.movDirection[0] = 'n';
			if( input.movDown )
				player.movDirection[0] = 's';
			if( input.movRight )
				player.movDirection[1] = 'e';
			if( input.movLeft )
				player.movDirection[1] = 'w';
			if( input.attUp )
				player.attDirection = 'n';
			if( input.attDown )
				player.attDirection = 's';
			if( input.attRight )
				player.attDirection = 'e';
			if( input.attLeft )
				player.attDirection = 'w';
			if( input.sprint )
				player.speed = player.normSpeed*1.8;
			if( input.rightSpell and spellchngTimeout == 0 )
			{
				player.curSpellNum++;
				spellchngTimeout = 13;
				if( player.curSpellNum == player.avalSpells.size() )
					player.curSpellNum = 0;
			}
			if( input.leftSpell and spellchngTimeout == 0 )
			{
				player.curSpellNum--;
				spellchngTimeout = 13;
				if( player.curSpellNum == -1 )
					player.curSpellNum = player.avalSpells.size()-1;
			}
			if( spellchngTimeout > 0 )
				spellchngTimeout--;
			player.eqpSpell = player.avalSpells[player.curSpellNum];
			ResetInput();
			inputT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - checkFlagsT >= 10 )
		{
			for( int i=0;i<400/*flagSize*/;i++ )
			{
				if( enemy[0].x == navMesh[i].x and enemy[0].y == navMesh[i].y )
				{
					enemy[0].state = 1;
					enemy[0].movDirection[0] = navMesh[i].goTo[0];
					enemy[0].movDirection[1] = navMesh[i].goTo[1];
					break;
				}
			}
			checkFlagsT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - BOTpatrolT >= 2000 )
		{	
			for( int i=0;i<enemy.size();i++ )
				if( enemy[i].state == 0 )
					Patrol( enemy[i] );
			BOTpatrolT = SDL_GetTicks();
		}
		/*
		if( SDL_GetTicks() - BOTattackT >= 1000 )
		{
			for( int i=0;i<enemy.size();i++ )
			{
				if( enemy[i].state == 1 )
				{
					enemy[i].attDirection = enemy[i].prevAttDir;
					if( enemy[i].attDirection == 'n' or enemy[i].attDirection == 's' )
						enemy[i].movDirection[0] = enemy[i].attDirection;
					if( enemy[i].attDirection == 'w' or enemy[i].attDirection == 'e' )
						enemy[i].movDirection[1] = enemy[i].attDirection;
				}
			}
			BOTattackT = SDL_GetTicks();
		}*/

		if( SDL_GetTicks() - attT >= 1000/60 )
		{
			Attack( player );
			for( int i = 0;i<enemy.size();i++ )
				Attack( enemy[i] );
			attT = SDL_GetTicks();
		}
		
		if( SDL_GetTicks() - movT >= 1000/60 )
		{
			DrawDir( player );
			Move( player, backgroundPos );
			for( int i = 0;i<enemy.size();i++ )
			{
				DrawDir(enemy[i]);
				Move(enemy[i]);
			}
			movT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - drawT >= 1000/4 )
		{
			Draw( player );
			for(int i = 0;i<enemy.size();i++)
				Draw( enemy[i] );
			drawT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - spellT >= 10 )
		{
			for( int i = 0;i<activeSpells.size();i++ )
				if( activeSpells[i].duration > 0 )
					Spell( activeSpells[i] );
			for( int j = 0;j < enemy.size(); j++ )
				for( int i = 0;i<NUM_SPELLS;i++ )
					if( enemy[j].spellWaitTime[i] >0 )
						enemy[j].spellWaitTime[i]--;
			for( int i = 0;i < NUM_SPELLS;i++ )
				if( player.spellWaitTime[i] > 0 )
					player.spellWaitTime[i]--;
			spellT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - drawSpellT >= 1000/15 )
		{
			for( int i = 0;i<activeSpells.size();i++ )
				DrawSpell(activeSpells[i]);
			drawSpellT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - infoT >= 800 )
		{
//			std::cout<<SDL_GetError()<<std::endl;	
			infoT = SDL_GetTicks();
		}
		if( SDL_GetTicks() - fpsT >= 1000 and ShouldIDisplayFPS() )
		{
//			std::cout<<"FPS: "<<frames<<std::endl;
			int nextDigit;
			int temp = frames;
			for(nextDigit = 10;temp > 0;temp/=nextDigit)
				nextDigit*=10;
			for(int i = 0;i<5;i++)
			{
				temp = frames;
				if(nextDigit==0)
				{
					cframes[i] = '\0';
					break;
				}
				cframes[i] = ((temp/nextDigit)%10)+'0';
				nextDigit/=10;
			}
			frames = 0;
			fpsT = SDL_GetTicks();
		}
		SDL_RenderCopy( renderer, backgroundTexture, &backgroundFrame, &backgroundPos );
		for( int i = 0;i<roadblock.size();i++ )
		{
			roadblock[i].pos.x = player.pos.x - player.x + roadblock[i].x;
			roadblock[i].pos.y = player.pos.y - player.y + roadblock[i].y;
			SDL_Texture* temp;
			switch(roadblock[i].type)
			{
				case 't' : temp = treeTexture;break;
				case 'r' : temp = rockTexture;break;
			}
			SDL_RenderCopy( renderer, temp, &roadblock[i].frame, &roadblock[i].pos );
		}
		for( int i = 0;i<activeSpells.size();i++ )
		{
			activeSpells[i].pos.x = player.pos.x - player.x + activeSpells[i].x;
			activeSpells[i].pos.y = player.pos.y - player.y + activeSpells[i].y;
			SDL_Texture* temp;
			switch(activeSpells[i].id)
			{
				case 1 : temp = slashTexture;break;
				case 2 : temp = fireballTexture;break;
				case 3 : temp = fireballTexture;break;
			}
			SDL_RenderCopyEx( renderer, temp, &activeSpells[i].frame, &activeSpells[i].pos, activeSpells[i].angle, activeSpells[i].point, activeSpells[i].flip );
			if( activeSpells[i].duration <= -5 )
			{
				std::swap( activeSpells[i], activeSpells[activeSpells.size()-1] );
				activeSpells.pop_back();	
			}
		}
		for( int i = 0;i<enemy.size();i++ )
		{
			enemy[i].pos.x = player.pos.x - player.x + enemy[i].x;
			enemy[i].pos.y = player.pos.y - player.y + enemy[i].y;
			SDL_RenderCopy( renderer, enemyTexture, &enemy[i].frame, &enemy[i].pos );
		}
		switch (player.eqpSpell)
		{
			case 0 : curEquipSpell = slashTexture;break;
			case 1 : curEquipSpell = fireballTexture;break;
			case 2 : curEquipSpell = fireballTexture;break;
		}
		SDL_RenderCopy( renderer, playerTexture, &player.frame, &player.pos );

		SDL_RenderCopy( renderer, curEquipSpell, &curEquipSpellFrame, &curEquipSpellPos );

		if( ShouldIDisplayFPS() )
		{
			SDL_Surface* fpsSurface = TTF_RenderText_Solid( gothic, cframes, {255, 0, 0} );
			SDL_Texture* fpsTx = SDL_CreateTextureFromSurface( renderer ,fpsSurface );
			SDL_Rect fpsPos = {0, 0, 60, 30};
			SDL_RenderCopy( renderer, fpsTx, NULL, &fpsPos );
			frames++;
		}
		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
}
