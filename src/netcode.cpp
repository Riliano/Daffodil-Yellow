#include<SDL2/SDL_net.h>

const int DEFAULT_SERVER_SIZE = 1000;

TCPsocket server;
SDLNet_SocketSet allConnectedSockets;

void StartServer( int serverSize = DEFAULT_SERVER_SIZE, Uint16 port = DEFAULT_PORT )
{
    std::cout<<"Started servere on port: "<<port<<" with size: "<<serverSize<<std::endl;
    SDLNet_ResolveHost( &ip, NULL, port );
    allConnectedSockets = SDLNet_AllocSocketSet( serverSize );
    server = SDLNet_TCP_Open( &ip );
	SDLNet_TCP_AddSocket( allConnectedSockets, server );
}

