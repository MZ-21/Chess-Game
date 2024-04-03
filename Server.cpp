#include "thread.h"
#include "socketserver.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <iostream>

using namespace Sync;

class GameManager : public Thread{
    private:
    Socket& sock;
    bool& flag;

    public:
        GameManager(Socket& s, bool& f) : sock(s), flag(f){} // get access to socket and flag
        ~GameManager(){} // destructor to kill thread
    
    // manage thread from game
    virtual long ThreadMain() override{
        sock.Close(); // close connection
    }
};

int main(void)
{
    bool gameFlag = true;
    SocketServer sockServer(3000); // create server socket
    Socket gameSocket = sockServer.Accept(); // accepts connections to game
    GameManager gameThread(gameSocket, gameFlag);
    std::cout<<"I am server"<<std::endl;
    // run threadMain from GameManager
    while(true){
        sleep(1);
    }
    

}
