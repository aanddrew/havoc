#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

//
// Receives information from clients
// puts it into a byte queue to be used in main
//

void Server_Receiver_init(short port);
void Server_Receiver_deinit();

int Server_Receiver_getbytes(int client_id, Uint8* output, int size);
int Server_Receiver_queue_full_slots(int client_id);
int Server_Receiver_num_clients();
int Server_Receiver_is_client_active(int c);

void Server_Receiver_run();
void Server_Receiver_stop();

#endif
