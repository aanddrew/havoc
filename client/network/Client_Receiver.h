#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

void Client_Receiver_init(short port);
void Client_Receiver_deinit();

void Client_Receiver_run();
void Client_Receiver_stop();

#endif
