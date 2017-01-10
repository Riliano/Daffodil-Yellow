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

