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
		// Controlled players
		std::vector<int> contolledPlayers;

		user_t( int myID,  const TCPsocket &mySocket )
		{
			socket = mySocket;
			id = myID;
			ready = false;
			active = false;
			requestHash = false;
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
