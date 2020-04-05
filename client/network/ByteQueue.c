#include "ByteQueue.h"

void ByteQueue_init(ByteQueue* self, int size) {
    self->data = malloc(size);
    self->max_size = size;
    self->head = 0;
    self->tail = 0;
}
void ByteQueue_deinit(ByteQueue* self) {
    free(self->data);
    self->data = NULL;
}

int ByteQueue_getbytes(ByteQueue* self, Uint8* output, int num) {
    int offset = 0;
    if (self->tail < self->head) {
        offset = self->max_size;
    }
    //if there are'nt even this many bytes in the queue
    if (self->tail - self->head + self->max_size < num) {
        return 0;
    }

    for (int i = 0; i < num; i++) {
        output[i] = self->data[(i + self->head) % self->max_size];
    }
    self->head = (self->head + num) % self->max_size;
    return 1;
}

int ByteQueue_insert(ByteQueue* self, Uint8* input, int len) {
    if (ByteQueue_availabie(self) < len) {
        return 0;
        //resize queue maybe???, this way doesn't work
        self->max_size *= 2;
        self->data = realloc(self->data, self->max_size);
    }
    for (int i = 0; i < len; i++) {
        self->data[(self->tail + i) % self->max_size] = input[i];
    }
    self->tail = (self->tail + len) % self->max_size;
}

int ByteQueue_full(ByteQueue* self) {
    int offset = 0;
    if (self->tail < self->head) {
        offset = self->max_size;
    }
    return self->tail - self->head + offset;
}

int ByteQueue_availabie(ByteQueue* self) {
    return self->max_size - ByteQueue_full(self);
}

