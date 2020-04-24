#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "ByteQueue.h"
#include "Client_Pool.h"
#include "Client_Receiver.h"

#include "../../utils/Vector.h"

static SDL_Thread *thread;

void Client_Receiver_init() {}

void Client_Receiver_deinit() {}

Vector *Client_Receiver_get_received() {
  SDL_LockMutex(shared_pool.received_mutex);
  Vector *temp = shared_pool.received;
  shared_pool.received = shared_pool.received_swap;
  shared_pool.received_swap = temp;
  SDL_UnlockMutex(shared_pool.received_mutex);
  return shared_pool.received_swap;
}

void clone_packet(UDPpacket *source, UDPpacket *dest, int offset) {
  dest->len = source->len + offset;
  for (int i = 0; i < source->len; i++) {
    dest->data[i + offset] = source->data[i];
  }
  dest->address = source->address;
  dest->status = source->status;
  dest->channel = source->channel;
}

static int running = 1;

static int thread_fun(void *arg) {
  pool_t *pool = (pool_t *)arg;

  UDPpacket *pack = SDLNet_AllocPacket(PACKET_SIZE);
  while (running) {
    // get a packet from the server
    SDL_LockMutex(shared_pool.server_mutex);
    int numrecv = SDLNet_UDP_Recv(shared_pool.server, pack);
    SDL_UnlockMutex(shared_pool.server_mutex);

    // add it to the received vector
    if (numrecv) {
      SDL_LockMutex(shared_pool.received_mutex);
      UDPpacket *clone = SDLNet_AllocPacket(PACKET_SIZE);
      clone_packet(pack, clone, 0);
      Vector_push(shared_pool.received, clone);
      SDL_UnlockMutex(shared_pool.received_mutex);
    }

    SDL_LockMutex(pool->running_mutex);
    running = pool->running;
    SDL_UnlockMutex(pool->running_mutex);
  }
  printf("Client_Receiver stopped\n");
  return 0;
}

void Client_Receiver_run() {
  thread = SDL_CreateThread(thread_fun, "receiver_thread", &shared_pool);
}

void Client_Receiver_stop() {
  int value;
  SDL_WaitThread(thread, &value);
}
