all: Client Server ChessGame

Client : Client.o thread.o socket.o Blockable.o
	g++ -o Client Client.o socket.o thread.o Blockable.o -pthread -l rt

Client.o : Client.cpp socket.h
	g++ -c Client.cpp -std=c++11

Server : Server.o thread.o socket.o socketserver.o Blockable.o
	g++ -o Server Server.o thread.o socket.o socketserver.o Blockable.o -pthread -l rt

Blockable.o : Blockable.h Blockable.cpp
	g++ -c Blockable.cpp -std=c++11

Server.o : Server.cpp thread.h socketserver.h
	g++ -c Server.cpp -std=c++11

thread.o : thread.cpp thread.h
	g++ -c thread.cpp -std=c++11

ChessGame: Game/main.o Game/chess.o
	g++ -o ChessGame Game/main.o Game/chess.o -std=c++11

socket.o : socket.cpp socket.h
	g++ -c socket.cpp -std=c++11

socketserver.o : socketserver.cpp socket.h socketserver.h
	g++ -c socketserver.cpp -std=c++11

GameThread : GameThread.o thread.o socket.o Blockable.o
	g++ -o GameThread GameThread.o socket.o thread.o Blockable.o -pthread -l rt

GameThread.o : GameThread.cpp socket.h
	g++ -c GameThread.cpp -std=c++11

GameThread : GameThread.o thread.o socket.o Blockable.o
	g++ -o GameThread GameThread.o socket.o thread.o Blockable.o -pthread -l rt

GameThread.o : GameThread.cpp socket.h
	g++ -c GameThread.cpp -std=c++11

Game/main.o: Game/main.cpp Game/chess.h Game/chess.cpp
	g++ -c Game/main.cpp -std=c++11