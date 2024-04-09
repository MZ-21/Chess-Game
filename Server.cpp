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

std::mutex mtx; // Mutex for thread synchronization
std::condition_variable cv; // Condition variable for notifying
std::vector<Socket*> clientSockets; // Vector to store client sockets

Semaphore semTurn("Turn", 1, true); // create semaphore to write

class GameManager : public Thread {
    private:
    // Socket& sock;
    // bool& flag;
    Socket *player1, *player2;

    public:
        // GameManager(Socket& s, bool& f) : sock(s), flag(f){} // get access to socket and flag
        ~GameManager(){} // destructor to kill thread
        GameManager(Socket *p1, Socket *p2) : player1(p1), player2(p2) {}
    
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
            if(!player1->GetOpen()){
                std::cout << "player1 has disconnected" << std::endl;
                player2->Write(p1_end_msg);
                break;
            }
            if(player1_move.ToString() == "finished"){
                break;
            }
            player2->Write(player1_move);
            ByteArray player2_move;
            player2->Read(player2_move);
            ByteArray p2_end_msg("player2 has disconnected\nGame Over\nYou Win!");
            if(!player2->GetOpen()){
                std::cout << "player2 has disconnected" << std::endl;
                player1->Write(p2_end_msg);

                break;

            }
            if(player2_move.ToString() == "finished"){
                break;
            }
            player1->Write(player2_move);
            // if(player2_move.ToString() !="player2 has disconnected\nGame Over\nYou Win!"){
            //     player1->Write(end_msg)
            // }
            // break;
        }

        //Game logic here
        player1->Close();
        player2->Close();
        delete player1;
        delete player2;
        return 0;
        //sock.Close(); // close connection
    }
};

class ServerShutDown : public Thread {
    private:
        SocketServer & serverSocket;

    public:
        // GameManager(Socket& s, bool& f) : sock(s), flag(f){} // get access to socket and flag
        ServerShutDown(SocketServer & t): Thread(true), serverSocket(t){} // destructor to kill thread
    
        // manage thread from game
        virtual long ThreadMain() override{
            std::cout << "\nType 'quit' when you want to close the server" << std::endl;
            for (;;)
            {
                std::string s;
                std::cin >> s;
                if(s=="quit")
                {
                    serverSocket.Shutdown();
                    break;
                }
            }
        
        }
};


int main(void) {
    SocketServer sockServer(3000); // create server socket
    ServerShutDown ServerShutDown(sockServer);

    std::cout << "I am server" << std::endl;

    try {
        while (true) {
            
                Socket* newClientSocket = new Socket(sockServer.Accept());
            

                // Protecting the access to clientSockets vector with a mutex
                {
                    std::lock_guard<std::mutex> lock(mtx);  // Protect the clientSockets vector
                    clientSockets.push_back(newClientSocket);
                }

                if(clientSockets.size()% 2 != 0){
                    std::cout << "Client connected!" << std::endl;
                    ByteArray msg_confirming("Are you ready to play? (Enter yes): ");
                    newClientSocket->Write(msg_confirming);

                    ByteArray clientmsg;
                    newClientSocket->Read(clientmsg);

                    if(!newClientSocket->GetOpen()){
                        std::lock_guard<std::mutex> lock(mtx);  // Protect the clientSockets vector again
                        auto it = std::find_if(clientSockets.begin(), clientSockets.end(), [&](Sync::Socket* socket) {
                            return socket == newClientSocket;
                        });

                        if (it != clientSockets.end()) {
                            clientSockets.erase(it);  // Remove the socket from the vector
                        }
                    }
                }
                //ThreadSocket* comms = new ThreadSocket(newClientSocket, clientSockets.size());
                // Check if we have two clients to start a game
                if (clientSockets.size() >= 2) {
                    //std::cout << "found 2" << std::endl;
                    // Create a game session with the first two clients
                    GameManager* game = new GameManager(clientSockets[0], clientSockets[1]);
                    game->Start(); // Start game session in a new thread

                    // Remove the clients from the waiting list
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        clientSockets.erase(clientSockets.begin(), clientSockets.begin() + 2);
                    }
                }
                else{
                    ByteArray msg_to_client("waiting for opponent");
                    newClientSocket->Write(msg_to_client);
                }
        }
     catch (const std::exception &e)
    {
        // Handle std::exception and its subclasses
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
    }
     std::cout << "End of main" << std::endl;
        for (int i=0;i<clientSockets.size();i++)
            delete clientSockets[i];
        }
  
}