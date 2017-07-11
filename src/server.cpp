#include<iostream>
#include<SDL2/SDL_net.h>
#include<thread>

TCPsocket server;
SDLNet_SocketSet allConnectedSockets;

struct client_t
{
	human_t human;
	TCPsocket socket;
	bool active;

	client_t( TCPsocket sck )//, humanTemplate_t templt )
	{
		socket = sck;
//		human = MakeHuman( templt );
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

void NetRecieve( client_t &sender );
void NetSend();
void NewClient( TCPsocket socket );

void ServerMain( Uint16 port = DEFAULT_PORT, int serverSize = DEFAULT_SERVER_SIZE )
{

	std::string level = "Levels/1.lvl";
	LoadLevel( level.data() );
	std::cout<<"Server: Done with loading"<<std::endl;
	StartServer( port, serverSize );

	long long movT = SDL_GetTicks();
	long long chkNetT = SDL_GetTicks();
	long long sendNetT = SDL_GetTicks();
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
					NetRecieve( clients[i] );
				}
			}
			chkNetT = SDL_GetTicks();
		}
		// Send messeges from server
		if( SDL_GetTicks() - sendNetT >= 10 )
		{
			NetSend();
			sendNetT = SDL_GetTicks();
		}

		// Update movement
		if( SDL_GetTicks() - movT >= 1000/60 )
		{
			for( int i=0;i<clients.size();i++ )
			{
				//humans[i].DrawDir();
				MoveHuman( clients[i].human );
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
	client_t newClient( socket );//, humanTemplates[0] );
	clients.push_back( newClient );

//	human_t newPlayer( socket );
	//	humans.push_back( newPlayer );
	///REMOVE
	Uint8 meta[2] = {7, 1};
	int message[] = {true};
	SDLNet_TCP_Send( socket, meta, 2 );
	SDLNet_TCP_Send( socket, message, meta[1]*4 );
}
void RemoveClient( client_t &someone )
{
	SDLNet_TCP_DelSocket( allConnectedSockets, someone.socket );
}
void NetRecieve( client_t &sender )
{
	Uint8 meta[2];
	int recv = SDLNet_TCP_Recv( sender.socket, meta, 2 );
	if( recv <= 0 )
	{
		std::cout<<"User has disconnected"<<std::endl;
		RemoveClient( sender );
		return;
	}
	
	char msg[meta[1]];
	int fetched = 0;
	while( fetched < meta[1] )
		fetched = SDLNet_TCP_Recv( sender.socket, msg+fetched, meta[1]-fetched );
	if( meta[0] == 0 )
	{
		// User has send his username
		//for( int i=0;i<meta[1];i++ )
		//	sender.name[i]=msg[i];
	}
	if( meta[0] == 1 )
	{
		// User has requested textures
	}
	if( meta[0] == 2 )
	{
		// User had choosen a charackter
	}
	if( meta[0] == 10 )
	{
		// User has send his input
		//sender.human.movDirection[0] = msg[0];
		//sender.human.movDirection[1] = msg[1];
		//sender.human.attDirection = msg[2];
		//sender.active = true;
	}
}
void NetSend()
{
	/*
	int humanPosMsg[ 800 ];
	Uint8 humanPosMsgLen=0;
	for( int i=0;i<humans.size();i++ )
	{
		if( humans[i].active )
		{
			humanPosMsg[ humanPosMsgLen ] = humans[i].id;
			humanPosMsg[ humanPosMsgLen+1 ] = humans[i].pos.x;
			humanPosMsg[ humanPosMsgLen+2 ] = humans[i].pos.y;
			humanPosMsg[ humanPosMsgLen+3 ] = humans[i].drawDirection;
			humans[i].active = false;
			humanPosMsgLen += 4;
		}
	}
	if( humanPosMsg > 0 )
	{
		Uint8 posMeta[2] = {10, humanPosMsgLen};
		for( int i=0;i<humans.size();i++ )
		{
			SDLNet_TCP_Send( humans[i].socket, posMeta, 2 );
			SDLNet_TCP_Send( humans[i].socket, humanPosMsg, humanPosMsgLen*4 );
		}
	}
	*/
}
