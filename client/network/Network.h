#ifndef NETWORK_H
#define NETWORK_H

void Network_init();
void Network_deinit();

int Network_connect(const char* hostname);
void Network_disconnect();

#endif
