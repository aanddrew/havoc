#ifndef NETWORK_H
#define NETWORK_H

void Network_init();
void Network_deinit();
void Network_connect(const char* addr);
void Network_send();

#endif
