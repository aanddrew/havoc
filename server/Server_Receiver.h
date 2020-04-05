#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

void Server_Receiver_init(short port);
void Server_Receiver_deinit();

void Server_Receiver_run();
void Server_Receiver_stop();

#endif
