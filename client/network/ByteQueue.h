#ifndef BYTEQUEUE_H
#define BYTEQUEUE_H

#include <SDL2/SDL.h>

//in this model the tail is the most recent piece of data
//the head is the oldest piece of data
typedef struct {
    Uint8* data;
    int max_size;

    int head;
    int tail;
} ByteQueue;

void ByteQueue_init(ByteQueue* self, int size);
void ByteQueue_deinit(ByteQueue* self);

//gives puts num bytes into output, and 'removes' them from the queue
//i.e. calling getbytes twice will return two consecutive pieces of data
int ByteQueue_getbytes(ByteQueue* self, Uint8* output, int num);
//insert len bytes of data at the tail of the queue
int ByteQueue_insert(ByteQueue*, Uint8* data, int len);

//returns the number of full spots in the queue
int ByteQueue_full(ByteQueue* self);
//returns the number of available spots in the queue
int ByteQueue_availabie(ByteQueue* self);

#endif
