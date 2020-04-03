 #include "Network.h"

#define ZN_IMPLEMENTATION
#include "../../server/znet/znet.h"
#include <stdio.h>

static zn_State* S;
static zn_Udp* udpclient;
static zn_Udp* udpserver;

#define MYDATA_BUFLEN 1024
typedef struct MyData {
    char buffer[MYDATA_BUFLEN];
    int idx;
    int count;
} MyData;

void client_init();
void receiver_init();
void client_close();
void receiver_close();

void Network_init() {
    zn_initialize();

    S = zn_newstate();

    client_init();
    receiver_init();
}

void Network_deinit() {
    client_close();
    receiver_close();

    zn_close(S);
    zn_deinitialize();
}

const char* connected_addr;

short port = 6969;
void Network_connect(const char* addr) {
    
}

/*Client stuff*/
void client_init() {
    const char* my_internal_ip = "192.168.1.3";
    udpclient = zn_newudp(S, my_internal_ip, 0);
    if (udpclient == NULL) {
        printf("Error, udpclient is NULL\n");
    }
}

void client_close() {
    zn_closeudp(udpclient);
    zn_deludp(udpclient);
}

void Network_send() {
    const char* msg = "Hello, World\n";
    int returned = zn_sendto(udpclient, msg, strlen(msg), "198.58.109.228", 6969);
    if (returned != ZN_OK) {
        printf("Error sending message %d\n", returned);
    }
}
/*Client end*/

void on_udp_recv(void *ud, zn_Udp *udp, unsigned err, unsigned count,
        const char *addr, unsigned port) {
    MyData *data = (MyData*)ud;

    if (err != ZN_OK) {
        fprintf(stderr, "[%p] udp meet error when receiving message: %s\n",
                udp, zn_strerror(err));
        zn_deludp(udp);
        return;
    }

    ++data->count;
    printf("[%p] udp received message%d from %s:%d: %.*s (%d bytes)\n",
            udp, data->count, addr, port, (int)count, data->buffer, (int)count);

    zn_recvfrom(udp, data->buffer, MYDATA_BUFLEN, on_udp_recv, data);
}

/*receiver stuff*/
void receiver_init() {
    udpserver = zn_newudp(S, "192.168.1.3", 6969);
}

void receiver_close() {
    zn_closeudp(udpserver);
    zn_deludp(udpserver);
}

void receiver_run() {
    MyData* data = (MyData*)malloc(sizeof(MyData));
    data->count = 0; /* we receive this count times */
    zn_recvfrom(udpserver, data->buffer, MYDATA_BUFLEN,
            on_udp_recv, data);

    zn_run(S, ZN_RUN_LOOP);

}
/* end receiver stuff */
