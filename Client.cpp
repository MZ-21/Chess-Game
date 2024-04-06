#include "socket.h"
#include "thread.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <limits>

using namespace Sync;

int main(void)
{
	std::cout << "I am a client" << std::endl;
	std::string inputString;
	std::string ipAdr = "127.0.0.1";
	unsigned int port = 3000;

	try
	{
		// user input
		//while (true) // should we have a while loop here?
		//{
            std::string nameUser;
            
			std::cout << "Do you want to play chess? (enter done/server to close connection/server): ";
			std::cin >> inputString;

            Socket *sock1 = new Socket(ipAdr, port); // socket on port 2000

            if((inputString == "y" || inputString == "yes")){
                
                std::cout << "Enter your name: ";
			    std::cin >> nameUser;

                sock1->Open(); // attempting to connect to server

                ByteArray *byteA = new ByteArray(nameUser);//sending name user to server
                int number_bytes_written = sock1->Write(*byteA);

		
            }

			if (inputString == "done" || inputString == "server")
			{
				printf("User entered done/server, closing socket");
				sock1->Close();
				//break;
			}
			else
			{
				// wait for message back from server
				ByteArray *buff_server_msg = new ByteArray();
				int number_bytes_rec_server = sock1->Read(*buff_server_msg);

				std::string msg_server= buff_server_msg->ToString();
				std::cout << " \nMsg from server " << msg_server << std::endl;
			}
		//}
	}
	catch (const std::exception &e)
	{
		// Handle std::exception and its subclasses
		std::cerr << "Standard exception caught: " << e.what() << std::endl;
	}
}
