#include "thread.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <iostream>

using namespace Sync;

class GameThread : public Thread{
    private:
    Socket& sock;
    bool& flag;

    public:
        GameThread(Socket& s, bool& f) : sock(s), flag(f){} // get access to socket and flag
        ~GameThread(){} // destructor to kill thread
    
    // manage thread from game
    virtual long ThreadMain() override{
        sock.Close(); // close connection
    }
};

int main(void)
{
    bool flag = true;
    Socket gameSock("127.0.0.1", 3000); // connect to server
    GameThread gThread(gameSock, flag);
    gameSock.Open(); // open socket
    std::cout<<"I am game thread"<<std::endl;
    // run threadMain
    while(flag){
		sleep(1);
	}

	gameSock.Close(); // close socket
	return 0;
}
