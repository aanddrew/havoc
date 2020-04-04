#include "Vector.h"
#include <stdlib.h>
#include <string.h>

void Vector_init(Vector* vec) {
    vec->size = 4;
    vec->num = 0;
    vec->array = malloc(sizeof(void*) * vec->size);
}

void Vector_delete(Vector* vec) {
    free(vec->array);
}

void Vector_push(Vector* vec, void* element) {
    if (vec->num == vec->size) {
        vec->size *= 2;
        vec->array = realloc(vec->array, sizeof(void*) * vec->size);
    }

    vec->array[vec->num] = element;
    vec->num++;
}

void* Vector_get(Vector* vec, int index) {
    if (index >= vec->num || index < 0) {
        return NULL;
    }
    return vec->array[index];
}

void* Vector_pop(Vector* vec) {
    void* item = Vector_get(vec, vec->num - 1);
    vec->num--;
    return item;
}

void* Vector_get_last(Vector* vec) {
    return Vector_get(vec, vec->num-1);
}

void* Vector_get_first(Vector* vec) {
    return Vector_get(vec, 0);
}

void Vector_insert(Vector* vec, void* element, int index) {
    if (vec->num == vec->size) {
        vec->size *= 2;
        vec->array = realloc(vec->array, sizeof(void*) * vec->size);
    }

    for(int i = vec->num; i > index; i--) {
        vec->array[i] = vec->array[i-1];
    }
    vec->array[index] = element;
    vec->num++;
}

void Vector_set(Vector* vec, void* element, int index) {
    if (index >= vec->num || index < 0) {
        return;
    }
    vec->array[index] = element;
}

void Vector_reverse(Vector* vec) {
    for (int i = 0; i < vec->num /2; i++) {
        void* temp = Vector_get(vec, vec->num - 1 -i);
        Vector_set(vec, Vector_get(vec, i), vec->num -1 -i);
        Vector_set(vec, temp, i);
    }
}

Vector* Vector_combine(Vector* first, Vector* second) {
    int new_num = first->num + second->num;
    int new_size = first->size + second->size;

    Vector* new = malloc(sizeof(Vector));
    new->size = new_size;
    new->num  = new_num;

    new->array = malloc(sizeof(void*) * new->size);
    memcpy(new->array, first->array, sizeof(void*) * first->num);
    memcpy(new->array + first->num, second->array, sizeof(void*) * second->num);

    Vector_delete(first);
    Vector_delete(second);
    return new;
}
