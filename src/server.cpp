#include<iostream>
#include<SDL2/SDL_net.h>
#include<thread>

TCPsocket server;
SDLNet_SocketSet allConnectedSockets;

struct client_t
{
	human_t human;
	int templateID;
	TCPsocket socket;
	bool active = false;

	bool ready = false;

	bool wantsHumanTemplates = false;
	bool wantsRoadblockTemplates = false;
	std::queue<int> wantsTextureID;

	void Update( int *input )
	{
		// User has send his input
		human.movDirection[0] = input[0];
		human.movDirection[1] = input[1];
		human.attDirection = input[2];
		active = true;
	}

	void SetSocket( TCPsocket sck )
	{
		socket = sck;
	}
	void MakeHumanWithTemplate( humanTemplate_t *chosenTemplate )
	{
		human.Deploy( chosenTemplate, 0, 0 ); // Still needs updating
		// Spawn zone?
	}
	void MakeHumanWithTemplateID( int myTemplateID )
	{
		templateID = myTemplateID;
		MakeHumanWithTemplate( &humanTemplates[ myTemplateID ] );
	}
	void FindTemplateID()
	{
		for( int i=0;i<humanTemplates.size();i++ )
		{
			if( &humanTemplates[i] == human.humanTemplate )
			{
				templateID = i;
				break;
			}
		}
	}

	client_t( TCPsocket sck )//, humanTemplate_t templt )
	{
		socket = sck;
	}
	client_t( TCPsocket sck, humanTemplate_t *chosenTempalte )
	{
		socket = sck;
		human.Deploy( chosenTempalte, 0, 0 ); //Needs updating
	}
	client_t()
	{}
};

std::vector< client_t > clients;
std::vector< obsticle_t > objects;

void StartServer( Uint16 port, int serverSize )
{
	IPaddress ip;
	SDLNet_Init();
    std::cout<<"Started server on port: "<<port<<" with size: "<<serverSize<<std::endl;
    SDLNet_ResolveHost( &ip, NULL, port );
    allConnectedSockets = SDLNet_AllocSocketSet( serverSize );
    server = SDLNet_TCP_Open( &ip );
	SDLNet_TCP_AddSocket( allConnectedSockets, server );
}

#include "load.cpp"
#include "move.cpp"
//#incluide "ai.cpp"

void NetRecieve( client_t &sender, int id );
void NetSendPos();
void NewClient( TCPsocket socket );
void NetSendNewPlayer( client_t *clientToSend, int id );
void CheckForReady( client_t *sender, int id );

void ServerMain( Uint16 port = DEFAULT_PORT, int serverSize = DEFAULT_SERVER_SIZE )
{
	std::string level = "Levels/1.lvl";
	bool succesfulLoad = LoadLevel( level.data() );
	if( !succesfulLoad )
		return;

	int *msgptr = nullptr;
	int msgListHumanTemplates[ humanTemplates.size()*8 ];//human template has 8 necessary values
	msgptr = msgListHumanTemplates;
	for( int i=0;i<humanTemplates.size();i++ )
	{
		humanTemplates[i].MakeMesage( msgptr );
		msgptr += 8;
	}

	std::cout<<"Server: Done with loading"<<std::endl;
	StartServer( port, serverSize );

	long long movT = SDL_GetTicks();
	long long chkNetT = SDL_GetTicks();
	long long sendNetPosT = SDL_GetTicks();
	long long infoT = SDL_GetTicks();
	int cycles = 0;
	std::cout<<"Server: Entering main loop"<<std::endl;
	serverIsOn = true;
	while( true )
	{
		// Check for messeges send to server
		if( SDL_GetTicks() - chkNetT >= 0 )
		{
			int numActive = SDLNet_CheckSockets( allConnectedSockets, 0 );
			if( numActive > 0 )
			{
				if( SDLNet_SocketReady( server ) )
				{
					numActive--;
					std::cout<<"server: New client"<<std::endl;
					NewClient( SDLNet_TCP_Accept( server ) );
				}
			}
			for( int i=0;i<clients.size() and numActive > 0;i++ )
			{
				if( SDLNet_SocketReady( clients[i].socket ) )
				{
					numActive--;
					NetRecieve( clients[i], i );
				}
			}
			chkNetT = SDL_GetTicks();
		}
		// Send position messeges from server
		if( SDL_GetTicks() - sendNetPosT >= 10 )
		{
			NetSendPos();
			sendNetPosT = SDL_GetTicks();
		}

		// Update movement
		if( SDL_GetTicks() - movT >= 1000/60 )
		{
			for( int i=0;i<clients.size();i++ )
			{
				//humans[i].DrawDir();
				MoveHuman( clients[i].human );
				clients[i].active = false;
			}
			movT = SDL_GetTicks();
		}

		cycles++;
		// Debug or general info goes here
		if( SDL_GetTicks() - infoT >= 1000 )
		{
			//std::cout<<"Cycles per second: "<<cycles<<std::endl;
			cycles = 0;
			infoT = SDL_GetTicks();
		}
	}
}

