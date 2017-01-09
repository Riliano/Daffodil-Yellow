#include<iostream>
#include<algorithm>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>

TCPsocket server;
struct client_t
{
	TCPsocket socket;
	int x, y;
	int id;
	bool activity;
}clients[200];
int nextAvalID = 0;
int num_clients;
SDLNet_SocketSet allSockets;

int main()
{
	SDLNet_Init();

	IPaddress ip;
	Uint16 port = 1234;

	SDLNet_ResolveHost( &ip, NULL, port );
	server = SDLNet_TCP_Open( &ip );

	allSockets = SDLNet_AllocSocketSet( 200 );
	SDLNet_TCP_AddSocket( allSockets, server );

	long long sendT = SDL_GetTicks();

	std::cout<<"Start"<<std::endl;

	while( true )
	{
		int activeSockets = SDLNet_CheckSockets( allSockets, 0 );

		if( SDLNet_SocketReady( server ) )
		{
			activeSockets--;
			TCPsocket newSocket = SDLNet_TCP_Accept( server );
			if( newSocket )
			{
				std::cout<<"New"<<std::endl;
				clients[num_clients].socket = newSocket;
				clients[num_clients].id = ++nextAvalID;

				int msg[num_clients+1] = {clients[num_clients].id};
				int msgLen = 1;
				for( int i=0;i<num_clients;i++ )
				{
					msg[msgLen] = clients[i].id;
					msg[msgLen+1] = clients[i].x;
					msg[msgLen+2] = clients[i].y;
					msgLen+=3;
				}
				msg[msgLen] = -1;
				msgLen++;
				for( int i=0;i<msgLen;i++ )
					std::cout<<msg[i]<<" ";
				std::cout<<std::endl;

				SDLNet_TCP_Send( clients[num_clients].socket, msg, 4*msgLen );
				SDLNet_TCP_AddSocket( allSockets, clients[num_clients].socket );

				int othrmsg[5] = {-1, clients[num_clients].id, clients[num_clients].x, clients[num_clients].y};
				for( int i=0;i<num_clients;i++ )
					SDLNet_TCP_Send( clients[i].socket, othrmsg, 20 );
				num_clients++;
			}
		}

		for( int i=0;i<num_clients and activeSockets>0;i++ )
		{
			if( SDLNet_SocketReady( clients[i].socket ) )
			{
				activeSockets--;

				int msg[20];
				int res = SDLNet_TCP_Recv( clients[i].socket, msg, 20 );
				if( res <=0 )
				{
					SDLNet_TCP_DelSocket( allSockets, clients[i].socket );
					SDLNet_TCP_Close( clients[i].socket );
					std::swap( clients[i], clients[num_clients-1] );
					num_clients--;
					if( num_clients == 0 )
						nextAvalID = 0;
				}else
				{
					for( int i=0;i<num_clients;i++ )
					{
						if( clients[i].id == msg[0] )
						{
							clients[i].x = msg[1];
							clients[i].y = msg[2];
							clients[i].activity = true;
							break;
						}
					}
					
				}
			}
		}
		
		if( SDL_GetTicks() - sendT >= 10 )
		{
			int msg[300];
			int msgLen = 0;
			for( int i=0;i<num_clients;i++ )
			{
				if( clients[i].activity )
				{
					msg[msgLen] = clients[i].id;
					msg[msgLen+1] = clients[i].x;
					msg[msgLen+2] = clients[i].y;
					clients[i].activity = false;
					msgLen+=3;
				}
			}
			msg[msgLen] = -1;
			msgLen++;
			if( msgLen > 1 )
			{
				for( int j=0;j<num_clients;j++ )
				{
					SDLNet_TCP_Send( clients[j].socket, msg, msgLen*4 );
				}
			}
			sendT = SDL_GetTicks();
		}

	}
}
