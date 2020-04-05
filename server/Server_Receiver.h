#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

void Server_Receiver_init(short port);
void Server_Receiver_deinit();

int Server_Receiver_getbytes(Uint8* output, int size);
int Server_Receiver_queue_full_slots();

void Server_Receiver_run();
void Server_Receiver_stop();

#endif
