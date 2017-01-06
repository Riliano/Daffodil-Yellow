default:
	g++ -o Game src/main.cpp -std=c++11 -pthread -lSDL2 -lSDL2_image -lSDL2_net
server:
	g++ -o server src/server.cpp -std=c++11 -lSDL2 -lSDL2_net
