#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include "../utils/Vector.h"

//
// Receives information from clients
// puts it into a byte queue to be used in main
//

Vector* Server_Receiver_get_received();

void Server_Receiver_run(short port);
void Server_Receiver_stop();

#endif
