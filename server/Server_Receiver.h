#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include "../utils/Vector.h"

//
// Receives information from clients
//

//when this method is called it returns the vector of packets receieved
//from the clients and swaps the internal arrays the receiver is using to
//receive packets. 
//
//This method should be called, then the vector should be emptied before the 
//method is called again do this this swapping structure
Vector* Server_Receiver_get_received();

void Server_Receiver_run(short port);
void Server_Receiver_stop();

#endif
