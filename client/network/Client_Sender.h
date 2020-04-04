#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

#include <SDL2/SDL_net.h>

void Client_Sender_init(unsigned short port);
void Client_Sender_deinit();

void Client_Sender_connect(const char* host);

void Client_Sender_run();
void Client_Sender_stop();

#endif
