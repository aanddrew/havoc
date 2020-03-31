#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    void** array;
    int num;
    int size;
} Vector;

Vector* Vector_init();
void Vector_delete(Vector* vec);

void Vector_push(Vector* vec, void* element);
void* Vector_get(Vector* vec, int index);

void* Vector_pop(Vector* vec);

void* Vector_get_last(Vector* vec);
void* Vector_get_first(Vector* vec);

void Vector_insert(Vector* vec, void* element, int index);
void Vector_set(Vector* vec, void* element, int index);

void Vector_reverse(Vector* vec);

Vector* Vector_combine(Vector* first, Vector* second);

#endif
