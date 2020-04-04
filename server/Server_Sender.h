#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

void Server_Sender_init(unsigned short port);
void Server_Sender_deinit();

void Server_Sender_add_client(IPaddress addr);

void Server_Sender_run();
void Server_Sender_stop();

#endif
