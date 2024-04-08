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

using namespace Sync;

std::mutex mtx; // Mutex for thread synchronization
std::condition_variable cv; // Condition variable for notifying
std::vector<Socket*> clientSockets; // Vector to store client sockets

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
            if(player1_move.ToString() == "finished"){
                break;
            }
            player2->Write(player1_move);

            ByteArray player2_move;
            player2->Read(player2_move);
            player1->Write(player2_move);
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

// class ThreadSocket : public Thread
// {
// public:
//     int number_of_players;
// public:
//     ThreadSocket(Socket *socket_connect_req, const int &num_players) : socket(socket_connect_req){
//         number_of_players = num_players;
//     }
//     int getNumPlayers(){
//         return number_of_players;
//     }
//     int setNumberPlayers(const int &num_p){
//         number_of_players = num_p;
//     }

//     long ThreadMain() override {
//         // Read a message from the client
//         ByteArray received_buffered_msg;
//         int number_bytes_received = socket->Read(received_buffered_msg);
//         std::string msg_client = received_buffered_msg.ToString();

 
//         // Check the message content
//         if (msg_client != "server"){
//             // // Write manipulated msg back to socket
//             // ByteArray buffered_msg("Waiting for another player");
//             // int success_writing = socket->Write(buffered_msg);
//             // // printf("%d number bytes written to client\n", success_writing);
//             // return success_writing;
            
//             // Notify the client that we're waiting for another player
//             ByteArray buffered_msg("Waiting for another player");
//             socket->Write(buffered_msg);
//         }
//         else
//         {
//             // printf("user entered done, terminate");
//             Bye();
//             // printf("returning here");
//             //return 0;
//         }
//         // Return the number of bytes received or processed
//         return number_bytes_received;
//     }
//     void Bye(){
//         // terminationEvent.Wait();
//         // socket->Close(); // closing the connection
//         // delete socket;
//         // Close the socket and release resources
//         if (socket) {
//             socket->Close();
//             delete socket;
//             socket = nullptr; // Prevent double deletion and undefined behavior
//         }
//     }
//     ~ThreadSocket()
//     {
//         // waiting on Sync::Event termination event to make a block
//         Bye();
//     }

// private:
//     Socket *socket;
//     // If manipulateString is defined outside, you may need to declare it here or include its header
// };


int main(void) {
    SocketServer sockServer(3000); // create server socket
    std::cout << "I am server" << std::endl;

    try {
        while (true) {
            Socket* newClientSocket = new Socket(sockServer.Accept());
            std::cout << "Client connected!" << std::endl;

            // Protecting the access to clientSockets vector with a mutex
            {
                std::lock_guard<std::mutex> lock(mtx);  // Protect the clientSockets vector
                clientSockets.push_back(newClientSocket);
            }

            //ThreadSocket* comms = new ThreadSocket(newClientSocket, clientSockets.size());
            // Check if we have two clients to start a game
            if (clientSockets.size() >= 2) {
                std::cout << "found 2" << std::endl;
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
    } catch (const std::exception &e) {
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
        // Clean up any remaining sockets
        for (auto* socket : clientSockets) {
            socket->Close();
            delete socket;
        }
        sockServer.Shutdown(); // Shutdown the server on exception
    }
    return 0;
}