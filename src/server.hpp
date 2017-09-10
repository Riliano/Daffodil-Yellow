#pragma once

#include <iostream>
#include <SDL2/SDL_net.h>

#include "load.hpp"
#include "user.hpp"

#include "class/human.hpp"
#include "class/texture.hpp"
#include "class/message.hpp"

class server_t
{
	const Uint16 DEFAULT_PORT = 1234;
	const unsigned int DEFAULT_SERVER_SIZE = 32;
	TCPsocket server;
	SDLNet_SocketSet allSockets;

	textureBin_t *textures;
	hash_t *texturesHashes;
	humanBlueprint_t *humanBlueprints;
	std::vector < user_t > users;

	void StartServer( Uint16 port, unsigned int serverSize )
	{
		IPaddress ip;
		SDLNet_Init();
		std::cout<<"Starting server on port: "<<port<<" with size: "<<serverSize<<"\n";
		SDLNet_ResolveHost( &ip, NULL, port );
		allSockets = SDLNet_AllocSocketSet( serverSize );
		server = SDLNet_TCP_Open( &ip );
		SDLNet_TCP_AddSocket( allSockets, server );
	}

	void ServerMain( Uint16 port = DEFAULT_PORT, unsigned int serverSize = DEFAULT_SERVER_SIZE )
	{
		std::string level = "Levels/1.lvl";
		load_t load( level.data() );
		// Disassemble load
		// Human blueprints
		load.LoadHumanBlueprints( humanBlueprints );
		load.LoadTextures( textures, texturesHashes );
		
		std::cout<<"Server: Done with loading\n";
		StartServer( port, serverSize );

		std::cout<<"Server: Entering main loop\n";
		while( true )
		{

		}
	}
}
