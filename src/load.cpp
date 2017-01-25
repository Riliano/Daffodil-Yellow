void ReadFromFile( char fileName[], char writeHere[1000][300], int &numLines )
{
	std::ifstream level( fileName );
	if( level.is_open() )
	{
		while( !level.eof() )
		{
			std::string line;
			std::getline( level, line );
			if( line[0] != '#' and line.length() > 2 )
			{
				for( int i=0;i<line.length();i++ )
					writeHere[numLines][i] = line[i];
				writeHere[numLines][line.length()] = '\0';
				numLines++;
			}
		}
	}
}

void RecieveFromNet( TCPsocket sock, char writeHere[1000][300], int &numLines )
{
	while( writeHere[numLines][0] != -1 )
	{
		SDLNet_TCP_Recv( sock, writeHere[numLines], 300 );
		if( writeHere[numLines][0] != -1 )
			numLines++;
	}
}

void LoadLevel( char level[1000][300], int numLines, float scale = 1 )
{
	humans.clear();
	roadblock.clear();
	avalSpells.clear();
	activeSpells.clear();
	std::vector< std::pair<int, int> > loadedTextures;
	
	for( int l=0;l<numLines;l++ )
	{
		int i=0;
		char type, fileName[20];
		while( level[l][i] == ' ' )
			i++;
		type = level[l][i];
		i++;
		while( level[l][i] == ' ' )
			i++;
		int f = 0;
		int pseudoHash = 0;
		while( level[l][i] != ' ' and type != 'p' )
		{
			fileName[f] = level[l][i];
			pseudoHash+=fileName[f]*(f+1);
			f++;
			i++;
		}
		fileName[f] = '\0';
		while( level[l][i] == ' ' )
			i++;
		std::vector<int> info;
		int num = 0;
		std::vector<int> bucket;
		int negativ = 1;
		for( ;level[l][i-1]!='\0';i++ )
		{
			if( level[l][i] == ' ' or level[l][i] == '\0' )
			{
				for( int b=0;b<bucket.size();b++ )
					num += bucket[b] * pow(10, (bucket.size()-1-b) );
				num*=negativ;
				info.push_back(num);
				num = 0;
				negativ = 1;
				bucket.clear();
				while( level[l][i] == ' ' and level[l][i]!='\0' )
					i++;
			}
			if( level[l][i] == '-' )
				negativ = -1;
			else
				bucket.push_back( level[l][i]-'0' );
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

			texture_t newTexture;
			newTexture.CreateFromInfo( fileName, textureIDToGive );
#ifdef RENDER
			newTexture.texture = IMG_LoadTexture( renderer, newTexture.fullFileName );
#endif
			textures.push_back( newTexture );

			loadedTextures.push_back( {pseudoHash, textureIDToGive} );
		}
		if( type == 'o' )
		{
			obsticle_t toPush;
			toPush.CreateFromInfo( info, textureIDToGive, scale );
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
			human_t toPush;
			toPush.CreateFromInfo( info, ++nextAvalHumanID, textureIDToGive, ++nextAvalThreadID, scale );
			humans.push_back( toPush );
			if( toPush.state == -1 )
				std::swap( humans[ humans.size()-1 ], humans[0] );
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
	for( int i=0;i<humans.size();i++ )
		humans[i].speed = humans[i].normSpeed;
	for( int i=0;i<avalSpells.size();i++ )
		avalSpells[i].id = i;
	for( int i=0;i<roadblock.size();i++ )
		roadblock[i].id = i;
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

