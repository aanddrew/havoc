#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

//
// Sends information to each client that is connected
//

void Server_Sender_init();
void Server_Sender_deinit();

void Server_Sender_add_client(IPaddress addr);

void Server_Sender_run();
void Server_Sender_stop();

#endif
