#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <vector>
#include "class/human.hpp"
#include "class/texture.hpp"

class load_t
{
	public:
		std::unordered_set < std::string > textureFilenames;
		std::vector < int* > humanBlueprints;

		int InsertTexture( std::string filename )
		{
			textureFilenames.insert( filename );
			auto target = textureFilenames.find( filename );
			int id = 0;
			for( auto it = textureFilenames.begin(); it != target; it++ )
				id++;
			return id;
		}

		void LoadTextures( textureBin_t *textures, hash_t texturesHashes = nullptr )
		{
			textures = new textureBin_t [ textureFilenames.size() ];
			if( texturesHashes != nullptr )
				texturesHashes = new hash_t [ textureFilenames.size() ];
			int i=0;
			for( auto it = textureFilenames.begin();it!=textureFilenames.end();it++ )
			{
				textures[i].LoadBin( it->data() );
				textures[i].id = i;
				if( texturesHashes != nullptr )
				{
					texturesHashes[i].Hash( textures[i].bin, textures[i].size );
					texturesHashes[i].id = i;
				}
				i++;
			}
		}
		void LoadHumanBlueprints( humanBlueprint_t *loadBlueprints )
		{
			loadBlueprints = new humanBlueprint_t [ humanBlueprints.size() ];
			for( int i=0;i<humanBlueprints.size();i++ )
				loadBlueprints[i].CreateFromArray( humanBlueprints[i] );
		}

		load_t( const char *filename )
		{
			std::ifstream file( filename );
			while( !file.eof() )
			{
				std::string line;
				do
					std::getline( file, line );
				while( ( line[0] == '#' or line.length() < 2 ) and !file.eof() );
				std::stringstream ss( line );

				char operation;
				ss>>operation;
				// Load an object
				if( operation == 'l' )
				{
					char type;
					ss >> type;
					int info[10]; // Save information to a net message-like array
					while ( ss )
					{
						char parameter;
						ss >> parameter;
						switch ( parameter )
						{
							// texture
							case 't' : 
							{
								std::string textureName;
								ss>>textureName;
								info[ POS_TEXTUREID ] = InsertTexture( textureName );
								break;
							}
							// size(dimensions)
							case 's' : ss >> info[ POS_SIZE_W ] >> info[ POS_SIZE_H ];break;
							// health
							case 'h' : ss >> info[ POS_HP ];break;
							case 'f' : ss >> info[ POS_FRAME_W ] >> info[ POS_FRAME_H ];break;
							case 'v' : ss >> info[ POS_SPEED ];break;
							case 'p' : ss >> info[ POS_PLAYABLE ];break;
						}
					}

					switch ( type )
					{
						case 'h' : 
							info[ POS_ID ] = humanBlueprints.size();
							humanBlueprints.push_back( info );
							break;
					}
					type = 0;
				}
			}
		}
};
