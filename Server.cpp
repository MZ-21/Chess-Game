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
        GameManager(Socket *p1, Socket *p2) : player1(p1), player(p2) {}
    
    // manage thread from game
    virtual long ThreadMain() override{
        //Game logic here
        player1->Close();
        player2->Close();
        delete player1;
        delete player2;
        return 0;
        //sock.Close(); // close connection
    }
}

class ThreadSocket : public Thread
{
public:
    ThreadSocket(Socket *socket_connect_req) : socket(socket_connect_req){}

    long ThreadMain() override {
        // Read a message from the client
        ByteArray received_buffered_msg;
        int number_bytes_received = socket->Read(received_buffered_msg);
        std::string msg_client = received_buffered_msg.ToString();

 
        // Check the message content
        if (msg_client != "server"){
            // // Write manipulated msg back to socket
            // ByteArray buffered_msg("Waiting for another player");
            // int success_writing = socket->Write(buffered_msg);
            // // printf("%d number bytes written to client\n", success_writing);
            // return success_writing;
            
            // Notify the client that we're waiting for another player
            ByteArray buffered_msg("Waiting for another player");
            socket->Write(buffered_msg);
        }
        else
        {
            // printf("user entered done, terminate");
            Bye();
            // printf("returning here");
            //return 0;
        }
        // Return the number of bytes received or processed
        return number_bytes_received;
    }
    void Bye(){
        // terminationEvent.Wait();
        // socket->Close(); // closing the connection
        // delete socket;
        // Close the socket and release resources
        if (socket) {
            socket->Close();
            delete socket;
            socket = nullptr; // Prevent double deletion and undefined behavior
        }
    }
    ~ThreadSocket()
    {
        // waiting on Sync::Event termination event to make a block
        Bye();
    }

private:
    Socket *socket;
    // If manipulateString is defined outside, you may need to declare it here or include its header
};

int main(void) {
    //bool gameFlag = true;
    SocketServer sockServer(3000); // create server socket
    //Socket gameSocket = sockServer.Accept(); // accepts connections to game
    //GameManager gameThread(gameSocket, gameFlag);
    std::vector<Socket*> clientSockets;
    std::cout<<"I am server"<<std::endl;
    
    // run threadMain from GameManager
    while(true){
        sleep(1);
    }

    //std::cout << "I am a server." << std::endl;
    try{
        //SocketServer *s1 = new SocketServer(2000); // socket listening on port 2000
        while (true){
            // Blocking call, it will not return until someone tries to open socket on port, returns socket
            Socket* clientSockets = new Socket(server.Accept());
            std::cout << "Client connected!" << std::endl;
            clientSockets.push_back(clientSockets);

            //Check if we have two clients
            if (clientSockets.size() >= 2) {
                //Create a game session with the first two clients
                GameManager* game = new GameManager(*clientSocket[0], *client[1]);
                game->Start(); //Start game session in new thread

                //Remove the client from the waiting list
                clientSockets.erase(clientSockets.begin());
                clientSockets.erase(clientSockets.begin());
            }

            // Socket *socket_connect_req = new Socket(s1->Accept()); // socket of connection to client

            // ThreadSocket *socket_thread = new ThreadSocket(socket_connect_req);

            // long break_l = socket_thread->ThreadMain(); // calling thread main and overriding one in Thread class
            // // std::cout << break_l << "this is break" << std::endl;
            // if (break_l == 0)
            // {
            //     break;
            // }
        }
        //s1->Shutdown(); // closing socket
    }
    catch (const std::exception &e) {
        // Handle std::exception and its subclasses
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
        //Clean up any remaining sockets
        for (auto* socket : clientSockets) {
            socket->Close();
            delete socket;
        }
        server.Shutdown(); // Shutdown the server on exception
    }
    return 0;
}
