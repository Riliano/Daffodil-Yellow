void Move( human_t &someone, std::vector<human_t> humans, std::vector<obsticle_t> roadblock, SDL_Rect &backgroundPos )
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
	{
		someone.drawDirection = someone.prevDrawDirection;
	}
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
