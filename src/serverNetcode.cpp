#include "server.hpp"
void server_t::NewClient( TCPsocket socket )
{
	std::cout<<"New client\n";
	user_t newUser( users.size(), socket );
	users.push_back( newUser );
}
void server_t::NetSendTexture( user_t &reciever, int textureID )
{
	textureBin_t *texture = textures + textureID;
	msgMeta_t meta( MSG_META_TEXTURE, texture->size );
	SDLNet_TCP_Send( reciever.socket, &meta, 5 );
	SDLNet_TCP_Send( reciever.socket, texture, texture->size );
}
void server_t::NetRecieve( user_t &sender )
{
	msgMeta_t meta;
	int recv = SDLNet_TCP_Recv( sender.socket, &meta, 5 );
	if( recv <= 0 )
	{
		//RemoveClient( sender );
		return;
	}

	void *message;
	if( meta.size > 0 )
	{
		message = malloc( meta.size );
		SDLNet_TCP_Recv( sender.socket, message, meta.size );
	}
	switch( meta.type )
	{
		case MSG_META_REQ_TEXTURE : 
			{
				// listTextures should have only one number
				int *listTextures = static_cast<int*>( message );
				NetSendTexture( sender, *listTextures );
				break;
			}
	//		case MSG_META_REQ_HUMAN_TEMPLATES 
	}
}

