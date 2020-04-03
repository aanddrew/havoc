#define ZN_IMPLEMENTATION
#include "Server.h"

#include "znet/znet.h"

GameState Master_GameState;

static zn_State* S;
static zn_Udp* udpserver;

#define MYDATA_BUFLEN 1024
typedef struct MyData {
    char buffer[MYDATA_BUFLEN];
    int index;
    int count;
} MyData;
static MyData* data;

void on_udp_recv(void *ud, zn_Udp *udp, unsigned err, unsigned count,
        const char *addr, unsigned port) {
    MyData *data = (MyData*)ud;

    if (err != ZN_OK) {
        printf("[%p] udp meet error when receiving message: %s\n",
                udp, zn_strerror(err));
        zn_deludp(udp);
        return;
    }

    ++data->count;
    printf("[%p] udp received message %d from %s:%d: %.*s (%d bytes)\n",
            udp, data->count, addr, port, (int)count, data->buffer, (int)count);

    zn_recvfrom(udp, data->buffer, MYDATA_BUFLEN, on_udp_recv, data);
}


void Server_init(short port) {
    data = malloc(sizeof(MyData));
    data->count = 0;
    data->index = 0;

    zn_initialize();

    S = zn_newstate();
    udpserver = zn_newudp(S, "192.168.1.3", 6969);
}

void Server_deinit() {
    free(data);
    
    zn_close(S);
    zn_deinitialize();
}

void Server_start() {
    zn_recvfrom(udpserver, data->buffer, MYDATA_BUFLEN,
            on_udp_recv, data);
    zn_run(S, ZN_RUN_LOOP);
}

void Server_stop() {
    zn_closeudp(udpserver);
    zn_deludp(udpserver);
}
