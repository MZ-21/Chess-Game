#include "thread.h"
#include "socketserver.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <iostream>

using namespace Sync;

class ThreadSocket : public Thread
{
public:
    ThreadSocket(Socket *socket_connect_req) : socket(socket_connect_req)
    {
    }

    long ThreadMain() override
    {
        ByteArray received_buffered_msg;
        int number_bytes_received = socket->Read(received_buffered_msg);
        std::string msg_client = received_buffered_msg.ToString();

 

        if (msg_client != "server")
        {

            // Write manipulated msg back to socket
            ByteArray buffered_msg("Waiting for another player");
            int success_writing = socket->Write(buffered_msg);
            // printf("%d number bytes written to client\n", success_writing);

            return success_writing;
        }
        else
        {
            // printf("user entered done, terminate");
            Bye();
            // printf("returning here");
            return 0;
        }
    }
    void Bye(void)
    {
        // terminationEvent.Wait();
        socket->Close(); // closing the connection
        delete socket;
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

int main(void)
{
    std::cout << "I am a server." << std::endl;
    try
    {
        SocketServer *s1 = new SocketServer(2000); // socket listening on port 2000
        while (true)
        {
            // Blocking call, it will not return until someone tries to open socket on port, returns socket

            Socket *socket_connect_req = new Socket(s1->Accept()); // socket of connection to client

            ThreadSocket *socket_thread = new ThreadSocket(socket_connect_req);

            long break_l = socket_thread->ThreadMain(); // calling thread main and overriding one in Thread class
            // std::cout << break_l << "this is break" << std::endl;
            if (break_l == 0)
            {
                break;
            }
        }
        s1->Shutdown(); // closing socket
    }
    catch (const std::exception &e)
    {
        // Handle std::exception and its subclasses
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
    }
}
