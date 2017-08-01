bool MoveHuman( human_t &someone )
{
	/*
	SDL_Rect oldPos = {(int)someone.pos.x, (int)someone.pos.y, someone.size.w, someone.size.h};
//	if( someone.movDirection[0] != 0 and someone.movDirection[1] != 0 )
//		someone.curSpeed  = std::sqrt( someone.;
	float hor = 0;
	float ver = 0;
	switch( someone.movDirection[0] )
	{
		case 'n' : hor-=someone.curSpeed;break;
		case 's' : hor+=someone.curSpeed;break;
	}
	switch( someone.movDirection[1] )
	{
		case 'w' : ver-=someone.curSpeed;break;
		case 'e' : ver+=someone.curSpeed;break;
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
	someone.pos.Update( hor, ver );

	SDL_Rect human = {(int)someone.pos.x, (int)someone.pos.y, someone.size.w, someone.size.h};
	SDL_Rect pathBlocker;
	for( int i = 0;i<roadblock.size();i++ )
	{
		pathBlocker.x = roadblock[i].pos.x;
		pathBlocker.y = roadblock[i].pos.y;
		pathBlocker.w = roadblock[i].size.w;
		pathBlocker.h = roadblock[i].size.h;	
		if( RectCollision( human, pathBlocker ) and roadblock[i].stopsHumans )
		{
			someone.pos.Set( oldPos.x, oldPos.y );
			return false;
		}
	}
	/*
	for( int i = 0;i<humans.size();i++ )
	{
		if( humans[i].id != someone.id )
		{
			pathBlocker.x = humans[i].pos.x;
			pathBlocker.y = humans[i].pos.y;
			pathBlocker.w = humans[i].size.w;
			pathBlocker.h = humans[i].size.h;
			if( RectCollision( human, pathBlocker ) )
			{
				someone.pos.Set( oldPos.x, oldPos.y );
				return false;
			}
		}
	}
	*/
	if( someone.movDirection[0] == 0 and someone.movDirection[1] == 0 )
		return false;
	int horVelocity = 0;
	int verVelocity = 0;
	if( someone.movDirection[1] == 'w' )
		horVelocity = -someone.curSpeed;
	if( someone.movDirection[1] == 'e' )
		horVelocity = +someone.curSpeed;
	if( someone.movDirection[0] == 'n' )
		verVelocity = -someone.curSpeed;
	if( someone.movDirection[0] == 's' )
		verVelocity = +someone.curSpeed;

	someone.pos.Update( horVelocity, verVelocity );
	//someone.pos.Ceil();
	someone.movDirection[0] = 0;
	someone.movDirection[1] = 0;
	return true;

	//someone.curSpeed = someone.normSpeed;
}
