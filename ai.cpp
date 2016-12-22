void AStar( human_t &someone, std::vector<node_t> &path, std::vector<human_t> &humans, std::vector<obsticle_t> &roadblock, node_t &start )
{
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

		path.push_back( cur );
                bucket.pop();
                node_t newNode[8];
		if( Distance( cur.x, cur.y, someone.targetX, someone.targetY ) <= 50 )
		{
			start = cur;
                        break;
		}
                for( int i=0;i<8;i++ )
                {
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

void PathBuilder( human_t &someone, std::vector<human_t> &humans, std::vector<obsticle_t> &roadblock )
{
	std::vector<node_t> path;
	node_t start;
	AStar( someone, path, humans, roadblock, start );
	node_t cur = start;
	node_t end = {someone.x, someone.y, someone.x, someone.y, 0};

	while(  !( cur.x == end.x and cur.y == end.y ) and !path.empty() )
	{
		for( int i=0;i<path.size();i++ )
		{
			if( path[i].x == cur.cameFromX and path[i].y == cur.cameFromY )
			{
				cur = path[i];
				flag_t toPush(cur);
				someone.navMesh.push_back( toPush );
				std::swap( path[i], path[path.size()-1] );
				path.pop_back();
				break;
			}
		}
	}
	path.clear();
}
