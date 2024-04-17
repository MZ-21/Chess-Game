#include "thread.h"
#include "socketserver.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "SharedObject.h"
#include "Semaphore.h"
#include <algorithm>
using namespace Sync;

std::vector<Socket*> clientSockets; // Vector to store client sockets
int gameCounter = 0;

class GameManager : public Thread {
    private:
    bool& flag;
    Socket *player1, *player2;

    public:
        ~GameManager(){} // destructor to kill thread
        GameManager(Socket *p1, Socket *p2, bool & f, bool & g) : player1(p1), player2(p2), flag(f) {}
    
    // manage thread from game
    virtual long ThreadMain() override{
        ByteArray msg("start");
        player1->Write(msg);
        player2->Write(msg);

        ByteArray clientmsg;
        player1->Read(clientmsg);
        player2->Read(clientmsg);

        ByteArray msg1("you will play as White");
        ByteArray msg2("you will play as Black");
        player1->Write(msg1);
        player2->Write(msg2);

        while(true){
            ByteArray player1_move;
            player1->Read(player1_move);
            ByteArray p1_end_msg("player1 has disconnected\nGame Over\nYou Win!");

            // handle player1 disconnecting
            if(!player1->GetOpen()){
                std::cout << "player1 has disconnected" << std::endl;
                player2->Write(p1_end_msg);
                sleep(1); // let client finish first before closing
                player2->Close();
                delete player1;
                delete player2;
                gameCounter--;
                std::cout << "Number of Games: " << gameCounter << std::endl;
                return 0;
            }

            player2->Write(player1_move);
            ByteArray player2_move;
            player2->Read(player2_move);
            ByteArray p2_end_msg("player2 has disconnected\nGame Over\nYou Win!");

            // handle player2 disconnecting
            if(!player2->GetOpen()){
                std::cout << "player2 has disconnected" << std::endl;
                player1->Write(p2_end_msg);
                sleep(1); // let client finish first before closing
                player1->Close();
                delete player1;
                delete player2;
                gameCounter--;
                std::cout << "Number of Games: " << gameCounter << std::endl;
                return 0;
            }
            player1->Write(player2_move);
        }

        //Game logic here
        player1->Close();
        player2->Close();
        delete player1;
        delete player2;
        gameCounter--;
        std::cout << "Number of Games: " << gameCounter << std::endl;
        return 0;
    }
};

class ServerShutDown : public Thread {
    private:
        SocketServer & serverSocket;
        bool & flag;

    public:
        ServerShutDown(SocketServer & t, bool & f): Thread(true), serverSocket(t), flag(f){} // destructor to kill thread
    
        // manage thread from game
        virtual long ThreadMain() override{
            std::cout << "\nType 'quit' when you want to close the server" << std::endl;
            for (;;)
            {
                std::string s;
                std::cin >> s;
                if(s=="quit")
                {
                    flag = false; // signal serverThread
                    std::cout << "Server will shutdown soon" << std::endl;
                    break;
                }
            }
            return 0;
        }
};

class ServerThread : public Thread {
    private:
        SocketServer & sockServer;
        bool & flag;
        bool & isServerOn;

    public:
        ServerThread(SocketServer & t, bool & f, bool & sf): Thread(true), sockServer(t), flag(f), isServerOn(sf){} // destructor to kill thread
    
        // manage thread from game
        virtual long ThreadMain() override{
        try {
            while(true){
                Socket* newClientSocket = new Socket(sockServer.Accept());
                
                // add the clients to the waiting list
                clientSockets.push_back(newClientSocket);

                std::cout << "length: " << clientSockets.size() << std::endl;
                if(clientSockets.size()% 2 != 0){
                    std::cout << "Client connected!" << std::endl;
                    ByteArray msg_confirming("Are you ready to play? (Enter yes): ");
                    newClientSocket->Write(msg_confirming);

                    ByteArray clientmsg;
                    newClientSocket->Read(clientmsg);

                    if(!newClientSocket->GetOpen()){
                        auto it = std::find_if(clientSockets.begin(), clientSockets.end(), [&](Sync::Socket* socket) {
                            return socket == newClientSocket;
                        });

                        if (it != clientSockets.end()) {
                            clientSockets.erase(it);  // Remove the socket from the vector
                        }
                    }
                }
                
                // Check if we have two clients to start a game
                if (clientSockets.size() >= 2) {
                    // Create a game session with the first two clients
                    GameManager* game = new GameManager(clientSockets[0], clientSockets[1], flag, isServerOn);
                    game->Start(); // Start game session in a new thread
                    gameCounter++;
                    std::cout << "Number of Games: " << gameCounter << std::endl;

                    // Remove the clients from the waiting list
                    clientSockets.erase(clientSockets.begin(), clientSockets.begin() + 2);

                    if(!flag){ // stop accepting connections
                    std::cout << "No more connections will be accepted" << std::endl;
                    isServerOn = false;
                        break;
                    }
                }
                else{
                    ByteArray msg_to_client("waiting for opponent");
                    newClientSocket->Write(msg_to_client);
                }
            }
        }
            catch (const std::exception &e)
            {
                // Handle std::exception and its subclasses
                std::cerr << "Standard exception caught: " << e.what() << std::endl;
            }
            return 0;
        }
};


int main(void) {
    SocketServer sockServer(3000); // create server socket
    bool flag = true;
    bool isServerOn = true;
    std::cout << "I am server" << std::endl;
    ServerShutDown* serverShutDown = new ServerShutDown(sockServer, flag);
    ServerThread* server = new ServerThread(sockServer, flag, isServerOn);

    while (isServerOn) {
        sleep(1);
    }
    // wait for server thread to finish
    while (gameCounter > 0) {
        sleep(1);
    }

    // shutdown server
    std::cout << "End of main" << std::endl;
    std::cout << "server has shut down" << std::endl;
    sockServer.Shutdown();

    // termination of server
    delete serverShutDown;
    delete server;
    std::cout << "THE END" << std::endl;

    return 0;
}