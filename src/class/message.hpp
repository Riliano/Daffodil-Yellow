#pragma once

const int MSG_META_ID = 0;
const int MSG_META_LIST_TEXTURES = 1;
const int MSG_META_LIST_HUMAN_TEMPLATES = 2;
const int MSG_META_LIST_PLAYERS = 3;
const int MSG_META_LIST_ROADBLOCK_TEMPLATES = 4;
const int MSG_META_LIST_ROADBLOCKS = 5;
const int MSG_META_END_ASSET_DATA = 6;
const int MSG_META_TEXTURE = 10;
const int MSG_META_POSITION_HUMANS = 20;
const int MSG_META_REMOVE_HUMAN = 21;
const int MSG_META_NEW_HUMAN = 22;
const int MSG_META_REMOVE_ROADBLOCK = 30;
/*
const int MSG_META_NAME = 0;
const int MSG_META_REQ_TEXTURE = 1;
const int MSG_META_REQ_HUMAN_TEMPLATES = 2;
const int MSG_META_REQ_ROADBLOCK_TEMPLATES = 3;
const int MSG_META_CHOSEN_CHARACKTER = 4;
const int MSG_META_INPUT = 10;
*/

class msgMeta_t
{
	public:
		unsigned char type;
		unsigned int size;

		msgMeta_t( unsigned char myType, unsigned int mySize )
		{
			type = myType;
			size = mySize;
		}
};
