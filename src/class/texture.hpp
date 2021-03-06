#pragma once
//#include "../class.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <openssl/sha.h>
#include <fstream>

class textureBin_t
{
	public:
		int id;
		char *bin;
		unsigned int size;

		void LoadBin( const char *filename )
		{
			std::ifstream file;
			file.open( filename, std::ifstream::binary );
			if( file.is_open() )
			{
				file.seekg( 0, file.end );
				size = (int)file.tellg();
				file.seekg( 0, file.beg );
				bin = new char[ size ];
				file.read( bin, size );
				file.close();
			}
		}
		textureBin_t( int myID, const char *filename )
		{
			id = myID;
			LoadBin( filename );
		}
		textureBin_t()
		{}
};

class hash_t
{
	public:
		int id;
		unsigned char *hash;

		void Hash( char *bin, unsigned int size )
		{
			hash = new unsigned char[ SHA256_DIGEST_LENGTH ];
			SHA256_CTX ctx;
			SHA256_Init( &ctx );
			SHA256_Update( &ctx, bin, size );
			SHA256_Final( hash, &ctx );
		}

		hash_t( int myID, const textureBin_t &texture )
		{
			id = myID;
			Hash( texture.bin, texture.size );
		}
		hash_t( int myID, char *bin, unsigned int size )
		{
			id = myID;
			Hash( bin, size );
		}
		hash_t()
		{}
};

class texture_t
{
	public:
		int id;
		SDL_Texture *texture;

		texture_t( SDL_Renderer *renderer, int myID, const char *filename )
		{
			id = myID;
			texture = IMG_LoadTexture( renderer, filename );
		}
		texture_t( SDL_Renderer *renderer, int myID, const char *bin, unsigned int size )
		{
			SDL_RWops *rw = SDL_RWFromConstMem( bin, size );
			SDL_Surface *surface = IMG_LoadPNG_RW( rw );
			texture = SDL_CreateTextureFromSurface( renderer, surface );
			SDL_FreeSurface( surface );
			SDL_RWclose( rw );
		}
		texture_t()
		{}
};
