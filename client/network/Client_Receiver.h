#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include "Client_Pool.h"

void Client_Receiver_init();
void Client_Receiver_deinit();

Vector* Client_Receiver_get_received();

void Client_Receiver_run();
void Client_Receiver_stop();

#endif
