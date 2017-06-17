/*
void Spell( aoe_t &magic, std::vector<human_t> &humans, std::vector<obsticle_t> &roadblock, std::vector<int> &destroyedHumans, std::vector<int> &destroyedRoadblocks )
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
					destroyedRoadblocks.push_back( i );
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
				if( humans[i].curHealth <= 0 )
					destroyedHumans.push_back( i );
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
void MoveOnPath( point_t &pos, pathSegment_t pth )
{
	double sin = std::sin( (double)pth.angle*M_PI/180 );
	double cos = std::cos( (double)pth.angle*M_PI/180 );

	if( pth.type == 'l' )
	{
		double updateX = pth.speed * cos;
		double updateY = pth.speed * sin;
		pos.Update( updateX, updateY );
	}
	//circle?
}
void MoveBullets( bullet_t &bullet, std::vector<human_t> humans, std::vector<obsticle_t> roadblock, std::vector<int> &destroyedHumans, std::vector<int> &destroyedRoadblocks )
{
	if( bullet.path.Empty() )
		return;

	MoveOnPath( bullet.location, *bullet.path.curPath );
	
	SDL_Rect bulletRect = bullet.MakeRect();
	SDL_Rect possibleHit;

	for( int i=0;i<roadblock.size();i++ )
	{
		possibleHit.x = roadblock[i].x;
		possibleHit.y = roadblock[i].y;
		possibleHit.w = roadblock[i].w;
		possibleHit.h = roadblock[i].h;
		if( RectCollision( bulletRect, possibleHit ) and roadblock[i].stopsSpells )
		{
			bullet.remainingTime = 0;
			if( roadblock[i].destroyable )
			{
				roadblock[i].curHealth -= bullet.dmg;
				if( roadblock[i].curHealth <= 0 )
					destroyedRoadblocks.push_back( i );
			}
		}
	}

	for( int i=0;i<humans.size();i++ )
	{
		if( bullet.castByID != humans[i].id )
		{
			possibleHit.x = humans[i].x;
			possibleHit.y = humans[i].y;
			possibleHit.w = humans[i].w;
			possibleHit.h = humans[i].h;
			if( RectCollision( bulletRect, possibleHit ) )
			{
				bullet.remainingTime = 0;
				humans[i].curHealth -= bullet.dmg;
				if( humans[i].curHealth <= 0 )
					destroyedHumans.push_back( i );
			}
		}
	}
}
void MoveSpawner( spawner_t &spawner )
{
	if( spawner.path.Empty() )
		return;
	if( spawner.path.curPath->type == 'r' )
		spawner.angleOffset += spawner.path.curPath->angle;
	MoveOnPath( spawner.pos, *spawner.path.curPath );
}
*/
void Move( human_t &someone, std::vector<human_t> humans, std::vector<obsticle_t> roadblock,  bool moveBackground = false )
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
/*
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
*/
