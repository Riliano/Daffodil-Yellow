#include<algorithm>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>

TCPsocket server;
TCPsocket clients[200];
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
				clients[num_clients] = newSocket;
				SDLNet_TCP_AddSocket( allSockets, clients[num_clients] );
				num_clients++;
			}
		}

		for( int i=0;i<num_clients and activeSockets>0;i++ )
		{
			if( SDLNet_SocketReady( clients[i] ) )
			{
				activeSockets--;

				char msg[20];
				int res = SDLNet_TCP_Recv( clients[i], msg, 20 );
				if( res <=0 )
				{
					SDLNet_TCP_DelSocket( allSockets, clients[i] );
					SDLNet_TCP_Close( clients[i] );
					std::swap( clients[i], clients[num_clients-1] );
					num_clients--;
				}else
				{
					for( int j=0;j<num_clients;j++ )
						if( i!= j )
							SDLNet_TCP_Send( clients[j], msg, res );
				}
			}
		}
	}
}
