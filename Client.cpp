#include "socket.h"
#include "thread.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include "Game/chess.h"

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
// wait for the other player
// if there is another player, start game
				ByteArray msg_rcv;
				int number_bytes_received = sock1->Read(msg_rcv);
        		std::string msg_server = msg_rcv.ToString();
				if(msg_server != "start"){
					std::cout << msg_server << std::endl;
					number_bytes_received = sock1->Read(msg_rcv);
        			msg_server = msg_rcv.ToString();
					std::cout << msg_server << std::endl;
				}
				sock1->Write(nameUser);
				number_bytes_received = sock1->Read(msg_rcv);
        		msg_server = msg_rcv.ToString();
				std::cout << "Your opponent is here"<< std::endl;
				std::cout << msg_server << std::endl;
				Board b;
				b.setBoard();
				
				std::string move;
				int x1, x2, y1, y2;
				bool stop = false;
				// while (!stop)
				// {
				// 	(turn == WHITE) ? std::cout << "White's turn" << std::endl : std::cout << "Black's turn" << std::endl;
				// 	std::cout << "Type in your move as a single four character string. Use x-coordinates first in each pair." << endl;
				// 	std::cin >> move;
				// 	x1 = move[0] - 48;
				// 	y1 = move[1] - 48;
				// 	x2 = move[2] - 48;
				// 	y2 = move[3] - 48;
				// 	if (b.getSquare(x1, y1)->b.getColor() == turn)
				// 	{


				// 		if (b.makeMove(x1, y1, x2, y2) == false)
				// 		{
				// 			std::cout << "Invalid move, try again." << std::endl;
				// 		}
				// 		else
				// 			stop = true;
				// 	}
				// 	else
				// 		std::cout << "That's not your piece. Try again." << std::endl;
				// }

				//b.doMove();

				// do move
				//bool test = b.playGame();

                // ByteArray *byteA = new ByteArray(nameUser);//sending name user to server
                // int number_bytes_written = sock1->Write(*byteA);

		
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

				if(msg_server == "Ready Game!"){
					
				}
			}
		//}
	}
	catch (const std::exception &e)
	{
		// Handle std::exception and its subclasses
		std::cerr << "Standard exception caught: " << e.what() << std::endl;
	}
}