void NewClient( TCPsocket socket )
{
	SDLNet_TCP_AddSocket( allConnectedSockets, socket );

	client_t newClient;
	newClient.SetSocket( socket );
	if( playableTemplates.size() == 1 )
		newClient.MakeHumanWithTemplateID( playableTemplates[0] );
	Uint8 meta[2] = {MSG_META_END_ASSET_DATA, 1};
	int message[] = {true};
	SDLNet_TCP_Send( socket, meta, 2 );
	SDLNet_TCP_Send( socket, message, meta[1]*4 );
	clients.push_back( newClient );

	/*
	Uint8 meta[2] = {MSG_META_ID, 1};
	int message[] = {(int)clients.size()};
	SDLNet_TCP_Send( socket, meta, 2 );
	SDLNet_TCP_Send( socket, message, meta[1]*4 );
	*/
}
void RemoveClient( client_t &someone )
{
	SDLNet_TCP_DelSocket( allConnectedSockets, someone.socket );
}
void NetRecieve( client_t &sender, int id )
{
	Uint8 meta[2];
	int recv = SDLNet_TCP_Recv( sender.socket, meta, 2 );
	if( recv <= 0 )
	{
		std::cout<<"User has disconnected"<<std::endl;
		RemoveClient( sender );
		return;
	}

	int msg[meta[1]];
	int size = meta[1]*4;
	int fetched = 0;
	while( fetched < size )
		fetched = SDLNet_TCP_Recv( sender.socket, msg+fetched, size-fetched );

	if( meta[0] == MSG_META_NAME )
	{
		// User has send his username
		//for( int i=0;i<meta[1];i++ )
		//	sender.name[i]=msg[i];
	}
	if( meta[0] == MSG_META_REQ_TEXTURE )
		for( int i=0;i<meta[1];i++ )
			sender.wantsTextureID.push( msg[i] );
	if( meta[0] == MSG_META_REQ_HUMAN_TEMPLATES )
	{
		sender.wantsHumanTemplates = msg[0];
		CheckForReady( &sender, id );
	}
	if( meta[0] == MSG_META_REQ_ROADBLOCK_TEMPLATES )
	{
		sender.wantsRoadblockTemplates = msg[0];
		CheckForReady( &sender, id );
	}
	if( meta[0] == MSG_META_END_ASSET_DATA )
		sender.ready = msg[0];
	if( meta[0] == MSG_META_CHOSEN_CHARACKTER )
	{
		sender.templateID = msg[0];
		CheckForReady( &sender, id );
	}
	if( meta[0] == MSG_META_INPUT )
		sender.Update( msg );
}
void CheckForReady( client_t *sender, int id )
{
	//TODO
	Uint8 meta[2] = {MSG_META_END_ASSET_DATA, 1};
	int message[] = {true};
	SDLNet_TCP_Send( sender->socket, meta, 2 );
	SDLNet_TCP_Send( sender->socket, message, meta[1]*4 );
	NetSendNewPlayer( sender, id );
}
void NetSendPos()
{
	int humanPosMsg[ 800 ];
	Uint8 humanPosMsgLen = 0;
	for( int i=0;i<clients.size();i++ )
	{
		if( clients[i].active )
		{
			humanPosMsg[ humanPosMsgLen ] = i;
			humanPosMsg[ humanPosMsgLen+1 ] = clients[i].human.pos.x;
			humanPosMsg[ humanPosMsgLen+2 ] = clients[i].human.pos.y;
			humanPosMsg[ humanPosMsgLen+3 ] = (int)(clients[i].human.drawDirection);
			clients[i].active = false;
			humanPosMsgLen += 4;
		}
	}
	if( humanPosMsgLen > 0 )
	{
		Uint8 posMeta[2] = {MSG_META_POSITION_HUMANS, humanPosMsgLen};
		for( int i=0;i<clients.size();i++ )
		{
			SDLNet_TCP_Send( clients[i].socket, posMeta, 2 );
			SDLNet_TCP_Send( clients[i].socket, humanPosMsg, humanPosMsgLen*4 );
		}
	}
}
void NetSendTexture()
{

}
void NetSendPlayerTemplates()
{

}
void NetSendPlayerList( TCPsocket socket )
{
	int message[ clients.size()*4 ];
	int msgSize=0;
	for( int id=0;id<clients.size();id++ )
	{
		message[msgSize] = id;
		message[msgSize+1] = clients[id].templateID;
		message[msgSize+2] = clients[id].human.pos.x;
		message[msgSize+3] = clients[id].human.pos.y;
		msgSize+=4;
	}
	Uint8 meta[2] = {MSG_META_LIST_PLAYERS, (Uint8)msgSize};

	SDLNet_TCP_Send( socket, meta, 2 );
	SDLNet_TCP_Send( socket, message, meta[1]*4 );
}
void NetSendNewPlayer( client_t *clientToSend, int id )
{
	Uint8 meta[2] = {MSG_META_NEW_HUMAN, 4};
	int message[] = {id, clientToSend->templateID, (int)clientToSend->human.pos.x, (int)clientToSend->human.pos.y};
	
	for( int i=0;i<clients.size();i++ )
	{
		SDLNet_TCP_Send( clients[i].socket, meta, 2 );
		SDLNet_TCP_Send( clients[i].socket, message, meta[1]*4 );
	}
}
