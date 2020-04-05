#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include "Client_Pool.h"

void Client_Receiver_init();
void Client_Receiver_deinit();

Vector* Client_Receiver_get_received();

int Client_Receiver_getbytes(Uint8* output, int size);
int Client_Receiver_queue_full_slots();

void Client_Receiver_run();
void Client_Receiver_stop();

#endif
