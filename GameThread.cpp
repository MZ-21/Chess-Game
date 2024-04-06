#include "thread.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <iostream>

using namespace Sync;

class GameThread : public Thread //how to link game thread with the chess logic?
{
private:
    // Socket& sock;
    Socket &player1;
    Socket &player2;
    bool &flag;

public:
    // GameThread(Socket& s, bool& f) : sock(s), flag(f){} // get access to socket and flag
    // ~GameThread(){} // destructor to kill thread
    GameThread(Socket &s1, Socket &s2, bool &f) : player1(s1), player2(s2), flag(f) {}

    ~GameThread()
    {
        // Ensure both sockets are closed when the game thread is destroyed
        player1.Close();
        player2.Close();
    }

    // manage thread from game
    virtual long ThreadMain() override
    {
        while (flag)
        {
            // Game logic here
            // For example, read data from each socket (player1 and player2)

            // loop until game is over
                // give player1 mutex to make move (player2 is blocked)
                // receive input and process (maybe use sock.Read())
                // check if move is possible, let both know the move (sock.Write())
                // take mutex from player 1 and give to player2 (player1 is blocked)
                // receive input and process (sock.Read())

            // Process the game logic based on data received
            // Respond to each player based on game state

            // Pseudo-code example:
            // if (player1 sent "forfeit" or player2 sent "forfeit")
            //     flag = false; // End the game loop

            // if (game condition met)
            //     flag = false; // End the game loop
        }

        // Close connections at the end of the game
        player1.Close();
        player2.Close();

        return 0;

        // sock.Close(); // close connection
    }
};

int main(void)
{
    bool flag = true;
    Socket player1Sock("127.0.0.1", 3000); // Example socket for player 1
    Socket player2Sock("127.0.0.1", 3001); // Example socket for player 2
   //Socket gameSock("127.0.0.1", 3000); // connect to server

    player1Sock.Open(); // Open socket for player 1
    player2Sock.Open(); // Open socket for player 2

    // GameThread gThread(gameSock, flag);
    // gameSock.Open(); // open socket

    GameThread gThread(player1Sock, player2Sock, flag);

    gThread.Start(); // Start the game thread

    std::cout << "I am game thread" << std::endl;
    
    // run threadMain
    while (flag){
        sleep(1);
    }

    //gameSock.Close(); // close socket
    // Cleanup
    player1Sock.Close(); // Close player 1 socket
    player2Sock.Close(); // Close player 2 socket
    return 0;
}
