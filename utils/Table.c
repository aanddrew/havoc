#include "Table.h"

#include <stdlib.h>
#include <string.h>

Table* Table_init() {
    Table* table = malloc(sizeof(Table));
    table->names = NULL;
    table->data = NULL;
    table->capacity = 0;
    table->size = 0;
    return table;
}

int Table_insert(Table* self, const char* name, void* item) {
    if (self->capacity == 0) {
        self->capacity = 4;
        self->names = malloc(sizeof(char*) * self->capacity);
        self->data = malloc(sizeof(void*) * self->capacity);
    }
    else if (self->size == self->capacity) {
        self->capacity *= 2;

        self->names = realloc(self->names, sizeof(char*) * self->capacity);
        self->data  = realloc(self->data,  sizeof(void*) * self->capacity);
    }
    for(int i = 0; i < self->size; i++) {
        if (!strcmp(name, self->names[i])) {
            return 0;
        }
    }

    self->names[self->size] = strdup(name);
    self->data[self->size] = item;
    self->size++;

    return 1;
}

void* Table_get(Table* self, const char* name) {
    for(int i = 0; i < self->size; i++) {
        if (!strcmp(name, self->names[i])) {
            return self->data[i];
        }
    }
    return NULL;
}

void Table_delete(Table* self) {
    for(int i = 0; i < self->size; i++) {
        free(self->names[i]);
    }
    free(self->names);
    free(self->data);
    free(self);
}
