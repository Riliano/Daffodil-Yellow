#pragma once

#include <vector>
#include <SDL2/SDL_net.h>
#include "class/basic.hpp"

class user_t
{
	public:
		// ID
		int id;
		TCPsocket socket;
		// Request
		bool requestHash;
		std::vector<int> requestTextures;
		// Status
		bool ready;
		bool active;

		user_t( const TCPsocket &mySocket, int myID )
		{
			socket = mySocket;
			id = myID;
			ready = false;
			active = false;
			requestHash = true;
		}

};

class player_t
{
	public:
		int id;
		int blueprintID;

		point_t pos;
		int hp;
		float speed;
};
