#pragma once

#include <vector>
#include<SDL2/SDL_net.h>
#include "class/basic.hpp"

class user_t
{
	public:
		// ID
		int id;
		int blueprintID;
		TCPsocket socket;
		// Request
		bool requestHash;
		std::vector<int> requestTextures;
		// Status
		bool ready;
		bool active;
		// Game
		point_t pos;
		int hp;
		float speed;
};
