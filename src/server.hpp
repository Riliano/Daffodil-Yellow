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

	void NetRecieve( user_t );
//	void NetSendPos();
//	void NewClient( TCPsocket );
//	void NetSendNewPlayer( player_t );
//	void NetSendTexture( user_t, textureBin_t );
//	void NetSendTextureList( user_t );

	Uint16 port = DEFAULT_PORT;
	unsigned int serverSize = DEFAULT_SERVER_SIZE;

	TCPsocket server;
	SDLNet_SocketSet allSockets;

	textureBin_t *textures;
	hash_t *texturesHashes;
	humanBlueprint_t *humanBlueprints;
	std::vector < user_t > users;

	void StartServer()
	{
		IPaddress ip;
		SDLNet_Init();
		std::cout<<"Starting server on port: "<<port<<" with size: "<<serverSize<<"\n";
		SDLNet_ResolveHost( &ip, NULL, port );
		allSockets = SDLNet_AllocSocketSet( serverSize );
		server = SDLNet_TCP_Open( &ip );
		SDLNet_TCP_AddSocket( allSockets, server );
	}

	void Load( const char *level )
	{
		load_t load( level );
		load.LoadHumanBlueprints( humanBlueprints );
		load.LoadTextures( textures, texturesHashes );
		std::cout<<"Server: Done with loading\n";
	}

	void ServerMain()
	{
		std::string level = "Levels/1.lvl";
			
		StartServer();

		std::cout<<"Server: Entering main loop\n";
		while( true )
		{

		}
	}

	public:
	void Start()
	{
		StartServer();
		Load( "Levels/1.lvl" );// Change
		ServerMain();
	}
};


void NetRecieve( user_t &sender )
{
	msgMeta_t meta;
	int recv = SDLNet_TCP_Recv( sender.socket, &meta, 5 );
	if( recv <= 0 )
	{
		//RemoveClient( sender );
		return;
	}

	void *message;
}
