#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

void Client_Receiver_init();
void Client_Receiver_deinit();

Uint8* Client_Receiver_getbytes(int size);

void Client_Receiver_run();
void Client_Receiver_stop();

#endif
