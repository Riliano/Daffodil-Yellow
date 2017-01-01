void AStar( human_t someone, std::vector<node_t> &path, std::vector<human_t> humans, std::vector<obsticle_t> roadblock, node_t &start, bool *quit )
{
	long long begin = clock();
	std::vector<node_t> field;
	std::priority_queue<node_t> bucket;
	node_t temp;
	temp.x = someone.x;
	temp.y = someone.y;
	temp.cameFromX = someone.x;
	temp.cameFromY = someone.y;
	temp.value = Distance( someone.x, someone.y, someone.targetX, someone.targetY );
	field.push_back( temp );
	bucket.push( temp );
	while( !bucket.empty() )
	{
       		node_t cur = bucket.top();
		start = cur;
		path.push_back( cur );
                bucket.pop();
                node_t newNode[8];
		if( Distance( cur.x, cur.y, someone.targetX, someone.targetY ) <= 30 )
                        return;
		if( clock() - begin > 0.3 * CLOCKS_PER_SEC )
		{
			path.clear();
			return;
		}
                for( int i=0;i<8;i++ )
                {
			if( *quit )
			{
				path.clear();
				return;
			}
			newNode[i] = cur;
			newNode[i].cameFromX = cur.x;
			newNode[i].cameFromY = cur.y;
			int nodeOffset;
                        switch (i)
                        {
                                case 0 : nodeOffset=someone.speed;newNode[i].y-=nodeOffset;break;
                                case 1 : nodeOffset=someone.speed-someone.speed/3;newNode[i].y-=nodeOffset;newNode[i].x+=nodeOffset;break;
                                case 2 : nodeOffset=someone.speed;newNode[i].x+=nodeOffset;break;
                                case 3 : nodeOffset=someone.speed-someone.speed/3;newNode[i].y+=nodeOffset;newNode[i].x+=nodeOffset;break;
                                case 4 : nodeOffset=someone.speed;newNode[i].y+=nodeOffset;break;
                                case 5 : nodeOffset=someone.speed-someone.speed/3;newNode[i].y+=nodeOffset;newNode[i].x-=nodeOffset;break;
                                case 6 : nodeOffset=someone.speed;newNode[i].x-=nodeOffset;break;
                                case 7 : nodeOffset=someone.speed-someone.speed/3;newNode[i].y-=nodeOffset;newNode[i].x-=nodeOffset;break;
                        };
                        bool collision = false;
                        for( int j=0;j<roadblock.size();j++ )
			{
                                if( RectCollision( {roadblock[j].x, roadblock[j].y, roadblock[j].w, roadblock[j].h},  {newNode[i].x, newNode[i].y, someone.w, someone.h} ) )
				{
				       	collision = true;
					break;
				}
			}
                        for( int j=0;j<humans.size();j++ )
			{
                                if( RectCollision( {humans[j].x, humans[j].y, humans[j].w, humans[j].h}, {newNode[i].x, newNode[i].y, someone.w, someone.h} ) and humans[j].id != someone.id and humans[j].id != someone.targetID )
				{
				       	collision = true;
					break;
				}
			}
                        if( !collision )
                        {
                                newNode[i].value = /*cur.value +*/ Distance( newNode[i].x, newNode[i].y, someone.targetX, someone.targetY );
                                bool unreachedPlace = true;
                                for( int j=0;j<field.size();j++ )
                                {
                                        if( newNode[i].x == field[j].x and newNode[i].y == field[j].y )
                                        {
                                                unreachedPlace = false;
                                                if( field[j].value > newNode[i].value )
                                                {
                                                        field[j].value = newNode[i].value;
                                                        bucket.push( newNode[i] );
                                                }
                                                break;
                                        }
                                }
                                if( unreachedPlace )
                                {
                                        bucket.push( newNode[i] );
                                        field.push_back( newNode[i] );
                                }
                        }
                }
        }
}

void PathBuilder( human_t *someone, std::vector<flag_t> *navMesh, std::vector<human_t> humans, std::vector<obsticle_t> roadblock, bool *done, bool *quit)
{
	std::vector<node_t> path;
	node_t start;
	AStar( *someone, path, humans, roadblock, start, quit );
	node_t cur = start;
	node_t end = {someone->x, someone->y, someone->x, someone->y, 0};
	clock_t begin = clock();

	while(  !( cur.x == end.x and cur.y == end.y ) and !path.empty() )
	{
		if( clock() - begin > 0.7 * CLOCKS_PER_SEC )
		{
			path.clear();
			*done = true;
			return;
		}
		bool foundNext = false;
		for( int i=0;i<path.size();i++ )
		{
			if( path[i].x == cur.cameFromX and path[i].y == cur.cameFromY )
			{
				cur = path[i];
				flag_t toPush;
				toPush.ConstructFromNode_t(cur);
				navMesh->push_back( toPush );
				std::swap( path[i], path[path.size()-1] );
				path.pop_back();
				foundNext = true;
				break;
			}
			if( *quit )
			{
				path.clear();
				someone->navMesh.clear();
				*done = true;
				return;
			}
		}
		if( !foundNext )
		{
			path.clear();
			*done = true;
			return;
		}
	}
	path.clear();
	*done = true;
}
void CheckVision( human_t &someone, std::vector<human_t> humans )
{
//	if( someone.state == 2 )
//		return;
	SDL_Rect vision = { (someone.x - (someone.visionW - someone.w)/2), (someone.y - (someone.visionH - someone.h)/2), someone.visionW, someone.visionH }; 
	if( someone.prevDrawDirection == 'e' or someone.prevDrawDirection == 'w' )
		std::swap( vision.w, vision.h );
	for( int i=0;i<humans.size();i++ )
	{
		SDL_Rect possibleTarget = { humans[i].x, humans[i].y, humans[i].w, humans[i].h };
		if( RectCollision( vision, possibleTarget )  and humans[i].id != someone.id  and humans[i].state * someone.state < 0 )
		{
			someone.targetID = humans[i].id;
			someone.targetX = humans[i].x;
			someone.targetY = humans[i].y;
			someone.state = 2;
			for( int j=0;j<humans.size();j++ )
			{
				if( Distance(humans[j].x, humans[j].y, someone.x, someone.y) < 200 and humans[j].state == 1 )
				{
					humans[j].targetID = humans[i].id;
					humans[j].targetX = humans[i].x;
					humans[j].targetY = humans[i].y;
					humans[j].state = 2;
				}
			}
			return;
		}
	}
	//someone.state = 1;
}
