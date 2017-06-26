void LoadObjects( const char *filename, char type )
{
	std::ifstream file( filename );
	if( !file.is_open() )
	{
		std::cout<<"Error opening object file"<<std::endl;
		return;
	}

	while( !file.eof() )
	{
		std::string line;
		do
			std::getline( file, line );
		while( ( line[0] == '#' or line.length() < 2 ) and !file.eof() );

		std::stringstream ss( line );

		std::string name;
		std::string textureFileName;
		int textureID;
		ss>>name>>textureFileName;

		bool foundTexture = false;
		for( int i=0;i<textures.size();i++ )
		{
			if( textures[i].filename == textureFileName )
			{
				textureID = textures[i].id;
				foundTexture = true;
				break;
			}
		}
		if( !foundTexture )
		{
			texture_t newTexture( textureFileName.data(), textures.size() );//++nextAvalTextureID );
			textureID = newTexture.id;
			textures.push_back( newTexture );
		}
		
		int info[20];
		int i = 0;
		while( ss )
		{
			ss>>info[i];
			i++;
		}
		
		if( type == 'o' )
		{
			obsticle_t newRoadblock( name, info, textureID );
			roadblockTemplates.push_back( newRoadblock );
		}
		if( type == 'h' )
		{
			human_t newHuman( name, info, textureID );
			humanTemplates.push_back( newHuman );
		}
	}
}
void LoadLevel( const char *fileName, float scale = 1 )
{
	std::ifstream file( fileName );
	if( !file.is_open() )
	{
		std::cout<<"Error oppening level"<<std::endl;
		return;
	}

	while( !file.eof() )
	{
		std::string line;
		do
			std::getline( file, line );
		while( ( line[0] == '#' or line.length() < 2 ) and !file.eof() );
		if( file.eof() )
			break;

		std::stringstream ss( line );
		char action, type;

		ss>>action>>type;
		if( action == 'i' )
		{
			std::string fileName;
			ss>>fileName;
			if( type == 'o' or type == 'h' )
				LoadObjects( fileName.data(), type );
		}
		if( action == 'd' )
		{
			std::string name;
			ss>>name;
			if( type == 'o' )
			{
				int x, y;
				ss>>x>>y;
				for( int i=0;i<roadblockTemplates.size();i++ )
				{
					if( roadblockTemplates[i].name == name )
					{
						obsticle_t toDeploy = roadblockTemplates[i];
						toDeploy.Set( x, y );
						roadblock.push_back( toDeploy );
						break;
					}
				}
			}
			if( type == 'h' )
			{
				int x, y;
				ss>>x>>y;
				human_t toDeploy;
				for( int i=0;i<humanTemplates.size();i++ )
				{
					if( humanTemplates[i].name == name )
					{
						toDeploy = humanTemplates[i];
						break;
					}
				}
				toDeploy.Set( x, y );
				char player;
				ss>>player;
				if( player == 'p' )
				{
					int posx, posy;
					ss>>posx>>posy;
					toDeploy.SetPos( posx, posy );
				}
				humans.push_back( toDeploy );
				if( player == 'p' )
					std::swap( humans[0], humans[ humans.size()-1 ] );

			}
			if( type == 'b' )
			{
				//ss>>backgroundPos.x>>backgroundPos.y>>backgroundPos.w>>backgroundPos.h;
				//texture_t background( name.data(), ++nextAvalTextureID );
				//backgroundTextureID = background.id;
				//textures.push_back( background );
			}
		}
	}
}

/*
void LoadLevel( char levelFileName[], float scale = 1 )
{
	humans.clear();
	roadblock.clear();
	avalSpells.clear();
	activeSpells.clear();
	std::vector< std::pair<int, int> > loadedTextures;

	std::ifstream level( levelFileName );
	
	if( level.is_open() )
	{
		while( !level.eof() )
		{
			std::string line;
			do
				std::getline( level, line );
			while( (line[0] == '#' or line.length() < 2) and !level.eof() );
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
			for( ;line[i-1]!='\0';i++ )
			{
				if( line[i] == ' ' or line[i] == '\0' )
				{
					for( int b=0;b<bucket.size();b++ )
						num += bucket[b] * pow(10, (bucket.size()-1-b) );
					num*=negativ;
					info.push_back(num);
					num = 0;
					negativ = 1;
					bucket.clear();
					while( line[i] == ' ' and line[i]!='\0' )
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
				texture_t toPush( fileName, textureIDToGive );
				textures.push_back( toPush );

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
*/
