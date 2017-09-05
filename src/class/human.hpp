#pragma once

#include <string>
//#include "../class.hpp"
#include "basic.hpp"

const int POS_ID = 0;
const int POS_TEXTUREID = 1;
const int POS_SIZE_W = 2;
const int POS_SIZE_H = 3;
const int POS_SPEED = 4;
const int POS_HP = 5;
const int POS_FRAME_W = 6;
const int POS_FRAME_H = 7;
const int POS_PLAYABLE = 8;

class humanBlueprint_t
{
	public:

		int id;
		int textureID;
		std::string name;

		dimensions_t dimensions;

		float maxSpeed;
		int maxHp;

		dimensions_t textureFrame;

		bool playable;

		void CreateFromArray( int *arr )
		{
			id = arr[POS_ID];
			textureID = arr[POS_TEXTUREID];
			dimensions.Set( arr[POS_SIZE_W], arr[POS_SIZE_H] );
			maxSpeed = arr[POS_SPEED];
			maxHp = arr[POS_HP];
			textureFrame.Set( arr[POS_SIZE_W], arr[POS_FRAME_H] );
			playable = arr[POS_PLAYABLE];
		}
		void WriteToArray( int *arr )
		{
			arr[POS_ID] = id;
			arr[POS_TEXTUREID] = textureID;
			arr[POS_SIZE_W] = dimensions.width;
			arr[POS_SIZE_H] = dimensions.height;
			arr[POS_SPEED] = maxSpeed;
			arr[POS_HP] = maxHp;
			arr[POS_SIZE_W] = textureFrame.width;
			arr[POS_FRAME_H] = textureFrame.height;
			arr[POS_PLAYABLE] = playable;
		}

		humanBlueprint_t ( int *arr )
		{
			CreateFromArray( arr );
		}
		humanBlueprint_t()
		{}

};
