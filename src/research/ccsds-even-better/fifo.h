#pragma once

#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t size;
    size_t head;
    size_t tail;
    bool is_full;
} fifo_t;

void fifo_init(fifo_t* fifo, uint8_t* data, size_t size) {
    fifo->data = data;
    fifo->size = size;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->is_full = false;
}

void fifo_push(fifo_t* fifo, uint8_t byte) {
    fifo->data[fifo->head] = byte;
    fifo->head += 1;
    fifo->head %= fifo->size;

    if (fifo->head == fifo->tail) {
        fifo->is_full = true;
    }
}

void fifo_pull(fifo_t* fifo, uint8_t* byte) {
    fifo->is_full = false;
    *byte = fifo->data[fifo->tail];
    fifo->tail += 1;
    fifo->tail %= fifo->size;
}



size_t fifo_fullness(const fifo_t* fifo) {
    return (fifo->head + fifo->size - fifo->tail) % fifo->size;
}
size_t fifo_size(const fifo_t* fifo) {
    return fifo->size;
}

size_t fifo_free_space(const fifo_t* fifo) {
    return fifo_size(fifo) - fifo_fullness(fifo);
}

bool fifo_push_arr(fifo_t* fifo, const uint8_t* data, size_t size) {
    if (size > fifo_free_space(fifo)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        fifo_push(fifo, data[i]);
    }
    return true;
}

bool fifo_pull_arr(fifo_t* fifo, uint8_t* data, size_t size) {
    if (size > fifo_fullness(fifo)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        fifo_pull(fifo, &data[i]);
    }
    return true;
}