#include <stdio.h>

#include <SDL2/SDL_net.h>

#include "../utils/Vector.h"

#include "Server_Pool.h"
#include "Server_Receiver.h"
#include "Server_Sender.h"

#include "../game/Game.h"
#include "../game/Player.h"
#include "../game/Projectile.h"
#include "../utils/Network_utils.h"

#define HAVOC_SERVER

static int delta = 10; // 10 ms between update times

#define DISCONNECT_TIMEOUT 5.0f // SECONDS

int main() {
  SDL_Init(0);
  SDLNet_Init();
  Pool_init();

  Game_init();

  Server_Receiver_run(23432);
  Server_Sender_run();

  int last_time = SDL_GetTicks();
  while (1) {
    int current_time = SDL_GetTicks();
    // update loop
    while (current_time > last_time + delta) {
      Vector *received = Server_Receiver_get_received();
      while (received->num > 0) {
        UDPpacket *pack = Vector_pop(received);
        printf("received: ");
        print_packet(pack);
        printf("\n");

        int id = SDLNet_Read32(pack->data);
        int type = SDLNet_Read32(pack->data + 4);

        if (Player_get(id)) {
          Player_get(id)->time_since_last_message = 0.0f;
        }

        switch (type) {
        case CONNECT_REQUEST:
          printf("reconnection request\n");
          if (!Player_get(id)) {
            printf("actually reconnecting\n");
            Player_reconnect(id);
          }

          {
            // send their id back to them
            UDPpacket *id_packet = SDLNet_AllocPacket(4);
            SDLNet_Write32(id, id_packet->data);
            id_packet->len = 4;
            id_packet->address = pack->address;

            SDL_LockMutex(shared_pool.server_mutex);
            SDLNet_UDP_Send(shared_pool.server, id, id_packet);
            SDL_UnlockMutex(shared_pool.server_mutex);

            SDLNet_FreePacket(id_packet);
          }
          break;
        case PLAYER_UPDATE:
          if (!Player_get(id)) {
            Player_connect_with_id("unknown_player", id);
            Player_get(id)->team = id;
          }
          if (Player_get(id)->is_alive) {
            Network_decipher_player_packet(pack, Player_get(id), 1);
          }
          SDLNet_FreePacket(pack);
          pack = NULL;
          break;
        case CHANGE_NAME:
          Network_decipher_change_name_packet(pack);
          SDLNet_FreePacket(pack);
          pack = NULL;
          break;
        case PROJECTILE_LAUNCH:
          if (Player_get(id) && Player_get(id)->is_alive) {
            Network_decipher_projectile_packet(pack, NULL);
          } else {
            SDLNet_FreePacket(pack);
            pack = NULL;
          }
          break;
        case GET_NAMES:
          SDLNet_FreePacket(pack);
          pack = Network_create_receive_names_packet();
          break;
        default:
          SDLNet_FreePacket(pack);
          pack = NULL;
          break;
        }

        if (pack) {
          SDL_LockMutex(shared_pool.sending_mutex);
          Vector_push(shared_pool.sending, pack);
          SDL_UnlockMutex(shared_pool.sending_mutex);
        }
      }

      float dt = ((float)delta) / 1000.0f;
      Game_update(dt);

      // send player information back to the clients
      for (int i = 0; i < Player_num_players(); i++) {
        Player *p = Player_get(i);
        if (p) {
          // send position velocity health to clients
          UDPpacket *pack = Network_create_player_packet(p);
          UDPpacket *copy = SDLNet_AllocPacket(pack->maxlen + 4);
          SDLNet_Write32(i, copy->data);
          for (int i = 0; i < pack->len; i++) {
            copy->data[i + 4] = pack->data[i];
          }
          copy->len = pack->len + 4;
          SDLNet_FreePacket(pack);

          SDL_LockMutex(shared_pool.sending_mutex);
          Vector_push(shared_pool.sending, copy);
          SDL_UnlockMutex(shared_pool.sending_mutex);

          // if the player is dead tell clients
          if (!p->is_alive && p->just_died) {
            p->just_died = 0;

            UDPpacket *deathpack = Network_create_player_die_packet(i);

            SDL_LockMutex(shared_pool.sending_mutex);
            Vector_push(shared_pool.sending, deathpack);
            SDL_UnlockMutex(shared_pool.sending_mutex);
          }
          // if the player should respawn tell the clients
          if (p->just_respawned) {
            p->just_respawned = 0;
            UDPpacket *spawnpack = Network_create_player_spawn_packet(i, p);

            SDL_LockMutex(shared_pool.sending_mutex);
            Vector_push(shared_pool.sending, spawnpack);
            SDL_UnlockMutex(shared_pool.sending_mutex);
          }

          // if the client should disconnect tell clients
          p->time_since_last_message += dt;
          if (p->time_since_last_message >= DISCONNECT_TIMEOUT) {
            UDPpacket *dcpack = Network_create_player_disconnect_packet(i);

            Player_disconnect(i);
            printf("Disconnecting client %d\n", i);
            SDL_LockMutex(shared_pool.clients_mutex);
            shared_pool.clients[i].active = 0;
            SDL_UnlockMutex(shared_pool.clients_mutex);

            SDL_LockMutex(shared_pool.sending_mutex);
            Vector_push(shared_pool.sending, dcpack);
            SDL_UnlockMutex(shared_pool.sending_mutex);
          }
        }
      }

      // send information about projectiles dying
      for (int i = 0; i < Proj_num_projectiles(); i++) {
        if (Proj_server_should_kill(i)) {
          Proj_server_do_kill(i);
          UDPpacket *pack = Network_create_projectile_death_packet(i);

          SDL_LockMutex(shared_pool.sending_mutex);
          Vector_push(shared_pool.sending, pack);
          SDL_UnlockMutex(shared_pool.sending_mutex);
        }
      }

      last_time += delta;
    }
  }

  Server_Receiver_stop();
  Server_Sender_stop();

  Pool_deinit();

  SDLNet_Quit();
  SDL_Quit();
  return 0;
}
