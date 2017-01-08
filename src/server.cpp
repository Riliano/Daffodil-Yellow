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
}clients[200];
int nextAvalID = -1;
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

	while( true )
	{
		int activeSockets = SDLNet_CheckSockets( allSockets, 0 );

		if( SDLNet_SocketReady( server ) )
		{
			activeSockets--;
			TCPsocket newSocket = SDLNet_TCP_Accept( server );
			if( newSocket )
			{
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
				}else
				{
					clients[i].x = msg[1];
					clients[i].y = msg[2];
					int info[4] = {clients[i].id, clients[i].x, clients[i].y};
					for( int j=0;j<num_clients;j++ )
					{
						if( clients[i].id!=clients[j].id )
							SDLNet_TCP_Send( clients[j].socket, info, 16 );
					}
				}
			}
		}
	}
}
