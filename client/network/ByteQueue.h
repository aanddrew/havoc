#ifndef BYTEQUEUE_H
#define BYTEQUEUE_H

#include <SDL2/SDL.h>

//in this model the head is the most recent piece of data
typedef struct {
    Uint8* data;
    int max_size;

    int head;
    int tail;
} ByteQueue;

void ByteQueue_init(ByteQueue* self, int size);
void ByteQueue_deinit(ByteQueue* self);

int ByteQueue_getbytes(ByteQueue* self, Uint8* output, int num);
int ByteQueue_insert(ByteQueue*, Uint8* data, int len);

int ByteQueue_full(ByteQueue* self);
int ByteQueue_availabie(ByteQueue* self);

#endif
