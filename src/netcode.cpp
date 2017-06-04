#include<SDL2/SDL_net.h>

const int DEFAULT_PORT = 1234;
const int DEFAULT_SERVER_SIZE = 1000;

IPaddress ip;
TCPsocket server;
SDLNet_SocketSet allConnectedSockets;

TCPsocket client;
SDLNet_SocketSet chkClient = SDLNet_AllocSocketSet( 1 );


void StartServer( int serverSize = DEFAULT_SERVER_SIZE, Uint16 port = DEFAULT_PORT )
{
    std::cout<<"Started servere on port: "<<port<<" with size: "<<serverSize<<std::endl;
    SDLNet_ResolveHost( &ip, NULL, port );
    allConnectedSockets = SDLNet_AllocSocketSet( serverSize );
    server = SDLNet_TCP_Open( &ip );
	SDLNet_TCP_AddSocket( allConnectedSockets, server );
}

void ConnectToServer( std::string address = "localhost", Uint16 port = DEFAULT_PORT )
{
    std::cout<<"Connecting to: "<<address<<" on port "<<port<<std::endl;
    SDLNet_ResolveHost( &ip, address.data(), port );	
    client = SDLNet_TCP_Open( &ip );
    if( !client )
        std::cout<<"Failed to connect"<<std::endl;
    else
        SDLNet_TCP_AddSocket( chkClient, client );
}